
# to make executable for client/server

# downloads sfml and imgui from github
# $ cmake -B build

# compile the executables
# $ cmake --build build

# you now have 2 executables
# to run the client
# $ ./build/client/mousetrace_client









# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# load compilers for C and C++
project(mousetrace LANGUAGES C CXX)

# keeps clangd aware of all compilation flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIMEH_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# instead of downloading SFML, CMake reaches out to github and
# downloads the source code for SFML and perpares it to be built
# alongside your project
include(FetchContent)
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 3.0.2
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM)
FetchContent_MakeAvailable(SFML)
#################################################################
#adding IMGUI and IMGUI-SFML
FetchContent_Declare(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.5
  GIT_SHALLOW ON
  SOURCE_SUBDIR ""
)
# IMGUI dosn't use CMake natively, so we just manually download its files
FetchContent_MakeAvailable(imgui)

# tell the IMGUI-SFML bridge where we downloaded the core IMGUI files
set(IMGUI_DIR ${imgui_SOURCE_DIR})

# Tell ImGui-SFML to use our fetched SFML instead of searching the hard drive
set(IMGUI_SFML_FIND_SFML OFF)

# fetchC IMGUI_SFML (the bridge for SFML 3)
FetchContent_Declare(imgui_sfml
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
  GIT_TAG v3.0   # Version specifically updated for SFML 3
  GIT_SHALLOW ON
  SYSTEM
)

FetchContent_MakeAvailable(imgui_sfml)


# tell cmake to execute the CMakeLists.txt in these folders
add_subdirectory(client)
add_subdirectory(server)


