#!/bin/bash

PIN_ROOT="/home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/pin-3.27-98718-gbeaa5d51e-gcc-linux/pin"
SIM_ROOT="/home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/advcomparch-ex1-helpcode/pintool/obj-intel64/simulator.so"
EXE_ROOT="/home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/parsec-3.0/parsec_workspace/executables"
INP_ROOT="/home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/parsec-3.0/parsec_workspace/inputs"
OUT_ROOT="/home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/parsec-3.0/parsec_workspace/outputs/exp2"

L1_FLAGS="-L1c 32 -L1a 8 -L1b 64"
L2_FLAGS=(
"-L2c 512 -L2a 8 -L2b 64"
"-L2c 512 -L2a 8 -L2b 128"
"-L2c 512 -L2a 8 -L2b 256"
"-L2c 1024 -L2a 16 -L2b 64"
"-L2c 1024 -L2a 16 -L2b 128"
"-L2c 1024 -L2a 16 -L2b 256"
"-L2c 1024 -L2a 16 -L2b 64"
"-L2c 1024 -L2a 16 -L2b 128"
"-L2c 1024 -L2a 16 -L2b 256"
"-L2c 2048 -L2a 16 -L2b 64"
"-L2c 2048 -L2a 16 -L2b 128"
"-L2c 2048 -L2a 16 -L2b 256"
)
TLB_FLAGS="-TLBe 64 -TLBa 4 -TLBp 4096"

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

#
OUTPUT_FILE=(
    "${OUT_ROOT}/blacksholes/exp2-blackscholes"
    "${OUT_ROOT}/bodytrack/exp2-bodytrack"
    "${OUT_ROOT}/canneal/exp2-canneal"
    "${OUT_ROOT}/fluidanimate/exp2-fluidanimate"
    "${OUT_ROOT}/freqmine/exp2-freqmine"
    "${OUT_ROOT}/rtview/exp2-rtview"
    "${OUT_ROOT}/streamcluster/exp2-streamcluster"
    "${OUT_ROOT}/swaptions/exp2-swaptions"
)

export LD_LIBRARY_PATH=/home/stefanos/Documents/programming-projects/uni/adv-comp-arch/lab1/parsec-3.0/pkgs/libs/hooks/inst/amd64-linux.gcc-serial/lib

for i in "${!L2_FLAGS[@]}";
do
    for j in "${!COMMANDS[@]}";
    do
        ${PIN_ROOT} -t ${SIM_ROOT} -o "${OUTPUT_FILE[$j]}${i}.txt" ${L1_FLAGS} ${L2_FLAGS[$i]} ${TLB_FLAGS} -- ${COMMANDS[$j]}
        > /var/log/syslog
    done
done



