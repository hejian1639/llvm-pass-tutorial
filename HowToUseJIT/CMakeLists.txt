set(LLVM_LINK_COMPONENTS
        Core
        ExecutionEngine
        Interpreter
        Support
        nativecodegen
        MCJIT
        OrcJIT
        )

add_llvm_example(HowToUseJIT
        HowToUseJIT.cpp
        )
