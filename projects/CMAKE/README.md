This CMake project provides a simple way to build the protocol library found in the `Protocol/` folder.

Usage (from this folder):

- Configure a static build (default):
  cmake -S . -B build
  cmake --build build --config Release

- Configure a shared/dynamic build:
  cmake -S . -B build -DAMC4030_USB_PROTO_DYNAMIC=ON
  cmake --build build --config Release

Notes:
- Outputs are placed in the top-level `Build/` directory to match the existing VS projects.
- On Windows use an appropriate generator (Visual Studio) to build Release/Debug configs and architectures.
