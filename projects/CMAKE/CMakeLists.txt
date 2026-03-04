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

# Create libraries

add_library(AMC4030_usb_proto_shared SHARED ${SOURCES} ${HEADERS})
target_compile_definitions(AMC4030_usb_proto_shared PRIVATE AMC4030_USB_PROTO_DYNAMIC)

add_library(AMC4030_usb_proto_static STATIC ${SOURCES} ${HEADERS})

# Common compile options and definitions
target_compile_features(AMC4030_usb_proto_shared PRIVATE c_std_11)
target_compile_features(AMC4030_usb_proto_static PRIVATE c_std_11)

if(MSVC)
    target_compile_definitions(AMC4030_usb_proto_shared PRIVATE AMC4030_USB_PROTO_BUILD)
    target_compile_definitions(AMC4030_usb_proto_shared PRIVATE _CONSOLE)
    # Keep Visual Studio's warning level and SDL checks left to the user; map common flags
    target_compile_options(AMC4030_usb_proto_shared PRIVATE /W3)

    target_compile_definitions(AMC4030_usb_proto_static PRIVATE AMC4030_USB_PROTO_BUILD)
    target_compile_definitions(AMC4030_usb_proto_static PRIVATE _CONSOLE)
    # Keep Visual Studio's warning level and SDL checks left to the user; map common flags
    target_compile_options(AMC4030_usb_proto_static PRIVATE /W3)
else()
    target_compile_definitions(AMC4030_usb_proto_shared PRIVATE AMC4030_USB_PROTO_BUILD)
    target_compile_options(AMC4030_usb_proto_shared PRIVATE -Wall -Wextra)
    target_compile_definitions(AMC4030_usb_proto_static PRIVATE AMC4030_USB_PROTO_BUILD)
    target_compile_options(AMC4030_usb_proto_static PRIVATE -Wall -Wextra)
endif()

# Include directory
target_include_directories(AMC4030_usb_proto_shared PUBLIC ${PROTO_SRC_DIR})
target_include_directories(AMC4030_usb_proto_static PUBLIC ${PROTO_SRC_DIR})

# Set output directories similar to VS projects (Build/)
set_target_properties(AMC4030_usb_proto_shared PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    OUTPUT_NAME "AMC4030_usbserial"
)
set_target_properties(AMC4030_usb_proto_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../Build"
    OUTPUT_NAME "AMC4030_usbserial"
)
# Allow user to choose architecture by setting CMAKE_GENERATOR or CMAKE_TOOLCHAIN_FILE

# Install rules (optional)
install(TARGETS AMC4030_usb_proto_shared
        ARCHIVE DESTINATION Build
        LIBRARY DESTINATION Build
        RUNTIME DESTINATION Build)

install(TARGETS AMC4030_usb_proto_static
        ARCHIVE DESTINATION Build
        LIBRARY DESTINATION Build
        RUNTIME DESTINATION Build)

message(STATUS "Protocol sources: ${SOURCES}")
