#if defined(__linux__)

#include "AMC4030_usb_com_protocol_Internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

static unsigned long get_millis()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long)(ts.tv_sec * 1000UL + ts.tv_nsec / 1000000UL);
}

void AMC4030_usb_protocol_LockContext(AMC4030_usb_protocol_context* obj)
{
    pthread_mutex_lock(&obj->lock);
}

void AMC4030_usb_protocol_UnLockContext(AMC4030_usb_protocol_context* obj)
{
    pthread_mutex_unlock(&obj->lock);
}

static int configure_serial_port(int fd)
{
    struct termios tio;
    if (tcgetattr(fd, &tio) < 0) return -1;

    cfmakeraw(&tio);

    // Baud 115200
    if (cfsetispeed(&tio, B115200) < 0) return -1;
    if (cfsetospeed(&tio, B115200) < 0) return -1;

    // 8N1
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;

    // disable hw flow control
    tio.c_cflag &= ~CRTSCTS;

    // non canonical, no echo, no signals
    tio.c_lflag = 0;
    tio.c_oflag = 0;
    tio.c_iflag = 0;

    // timeouts: VMIN=0 VTIME=1 (tenths of second) -> read will wait up to 100ms
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 1; // 100ms

    if (tcsetattr(fd, TCSANOW, &tio) < 0) return -1;

    // Drain and flush
    tcflush(fd, TCIOFLUSH);
    return 0;
}

int AMC4030_usb_protocol_FinishCommand(AMC4030_usb_protocol_context* obj)
{
    // make checksum and write size header
    {
        if (obj->write_bytes + 1 > 532)
        {
            return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
        }
        obj->write_bytes += 1;
        memcpy(obj->send_buffer, &obj->write_bytes, 2);
        obj->send_buffer[obj->write_bytes - 1] = AMC4030_usb_protocol_calculate_checksum(obj->write_bytes - 1, obj->send_buffer);
    }

    // send and receive
    {
        // flush input/output
        tcflush(obj->fd, TCIOFLUSH);

        ssize_t written = write(obj->fd, obj->send_buffer, obj->write_bytes);
        if (written != obj->write_bytes)
        {
            return AMC4030_USB_PROTO_COMM_WRITE_ERROR;
        }

        obj->read_bytes = 0;
        uint16_t expected_size = 5;
        int expected_size_read = 0;
        int four_byte_verified = 0;
        int fifth_byte_verified = 0;
        unsigned long start = get_millis();

        while (expected_size > obj->read_bytes)
        {
            // wait for data with a short sleep
            unsigned long now = get_millis();
            if (now - start >= 1000)
            {
                return AMC4030_USB_PROTO_COMM_READ_ERROR;
            }

            // read available data
            ssize_t r = read(obj->fd, obj->recv_buffer + obj->read_bytes, sizeof(obj->recv_buffer) - obj->read_bytes);
            if (r < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // no data, sleep a little
                    struct timespec ts = {0, 1000000}; // 1ms
                    nanosleep(&ts, NULL);
                    continue;
                }
                return AMC4030_USB_PROTO_COMM_READ_ERROR;
            }
            if (r == 0)
            {
                // no data available, short sleep
                struct timespec ts = {0, 1000000}; // 1ms
                nanosleep(&ts, NULL);
                continue;
            }
            obj->read_bytes += (uint16_t)r;

            if (!expected_size_read && obj->read_bytes > 2)
            {
                memcpy(&expected_size, obj->recv_buffer, 2);
                if (expected_size < 5)
                {
                    return AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR;
                }
                expected_size_read = 1;
            }
            if (!four_byte_verified && obj->read_bytes >= 4)
            {
                if (memcmp(obj->recv_buffer, "erro", 4) == 0)
                {
                    return AMC4030_USB_PROTO_DEVICE_RECIEVE_ERROR;
                }
                uint16_t code;
                memcpy(&code, obj->recv_buffer + 2, 2);
                if (code != obj->current_command)
                {
                    return AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR;
                }
                four_byte_verified = 1;
            }
            if (!fifth_byte_verified && obj->read_bytes >= 5)
            {
                if (obj->recv_buffer[4] != 0)
                {
                    return AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR;
                }
                fifth_byte_verified = 1;
            }
            if (obj->read_bytes >= 512)
            {
                return AMC4030_USB_PROTO_COMM_READ_ERROR;
            }
        }

        if (obj->read_bytes < 5)
        {
            return AMC4030_USB_PROTO_COMM_READ_ERROR;
        }
        if (!expected_size_read || obj->read_bytes != expected_size)
        {
            return AMC4030_USB_PROTO_COMM_READ_ERROR;
        }
    }

    // validate response
    {
        if (obj->recv_buffer[obj->read_bytes - 1] != AMC4030_usb_protocol_calculate_checksum(obj->read_bytes - 1, obj->recv_buffer))
        {
            return AMC4030_USB_PROTO_RESPONSE_FORMAT_ERROR;
        }
    }
    return 0;
}

AMC4030_usb_protocol_context* AMC4030_usb_protocol_Create(const char* commfile, int* error)
{
    AMC4030_usb_protocol_context* obj = NULL;
    int mapped_error = AMC4030_USB_PROTO_NO_ERROR;

    int fd = open(commfile, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        switch (errno)
        {
            case EACCES:
                mapped_error = AMC4030_USB_PROTO_COMM_INIT_FILE_NO_PERM;
                break;
            case ENOENT:
                mapped_error = AMC4030_USB_PROTO_COMM_INIT_FILE_NOT_FOUND;
                break;
            case EBUSY:
                mapped_error = AMC4030_USB_PROTO_COMM_INIT_FILE_IN_USE;
                break;
            default:
                mapped_error = AMC4030_USB_PROTO_COMM_INIT_GENERIC_ERROR;
                break;
        }
        if (error) *error = mapped_error;
        return NULL;
    }

    if (configure_serial_port(fd) != 0)
    {
        mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
        close(fd);
        if (error) *error = mapped_error;
        return NULL;
    }

    obj = (AMC4030_usb_protocol_context*)calloc(1, sizeof(AMC4030_usb_protocol_context));
    if (!obj)
    {
        mapped_error = AMC4030_USB_PROTO_COMM_INIT_OUT_OF_MEMORY;
        close(fd);
        if (error) *error = mapped_error;
        return NULL;
    }

    obj->fd = fd;
    if (pthread_mutex_init(&obj->lock, NULL) != 0)
    {
        mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
        close(fd);
        free(obj);
        if (error) *error = mapped_error;
        return NULL;
    }

    if (error) *error = mapped_error;
    return obj;
}

void AMC4030_usb_protocol_Destroy(AMC4030_usb_protocol_context* obj)
{
    if (!obj) return;
    pthread_mutex_destroy(&obj->lock);
    if (obj->fd >= 0) close(obj->fd);
    free(obj);
}
#endif