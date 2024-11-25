#!bin/bash

echo "int main() { return 0; }" > test.cpp
g++ -pthread test.cpp -o test_pthread 2>/dev/null
if [ $? -eq 0 ]; then
    PTHREAD_FLAG="-pthread"
else
    PTHREAD_FLAG="-lpthread"
fi
rm -f test.cpp test_pthread


g++ -std=c++11 \
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
main.cc -o main
# -Wextra -Wall \

