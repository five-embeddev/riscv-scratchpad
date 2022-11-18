#!/bin/bash

SPIKE=../../riscv-isa-sim/spike
CMD_FILE=./test/run_sim.cmd
MARCH=rv32imac_zicsr
VCD_FILE=test/vcd-trace.vcd
LOG_FILE=test/run_sim.log
MMAP=0x8000000:0x2000,0x80000000:0x4000,0x20010000:0x6a120
# The core runs in 5000 cycle increments, then updates the system (see sim.h:INTERLEAVE)
# This will run for 20 such super cycles
CYCLES=100000
ELF_FILE=build/main.elf

${SPIKE} \
    --vcd-log=${VCD_FILE} \
    --priv=m \
    --isa=${MARCH} \
    -l \
    -m${MMAP} \
    --max-cycles ${CYCLES} \
    --log ${LOG_FILE} \
    -d \
    --debug-cmd=${CMD_FILE} \
    ${ELF_FILE} 2> test/trace.log

