

SET(CMAKE_C_COMPILER "/Users/jianhe/llvm-project/clang/cmake-build-debug/bin/clang")
SET(CMAKE_CXX_COMPILER "/Users/jianhe/llvm-project/clang/cmake-build-debug/bin/clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 \
-isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1 \
-isystem '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/usr/include' \
-Xclang -load -Xclang /Users/jianhe/llvm-pass-tutorial/cmake-build-debug/skeleton/libSkeletonPass.so")

add_executable(test
        feature.cpp
        )

