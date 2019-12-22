cmake_minimum_required(VERSION 3.4)



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
add_subdirectory(jit)  # Use your pass name here.
add_subdirectory(HowToUseJIT)  # Use your pass name here.
add_subdirectory(HowToUseLLJIT)  # Use your pass name here.

