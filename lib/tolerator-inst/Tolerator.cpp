

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "Tolerator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;
using tolerator::Tolerator;


namespace tolerator {

char Tolerator::ID = 0;

}


bool
Tolerator::runOnModule(Module& m) {
  auto& context = m.getContext();

  // This analysis just prints a message when the program starts or exits.
  // You should modify this code as you see fit.
  auto* voidTy = Type::getVoidTy(context);
  auto* int64Ty = Type::getInt64Ty(context);
  auto* int1Ty = Type::getInt1Ty(context);

 // llvm::GlobalVariable* gvar_ptr_abc = new GlobalVariable(/*Module=*/*mod, 
 //       /*Type=*/PointerTy_0,
 //       /*isConstant=*/false,
 //       /*Linkage=*/GlobalValue::CommonLinkage,
 //       /*Initializer=*/0, // has initializer, specified below
 //       /*Name=*/"abc");
 // gvar_ptr_abc->setAlignment(4);

  //auto helloworld = m.getOrInsertFunction("ToLeRaToR_helloworld", voidTy);
  //appendToGlobalCtors(m, llvm::cast<Function>(helloworld.getCallee()), 0);

  //auto goodbyeworld = m.getOrInsertFunction("ToLeRaToR_goodbyeworld", voidTy);
  //appendToGlobalDtors(m, llvm::cast<Function>(goodbyeworld.getCallee()), 0);

  // Declare the saveRange function
  auto* helperTy1 = FunctionType::get(voidTy, {int64Ty, int64Ty}, false);
  auto calleeAlloca = m.getOrInsertFunction("ToLeRaToR_saveRange", helperTy1);

  auto* helperTy2 = FunctionType::get(int1Ty, {int64Ty, int64Ty}, false);
  auto calleeLoad = m.getOrInsertFunction("ToLeRaToR_loadValidate", helperTy2);

  for (auto &Func : m) {
    for (auto &BB : Func) {
      for (auto &Inst : BB) {
        StringRef Name = Inst.getOpcodeName();
        if (Name == "alloca") {
          analyzeAlloca(Inst, calleeAlloca, context);
        }
        else if (Name == "load") {
          analyzeLoad(Inst, calleeLoad, context);
        }
        else if (Name == "store") {
          //analyzeStore(Inst);
        }
        else {
          continue;
        }
      }
    }
  }
  return true;
}

void
Tolerator::analyzeAlloca(llvm::Instruction &Inst, llvm::FunctionCallee callee, llvm::LLVMContext& context) {
  uint64_t ArraySize = 1;
  AllocaInst *AI = dyn_cast<AllocaInst>(&Inst);
  if (AI->isArrayAllocation()) {
    const ConstantInt *CI = dyn_cast<ConstantInt>(AI->getArraySize());
    assert(CI && "non-constant array size");
    ArraySize = CI->getZExtValue();
  }
  Type *Ty = AI->getAllocatedType();
  uint64_t SizeInBytes = AI->getModule()->getDataLayout().getTypeAllocSize(Ty);

  //errs() << "Inst-> " << Inst << "\n";
  //errs() <<  "Inst.getArraySize() " << SizeInBytes * ArraySize << "\n";
  llvm::IRBuilder<> builder(&Inst);
  builder.SetInsertPoint(AI->getNextNode());
  auto* int64Ty = Type::getInt64Ty(context);
  auto* allocaAddr = builder.CreatePointerCast(AI, int64Ty, "start_addr");
  builder.CreateCall(callee, {allocaAddr, builder.getInt64(SizeInBytes * ArraySize)});
}

void
Tolerator::analyzeLoad(llvm::Instruction &Inst, llvm::FunctionCallee callee, llvm::LLVMContext& context) {
  auto* int64Ty = Type::getInt64Ty(context);

  LoadInst *LI = dyn_cast<LoadInst>(&Inst);
  llvm::IRBuilder<> builder(&Inst);
  //gets the address of the load instruction and cast to ptr
  auto* loadAddr = builder.CreatePointerCast(LI->getPointerOperand(), int64Ty, "loadAddr");
  auto loadSize =  LI->getModule()->getDataLayout().getTypeStoreSize(LI->getType());
  auto sizeObj = ConstantInt::get(int64Ty, loadSize);
  // Insert a CallInst just before the load.
  auto* load_res = builder.CreateCall(callee, {loadAddr, sizeObj});
}