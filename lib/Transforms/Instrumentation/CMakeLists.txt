add_llvm_library(LLVMInstrumentation
  AddressSanitizer.cpp
  BoundsChecking.cpp
  DataFlowSanitizer.cpp
  GCOVProfiling.cpp
  MemorySanitizer.cpp
  Instrumentation.cpp
  InstrProfiling.cpp
  PGOInstrumentation.cpp
  SafeStack.cpp
  SanitizerCoverage.cpp
  ThreadSanitizer.cpp
  Avery.cpp
  AveryAugmentArgs.cpp
  AveryMemMask.cpp
  AveryDataStack.cpp
  AveryLoopMaskElimination.cpp
  ExpandByVal.cpp
  ExpandVarArgs.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms
  )

add_dependencies(LLVMInstrumentation intrinsics_gen)
