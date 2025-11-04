.PHONY: all build cmake clean format

BUILD_DIR := build
BUILD_TYPE ?= Debug 

all: build

${BUILD_DIR}/build.ninja:
	cmake \
		-G Ninja \
		-B${BUILD_DIR} \
		-DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
		-DCMAKE_TOOLCHAIN_FILE='../../resources/cmake/gcc-arm-none-eabi.cmake' \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=true

cmake: ${BUILD_DIR}/build.ninja

build: cmake
	ninja -C ${BUILD_DIR}

clean:
	rm -rf $(BUILD_DIR)
