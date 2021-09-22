# ------------------------------------------------------------------------
# 
# CMake is used to compile. The following makefile will initialize and launch CMake.

CMAKE_DIR=../cmake/cmake/
include ../cmake/targets.mak

# ------------------------------------------------------------------------
#
# Upload and debugger commands.
# These are not intended to be general purpose, only as an example for quick testing.
# Tested on windows using Msys2 shell.
# Please update according to your system.
# 
# OpenOCD from Xpack is used
# https://xpack.github.io/openocd/
#
# GDB from Xpack GCC toolchain is used
# https://xpack.github.io/riscv-none-embed-gcc/
#
# Jlink tools from SEGGER. 
# https://www.segger.com/JLinkOBSiFive

OUTPUT_FILE=build/main.hex

WINDOWS_SEGGER_PATH="C:\Program Files (x86)\SEGGER\JLink"
JLINK_DEVICE=FE310
JLINK_EXE=JLinkExe
JLINK_DOT_EXE=JLink.exe # NOTE - Not the Java linker!! Watch out for other jlink.exe..
# TODO - Platform independence...
JLINK_FULL_PATH=${WINDOWS_SEGGER_PATH}/${JLINK_DOT_EXE}

JLINK_UPLOADER_FLAGS= \
    -device ${JLINK_DEVICE} \
    -speed 4000 \
    -if JTAG \
    -jtagconf -1-1 \
    -autoconnect 1 \
    -NoGui 1 \

OPENOCD_PATH=/c/toolchain/xpack-openocd-0.10.0-15
#OPENOCD_PATH=/opt/openocd
OPENOCD_CFG=${OPENOCD_PATH}/scripts/board/sifive-hifive1-revb.cfg
OPENOCD=${OPENOCD_PATH}/bin/openocd

help:
	@echo "upload - use ${JLINK_DOT_EXE} to upload ${OUTPUT_FILE}"
	@echo "openocd - Start the OpenOCD server"
	@echo "debug - Start a gdb instance"

upload : build/upload.cmd
	${JLINK_FULL_PATH} \
		${JLINK_UPLOADER_FLAGS} \
		-CommanderScript $<

debug : 
	 start riscv-none-embed-gdb \
		-ex "target extended-remote 127.0.0.1:3333" \
		build/main.elf

openocd : ${OPENOCD_CFG}
	start ${OPENOCD} \
		-f ${OPENOCD_CFG} \
		-l openocd.log  2>&1 > /dev/null

openocd-cli : ${OPENOCD_CFG}
	telnet 127.0.0.1 4444

# Upload command
build/upload.cmd :
	rm -f $@
	echo h >> $@
	echo loadfile ${OUTPUT_FILE} >> $@
	echo r >> $@
	echo q >> $@


.PHONY: upload 
