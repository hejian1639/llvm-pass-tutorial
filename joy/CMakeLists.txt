
set(LLVM_LINK_COMPONENTS
        Core
        )



include(AddLLVM)

add_llvm_example(joy ch2_toy.cpp)
