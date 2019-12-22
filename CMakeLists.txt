cmake_minimum_required(VERSION 3.4)

# we need LLVM_HOME in order not automatically set LLVM_DIR
#if(NOT DEFINED ENV{LLVM_HOME})
#    message(FATAL_ERROR "$LLVM_HOME is not defined")
#else ()
#    set(ENV{LLVM_DIR} $ENV{LLVM_HOME}/lib/cmake/llvm)
#endif()


find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
        )

add_subdirectory(skeleton)  # Use your pass name here.
add_subdirectory(test)  # Use your pass name here.
add_subdirectory(joy)  # Use your pass name here.

#add_custom_target(order DEPENDS SkeletonPass test)
