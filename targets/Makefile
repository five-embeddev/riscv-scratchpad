SRC=example.c 
SRC_BASE=${SRC:%.c=%}
CROSS_COMPILE=riscv-none-embed-
GCC=${CROSS_COMPILE}gcc
OBJDUMP=${CROSS_COMPILE}objdump

# Alias for full name
# G=imafdzicsr_zifencei
ALIAS_TARGETS=\
	rv32g.      \

# 
SPEC_TARGETS=\
     rv32e.ilp32e      \
     rv32eac.ilp32e	   \
     rv32em.ilp32e	   \
     rv32emac.ilp32e   \
     rv32i.ilp32	   \
     rv32iac.ilp32	   \
     rv32im.ilp32	   \
     rv32imac.ilp32	   \
     rv32imaf.ilp32f   \
     rv32imafc.ilp32f  \
     rv32imafdc.ilp32d \
     rv64imac.lp64	   \
     rv64imafc.lp64f   \
     rv64imafdc.lp64d   

OTHER_TARGETS=\
     rv32imafd.ilp32d \
     rv32ec.ilp32e      \
     rv32ic.ilp32      \

# ABI does not match
BAD_TARGETS+=rv32ic.ilp32e  

# Capital letter
BAD_TARGETS+=bad_rv32G. \
			 bad_rv32iMAFD.ilp32d \


# Reverse of rv32imafdc - not in cannonical order
BAD_TARGETS+=bad_rv32ci.ilp32d 

# Reverse of rv32imafdc - not in cannonical order
BAD_TARGETS+=bad_rv32cdfami.ilp32d 

# Reverse of rv32imafdc - not in cannonical order - base ISA correct
BAD_TARGETS+=bad_rv32icdfam.ilp32d 

GOOD_TARGETS=${ALIAS_TARGETS} ${SPEC_TARGETS}  ${OTHER_TARGETS} 

ALL=\
	$(foreach TARGET, ${GOOD_TARGETS}, ${SRC_BASE}.${TARGET}.disasm) \
	$(foreach TARGET, ${BAD_TARGETS}, ${SRC_BASE}.${TARGET}.disasm)

DEFINES=${GOOD_TARGETS:%=%.defines} ${BAD_TARGETS:%=%.defines}
MULTILIB=${GOOD_TARGETS:%=%.multilib} 

all : ${ALL}

defines : ${DEFINES}

multilib : ${MULTILIB}

${SRC_BASE}.%.disasm : ${SRC_BASE}.c
	@echo "[TARGET $*]"
	${GCC} -nostartfiles  -march=$(subst bad_,,$(basename ${*}))  $(foreach MABI, $(subst .,,$(suffix ${*})), -mabi=${MABI}) -Os $< -lm -o $(basename $@).elf
	${OBJDUMP} -S $(basename $@).elf > $@

%.defines : 
	${GCC}  -march=$(subst bad_,,$(basename ${*})) $(foreach MABI, $(subst .,,$(suffix ${*})), -mabi=${MABI}) -dM -E - < /dev/null > $@
	-diff rv32g..defines $@ > ${@}.diff

%.multilib : 
	echo "$* : `${GCC}  -march=$(subst bad_,,$(basename ${*})) $(foreach MABI, $(subst .,,$(suffix ${*})), -mabi=${MABI}) --print-multi-directory`" > $@

clean :
	rm -f ${ALL} ${DEFINES} ${DEFINES:%=%.diff} ${MULTILIB}
