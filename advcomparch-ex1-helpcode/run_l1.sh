#!/bin/bash

## Modify the following paths appropriately
PARSEC_PATH=/path/to/parsec
PIN_EXE=/path/to/pin
PIN_TOOL=/path/to/pintool/obj-intel64/simulator.so

CMDS_FILE=./cmds_simlarge.txt
outDir="./outputs/"

export LD_LIBRARY_PATH=$PARSEC_PATH/pkgs/libs/hooks/inst/amd64-linux.gcc-serial/lib/

## Triples of <cache_size>_<associativity>_<block_size>
CONFS="16_4_64 32_4_64"

L2size=1024
L2assoc=8
L2bsize=128
TLBe=64
TLBp=4096
TLBa=4
L2prf=0

for BENCH in $@; do
	cmd=$(cat ${CMDS_FILE} | grep "$BENCH")
for conf in $CONFS; do
	## Get parameters
    L1size=$(echo $conf | cut -d'_' -f1)
    L1assoc=$(echo $conf | cut -d'_' -f2)
    L1bsize=$(echo $conf | cut -d'_' -f3)

	outFile=$(printf "%s.dcache_cslab.L1_%04d_%02d_%03d.out" $BENCH ${L1size} ${L1assoc} ${L1bsize})
	outFile="$outDir/$outFile"

	pin_cmd="$PIN_EXE -t $PIN_TOOL -o $outFile -L1c ${L1size} -L1a ${L1assoc} -L1b ${L1bsize} -L2c ${L2size} -L2a ${L2assoc} -L2b ${L2bsize} -TLBe ${TLBe} -TLBp ${TLBp} -TLBa ${TLBa} -L2prf ${L2prf} -- $cmd"
	time $pin_cmd
done
done
