
set(LLVM_LINK_COMPONENTS
        Core
        OrcJIT
        Support
        nativecodegen
        )



include(AddLLVM)

add_llvm_executable(jit ch3_toy.cpp)
