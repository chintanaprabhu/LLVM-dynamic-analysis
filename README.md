Software Engineering - Fault tolerance analysis using LLVM
==========================================================

This project demonstrates fault tolerance based on LLVM. It implements dynamic analyses by using LLVM for static program instrumentation.

This demonstrates protection of programs from different types of common bugs by checking a few simple policies at runtime. These are all standard bugs that will cause a program to crash, which can cause problems with availability.

1. Heap, global, and stack pointer accesses. Any (1) load from or (2) store to a pointer that occurs in the given IR must access an address that is a valid global, local variable, or heap allocated memory that occurs within the provided IR. Both spatial and temporal safety should be enforced. Once memory is no longer temporally valid, accesses should be invalid. You may assume that all heap memory will be allocated via a call to malloc(). Allocated heap memory will only be freed via a call to free().
2. Invalid frees. Any call to free() must pass a pointer that is either valid or NULL.
3. Divide by zero bugs. Any integer division must have a nonzero denominator.

Analysis 1 (Terminating with warnings)

The simplest way to handle incorrect behaviors is simply to terminate the running program and print (log) an error message describing what form of error has occurred. Handles each of the three classes of bugs by simply printing out an error message to stderr and ending the program, making the program return -1 to the system to indicate an error.

Building with CMake

1. Clone the demo repository.

        git clone https://github.com/chintanaprabhu/LLVM-dynamic-analysis.git

2. Create a new directory for building.

        mkdir tolerantbuild

3. Change into the new directory.

        cd tolerantbuild

4. Run CMake with the path to the LLVM source.

        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=True \
            -DLLVM_DIR=</path/to/LLVM/build>/lib/cmake/llvm/ ../LLVM-dynamic-analysis

5. Run make inside the build directory:

        make

This produces a tolerator tool called `bin/tolerator` and supporting
libraries in `lib/`.

Note, building with a tool like ninja can be done by adding `-G Ninja` to
the cmake invocation and running ninja instead of make.

Running
==============================================

First suppose that you have a program compiled to bitcode:

    clang -g -c -emit-llvm simpletest.c -o simple.bc

Running `tolerator` with the "log and quit" policy:

    bin/tolerator -log simple.bc -o simple

Running the resulting instrumented program:

    ./simple

