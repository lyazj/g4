#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.27)
project(Telescope)

#----------------------------------------------------------------------------
# Setup include directory for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
#
find_package(ROOT)
set(ROOT_CC_FLAG_LIST ${ROOT_CC_FLAGS})
set(ROOT_CXX_FLAG_LIST ${ROOT_CXX_FLAGS})
separate_arguments(ROOT_CC_FLAG_LIST)
separate_arguments(ROOT_CXX_FLAG_LIST)
add_compile_options(${ROOT_CXX_FLAG_LIST})
add_compile_definitions(${ROOT_DEFINITIONS})
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})

#----------------------------------------------------------------------------
# General compile flags
#
add_compile_options(-O3)
add_compile_options(-Wall -Wshadow -Wextra)
add_compile_definitions(BASEDIR="${PROJECT_SOURCE_DIR}")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the library, and link it to ROOT libraries
#
add_library(Common SHARED ${sources} ${headers})
target_link_libraries(Common PUBLIC ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Add executables, and link them to the Common library
#
add_executable(Telescope Telescope.cc)
target_link_libraries(Telescope Common)
add_executable(DrawLise DrawLise.cc)
target_link_libraries(DrawLise Common)
