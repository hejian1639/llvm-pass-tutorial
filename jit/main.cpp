#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/ManagedStatic.h"
#include <iostream>

using namespace llvm;

int foo(int x) {
    return 2 * x;
}

template<typename R, typename... Args>
struct function_traits_helper {
    static constexpr auto param_count = sizeof...(Args);
    using return_type = R;

    using param_type = std::tuple<Args...>;
};

template<typename Func>
struct function_traits {
};


// int(int, int)
template<typename R, typename... Args>
struct function_traits<R(Args...)> : public function_traits_helper<R, Args...> {
};

struct Context {

    std::vector<Type *> argumentTypes;
    std::vector<Value *> arguments;
    Type *returnType;
};

template<typename T>
Type *convert(LLVMContext &C);

template<>
Type *convert<int>(LLVMContext &C) {
    return Type::getInt32Ty(C);
}

Value *convert(IRBuilder<> &builder, int t) {
    return builder.getInt32(t);
}

template<typename Tuple, size_t N>
struct tuple_convert {
    static void convert(std::vector<Type *> &arguments, LLVMContext &context) {
        tuple_convert<Tuple, N - 1>::convert(arguments, context);
        arguments.push_back(::convert < std::tuple_element<N - 1, Tuple>::type > (context));
    }

    static void convert(const Tuple &t, std::vector<Value *> &arguments, IRBuilder<> &builder) {
        tuple_convert<Tuple, N - 1>::convert(t, arguments, builder);
        arguments.push_back(::convert(builder, std::get<N - 1>(t)));
    }
};

// 类模板的特化版本
template<typename Tuple>
struct tuple_convert<Tuple, 1> {
    static void convert(std::vector<Type *> &arguments, LLVMContext &context) {
        arguments.push_back(::convert < typename std::tuple_element<0, Tuple>::type > (context));
    }

    static void convert(const Tuple &t, std::vector<Value *> &arguments, IRBuilder<> &builder) {
        arguments.push_back(::convert(builder, std::get<0>(t)));
    }

};


template<class Fun, class ... Args>
auto jit_bind(LLVMContext &context, ExecutionEngine *ee, Module *module, Fun fun, Args ... args) {
    using FunOriginalTy = std::remove_pointer_t<std::decay_t<Fun>>;
    static_assert(std::is_function<FunOriginalTy>::value,
                  "easy::jit: supports only on functions and function pointers");
    Context c;
    c.returnType = convert<typename function_traits<FunOriginalTy>::return_type>(context);
    using Tuple=typename function_traits<FunOriginalTy>::param_type;
    tuple_convert<Tuple, std::tuple_size<Tuple>::value>::convert(c.argumentTypes, context);

    Function *inner = Function::Create(FunctionType::get(c.returnType, c.argumentTypes, false),
                                       Function::ExternalLinkage, "inner", module);
    //在IR中声明一个函数bar，我们会用IR定义这个函数
    Function *wrapper = Function::Create(FunctionType::get(c.returnType, {}, false),
                                         Function::ExternalLinkage, "wrapper", module);

    BasicBlock *entry = BasicBlock::Create(context, "entry", wrapper);
    IRBuilder<> builder(entry);
    tuple_convert<Tuple, std::tuple_size<Tuple>::value>::convert(std::make_tuple(args...), c.arguments, builder);

    //调用函数foo
    Value *ret = builder.CreateCall(inner, c.arguments);
    //返回值
    builder.CreateRet(ret);

    //将外部的C++代码中的全局函数映射到IR代码中，IR代码中只有声明
    ee->addGlobalMapping(inner, (void *) fun);

    return ee->getFunctionAddress("wrapper");

}

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    LLVMContext context;
    std::unique_ptr<Module> module = std::make_unique<Module>("test", context);
    Module *pModule = module.get();


    ExecutionEngine *ee = EngineBuilder(std::move(module)).create();

    typedef int (*FuncType)();

    auto barAddr = jit_bind(context, ee, pModule, foo, 10);

    FuncType barFunc = (FuncType) barAddr;

    std::cout << barFunc() << std::endl;

    delete ee;
    llvm_shutdown();


    return 0;
}

