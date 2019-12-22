#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include <iostream>

using namespace llvm;

/*
    务必使用llvm-3.3
*/
int foo(int x) {
    return 2 * x;
}


int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    LLVMContext context;
    std::unique_ptr<Module> module = std::make_unique<Module>("test", context);

    //在IR中声明一个函数，注意我们并不会在IR中定义foo，我们会将这个IR中声明的函数映射到C++代码中的函数
    Function *f = Function::Create(FunctionType::get(Type::getInt32Ty(context),
                                                               {Type::getInt32Ty(context)}, false),
                                             Function::ExternalLinkage, "foo", module.get());

    //在IR中声明一个函数bar，我们会用IR定义这个函数
    Function *b = Function::Create(FunctionType::get(Type::getInt32Ty(context),
                                                               {}, false),
                                             Function::ExternalLinkage, "bar", module.get());

    //创建函数bar的代码块
    BasicBlock *entry = BasicBlock::Create(context, "entry", b);
    IRBuilder<> builder(entry);

    //调用函数foo
    Value *ret = builder.CreateCall(f, builder.getInt32(10));
//    Value *ret = builder.getInt32(10);
    //返回值
    builder.CreateRet(ret);

    ExecutionEngine *ee = EngineBuilder(std::move(module)).create();


    //将外部的C++代码中的全局函数映射到IR代码中，IR代码中只有声明
    ee->addGlobalMapping(f, (void *) foo);
    std::vector<GenericValue> noargs;
    GenericValue gv = ee->runFunction(b, noargs);

    void *barAddr = ee->getPointerToFunction(b);
    typedef int (*FuncType)();
    FuncType barFunc = (FuncType) barAddr;

    std::cout << barFunc() << std::endl;
    return 0;
}

