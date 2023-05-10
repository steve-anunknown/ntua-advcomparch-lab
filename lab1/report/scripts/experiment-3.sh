#!/bin/bash
# bash script to execute the third experiment
PIN_ROOT="~/adv-comp-arch/lab1/pin-3.27-98718-gbeaa5d51e-gcc-linux/pin"
SIM_ROOT="~/adv-comp-arch/lab1/advcomparch-ex1-helpcode/pintool/obj-intel64/simulator.so"
EXE_ROOT="~/adv-comp-arch/lab1/parsec-3.0/parsec_workspace/executables"
INP_ROOT="~/adv-comp-arch/lab1/parsec-3.0/parsec_workspace/inputs"
OUT_ROOT="~/adv-comp-arch/lab1/parsec-3.0/parsec_workspace/outputs/exp3"

L1_FLAGS="-L1c 32 -L1a 8 -L1b 64"
L2_FLAGS="-L2c 1024 -L2a 8 -L2b 128"
TLB_FLAGS=(
"-TLBe 64 -TLBa 1 -TLBp 4096"
"-TLBe 64 -TLBa 2 -TLBp 4096"
"-TLBe 64 -TLBa 4 -TLBp 4096"
"-TLBe 64 -TLBa 8 -TLBp 4096"
"-TLBe 64 -TLBa 16 -TLBp 4096"
"-TLBe 64 -TLBa 32 -TLBp 4096"
"-TLBe 64 -TLBa 64 -TLBp 4096"
"-TLBe 128 -TLBa 4 -TLBp 4096"
"-TLBe 256 -TLBa 4 -TLBp 4096"
)

COMMANDS=(
"${EXE_ROOT}/blackscholes 1 ${INP_ROOT}/in_64K.txt prices.txt"
"${EXE_ROOT}/bodytrack ${INP_ROOT}/sequenceB_4 4 4 4000 5 0 1"
"${EXE_ROOT}/canneal 1 15000 2000 ${INP_ROOT}/400000.nets 128"
"${EXE_ROOT}/fluidanimate 1 5 ${INP_ROOT}/in_300K.fluid out.fluid"
"${EXE_ROOT}/freqmine ${INP_ROOT}/kosarak_990k.dat 790"
"${EXE_ROOT}/rtview ${INP_ROOT}/happy_buddha.obj -automove -nthreads 1 -frames 3 -res 1920 1080"
"${EXE_ROOT}/streamcluster 10 20 128 16384 16384 1000 none output.txt 1"
"${EXE_ROOT}/swaptions -ns 64 -sm 40000 -nt 1"
)
OUTPUT_FILE=(
    "${OUT_ROOT}/blacksholes/exp3-blackscholes"
    "${OUT_ROOT}/bodytrack/exp3-bodytrack"
    "${OUT_ROOT}/canneal/exp3-canneal"
    "${OUT_ROOT}/fluidanimate/exp3-fluidanimate"
    "${OUT_ROOT}/freqmine/exp3-freqmine"
    "${OUT_ROOT}/rtview/exp3-rtview"
    "${OUT_ROOT}/streamcluster/exp3-streamcluster"
    "${OUT_ROOT}/swaptions/exp3-swaptions"
)

export LD_LIBRARY_PATH=~/adv-comp-arch/lab1/parsec-3.0/pkgs/libs/hooks/inst/amd64-linux.gcc-serial/lib

for i in "${!TLB_FLAGS[@]}";
do
    for j in "${!COMMANDS[@]}";
    do
        ${PIN_ROOT} -t ${SIM_ROOT} -o "${OUTPUT_FILE[$j]}${i}.txt" ${L1_FLAGS} ${L2_FLAGS} ${TLB_FLAGS[$i]} -- ${COMMANDS[$j]}
    done
done

