#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Casting.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Support/InitLLVM.h"
#include <iostream>

using namespace llvm;
using namespace llvm::orc;

ExitOnError ExitOnErr;

/*
    务必使用llvm-3.3
*/
extern "C" int foo(int x) {
    return 2 * x;
}


int main(int argc, char *argv[]) {
    // Initialize LLVM.
    InitLLVM X(argc, argv);

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    cl::ParseCommandLineOptions(argc, argv, "HowToUseLLJIT");
    ExitOnErr.setBanner(std::string(argv[0]) + ": ");

    auto Context = std::make_unique<LLVMContext>();
    auto module = std::make_unique<Module>("test", *Context);

    //在IR中声明一个函数，注意我们并不会在IR中定义foo，我们会将这个IR中声明的函数映射到C++代码中的函数
    auto f = module->getOrInsertFunction("foo", Type::getInt32Ty(*Context),
                                         Type::getInt32Ty(*Context));
    //在IR中声明一个函数bar，我们会用IR定义这个函数
    Function *barFunction = Function::Create(FunctionType::get(Type::getInt32Ty(*Context),
                                                       {}, false),
                                     Function::ExternalLinkage, "bar", module.get());

    //创建函数bar的代码块
    BasicBlock *entry = BasicBlock::Create(*Context, "entry", barFunction);
    IRBuilder<> builder(entry);

    //用一个局部变量获取全局变量v的值
    Value *v = builder.getInt32(10);
    //调用函数foo
    Value *ret = builder.CreateCall(f, v);
//    Value *ret = builder.getInt32(10);
    //返回值
    builder.CreateRet(ret);

    auto J = ExitOnErr(LLJITBuilder().create());

    ThreadSafeModule M(std::move(module), std::move(Context));

    ExitOnErr(J->addIRModule(std::move(M)));

    // Look up the JIT'd function, cast it to a function pointer, then call it.
    auto barSym = ExitOnErr(J->lookup("bar"));
    int (*bar)() = (int (*)()) barSym.getAddress();

    int Result = bar();
    std::cout << "bar() = " << Result << "\n";

    return 0;
}

