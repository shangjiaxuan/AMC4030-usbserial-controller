cmake_minimum_required(VERSION 3.15)
project(AMC4030_usbserial_protocol C)

# Option to build as shared library (default OFF -> build STATIC)
option(AMC4030_USB_PROTO_DYNAMIC "Build protocol as shared library" OFF)

set(PROTO_SRC_DIR "${CMAKE_SOURCE_DIR}/../../Protocol")

set(SOURCES
    ${PROTO_SRC_DIR}/AMC4030_usb_com_protocol_Shared.c
    ${PROTO_SRC_DIR}/AMC4030_usb_com_protocol_Win32.c
    ${PROTO_SRC_DIR}/AMC4030_usb_com_protocol_Linux.c
)

set(HEADERS
    ${PROTO_SRC_DIR}/AMC4030_usb_com_protocol.h
)

# Create library
if(AMC4030_USB_PROTO_DYNAMIC)
    add_library(AMC4030_usb_proto SHARED ${SOURCES} ${HEADERS})
    target_compile_definitions(AMC4030_usb_proto PRIVATE AMC4030_USB_PROTO_DYNAMIC)
else()
    add_library(AMC4030_usb_proto STATIC ${SOURCES} ${HEADERS})
endif()

# Common compile options and definitions
target_compile_features(AMC4030_usb_proto PRIVATE c_std_11)

if(MSVC)
    target_compile_definitions(AMC4030_usb_proto PRIVATE AMC4030_USB_PROTO_BUILD)
    target_compile_definitions(AMC4030_usb_proto PRIVATE _CONSOLE)
    # Keep Visual Studio's warning level and SDL checks left to the user; map common flags
    target_compile_options(AMC4030_usb_proto PRIVATE /W3)
else()
    target_compile_definitions(AMC4030_usb_proto PRIVATE AMC4030_USB_PROTO_BUILD)
    target_compile_options(AMC4030_usb_proto PRIVATE -Wall -Wextra)
endif()

# Include directory
target_include_directories(AMC4030_usb_proto PUBLIC ${PROTO_SRC_DIR})

# Set output directories similar to VS projects (Build/)
set_target_properties(AMC4030_usb_proto PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    OUTPUT_NAME "AMC4030-usbserial-protocol"
)

# Allow user to choose architecture by setting CMAKE_GENERATOR or CMAKE_TOOLCHAIN_FILE

# Install rules (optional)
install(TARGETS AMC4030_usb_proto
        ARCHIVE DESTINATION Build
        LIBRARY DESTINATION Build
        RUNTIME DESTINATION Build)

message(STATUS "Protocol sources: ${SOURCES}")
