
set(LLVM_LINK_COMPONENTS
        Core
        )



include(AddLLVM)

add_llvm_executable(joy ch2_toy.cpp)
