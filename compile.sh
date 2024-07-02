#!bin/bash

g++ -std=c++11 \
-O1 \
-fcrossjumping \
-fcse-skip-blocks \
-fdelete-null-pointer-checks \
-fgcse \
-lpthread \
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

