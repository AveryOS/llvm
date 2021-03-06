//===-- CFGMST.h - Minimum Spanning Tree for CFG ----------------*- C++ -*-===//
//
//                      The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a Union-find algorithm to compute Minimum Spanning Tree
// for a given CFG.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Support/BranchProbability.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <string>
#include <utility>
#include <vector>

namespace llvm {

class Avery : public ModulePass {
 public:
  explicit Avery()
      : ModulePass(ID) {}
  bool runOnModule(Module &M) override;
  static char ID;
  const char *getPassName() const override { return "Avery"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
  }

  struct Range {
    bool Unknown;
    int64_t RelStart;
    int64_t RelEnd;

    static Range top();
    static Range unknown();
    static Range exact();

    static std::string num(int64_t n);
    bool allowed();
    std::string str();
    static int64_t offset(int64_t base, int64_t offset);
    Range offset(int64_t o) const;
    Range widen(Range old);
  };

  typedef DenseMap<Value *, Range> State;


 private:
  Function *Func;
  Type *Int32Ty;
  Type *IntPtrTy;
  Type *StackPtrTy;
  const DataLayout *DL;
  Value *Mask;
  Value *UnsafeStackPtr = nullptr;

  void augmentArgs(Module &M);
  void memMask(Function &F);
  bool eliminateMasks(Function &F, DenseSet<Value *> &prot);
  void splitStacks(Function &F);

  uint64_t getStaticAllocaAllocationSize(const AllocaInst* AI);
  void findInsts(Function &F,
                            SmallVectorImpl<AllocaInst *> &StaticAllocas,
                            SmallVectorImpl<AllocaInst *> &DynamicAllocas,
                            SmallVectorImpl<Argument *> &ByValArguments);

  AllocaInst *
  createStackRestorePoints(Function &F,
                                      Value *StaticTop, bool NeedDynamicTop);

  void moveDynamicAllocasToUnsafeStack(
      Function &F, AllocaInst *DynamicTop,
      ArrayRef<AllocaInst *> DynamicAllocas);

  Value *moveStaticAllocasToUnsafeStack(
      Function &F, ArrayRef<AllocaInst *> StaticAllocas,
      ArrayRef<Argument *> ByValArguments);

  void ExecuteI(State &R, Instruction *I, bool Widen);
  void ExecuteB(State &InState, BasicBlock *BB, bool Widen);
  void Join(State &A, State &B);

  void protectFunction(Function &F, DenseSet<Value *> &prot, Value *Mask);
  bool safePtr(Value *I, DenseSet<Value *> &prot, SmallSet<Value *, 8> &phis, int64_t offset, int64_t size, Value *&target, bool CanOffset);
  void protectValueAndSeg(Function &F, DenseSet<Value *> &prot, Instruction *I, unsigned PtrOp, Value *Mask);
  Value *protectValue(Function &F, DenseSet<Value *> &prot, Use &PtrUse, Value *Mask, bool CanOffset);
};


bool operator!=(const Avery::Range& lhs, const Avery::Range& rhs);

} // end namespace llvm
