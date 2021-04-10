
build : build/Makefile
	@echo Build 
	${MAKE} -C build

setup : build/Makefile

build/Makefile : src/CMakeLists.txt ${CMAKE_DIR}/riscv.cmake
	if [ ! -d build ] ; then mkdir build ; fi
	cd build;  \
		cmake \
		    -G "Unix Makefiles" \
			-DCMAKE_TOOLCHAIN_FILE=../${CMAKE_DIR}/riscv.cmake \
		    ../src

clean:
	rm -rf build

.PHONY: setup build
.SUFFIXES:
