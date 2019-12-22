
set(LLVM_LINK_COMPONENTS
        Core
        ExecutionEngine
        Interpreter
        Support
        nativecodegen
        MCJIT
        OrcJIT
        )



include(AddLLVM)

add_llvm_executable(jit main.cpp)
