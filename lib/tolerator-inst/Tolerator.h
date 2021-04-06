
#pragma once


#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"


namespace tolerator {


enum class AnalysisType {
  LOGGING,
  IGNORING,
  DEFAULTING,
  BYPASSING
};


struct Tolerator : public llvm::ModulePass {
  static char ID;

  AnalysisType analysisType;
  Tolerator(AnalysisType analysisType) : llvm::ModulePass(ID), analysisType(analysisType) {}

  bool runOnModule(llvm::Module& m) override;
  void analyzeAlloca(llvm::Instruction& Inst, llvm::FunctionCallee calleeAlloca, llvm::LLVMContext& context);
  void analyzeLoad(llvm::Instruction& Inst, llvm::FunctionCallee calleeLoad, llvm::LLVMContext& context);
  void analyzeStore(llvm::Instruction& Inst, llvm::FunctionCallee calleeStore, llvm::LLVMContext& context);
};


}  // namespace tolerator


