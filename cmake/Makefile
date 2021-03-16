
build : build/Makefile
	${MAKE} -C build

setup : build/Makefile

build/Makefile : src/CMakeLists.txt cmake/riscv.cmake
	if [ ! -d build ] ; then mkdir build ; fi
	cd build;  \
		cmake \
		    -G "Unix Makefiles" \
			-DCMAKE_TOOLCHAIN_FILE=../cmake/riscv.cmake \
		    ../src

clean:
	rm -rf build

.PHONY: setup build
.SUFFIXES:
