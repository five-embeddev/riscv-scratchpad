SPIKE=../../riscv-isa-sim/spike
CMD_FILE=./run_sim.cmd
MARCH=rv32imac_zicsr
VCD_FILE=vcd-trace.vcd
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
    --log run_sim.log \
    -d \
    --debug-cmd=${CMD_FILE} \
    ${ELF_FILE}

