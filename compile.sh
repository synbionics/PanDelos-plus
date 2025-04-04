#!bin/bash
# Initialize single thread flag
SINGLE_THREAD_ENABLED=false

# Parse command line arguments
for arg in "$@"
do
    if [ "$arg" == "--st" ]; then
        SINGLE_THREAD_ENABLED=true
    fi
done

echo "int main() { return 0; }" > test.cpp
g++ -pthread test.cpp -o test_pthread 2>/dev/null
if [ $? -eq 0 ]; then
    PTHREAD_FLAG="-pthread"
else
    PTHREAD_FLAG="-lpthread"
fi
rm -f test.cpp test_pthread


COMPILE_FLAGS="-std=c++11 \
-O1 \
-fcrossjumping \
-fcse-skip-blocks \
-fdelete-null-pointer-checks \
-fgcse \
${PTHREAD_FLAG} \
-fipa-bit-cp  -fipa-cp  -fipa-icf \
-fipa-ra  -fipa-sra  -fipa-vrp \
-fisolate-erroneous-paths-dereference \
-flra-remat \
-foptimize-sibling-calls \
-fthread-jumps \
-ftree-builtin-call-dce \
-fvect-cost-model=dynamic \
-floop-interchange \
-floop-unroll-and-jam \
-fpredictive-commoning \
-ftree-partial-pre \
-faggressive-loop-optimizations \
-fsplit-wide-types-early \
-fmodulo-sched \
-fmodulo-sched-allow-regmoves \
-fno-lifetime-dse \
-w"

if [ "$SINGLE_THREAD_ENABLED" = true ]; then
    COMPILE_FLAGS="$COMPILE_FLAGS -DSINGLE_THREAD"
    echo "Compiling in single thread mode"
else
    echo "Compiling in multi thread mode"
fi

# echo "Compile flags: $COMPILE_FLAGS"
g++ $COMPILE_FLAGS src/main.cc -o main


