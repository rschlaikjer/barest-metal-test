cmake_minimum_required(VERSION 3.1.3)

# Set the ARM toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchain-arm.cmake")
message(STATUS "Cmake toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# Need to define a project right after the toolchain to force evaluation
project(baremetal C CXX)

# Common paths
get_filename_component(LINKERSCRIPT_DIR "./" REALPATH)
get_filename_component(SCRIPTS_DIR "scripts" REALPATH)

# Macros for build targets
include(cmake/macros.cmake)

# Global CFLAGS defaultsd
set(OPT "-Os")
set(DEBUG "-ggdb3")
set(CSTD "-std=c99")
set(CXXSTD "-std=c++14")

set(CFLAGS "-Wall -Wextra -Wshadow -Wimplicit-function-declaration \
            -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
            -fno-common -ffunction-sections -fdata-sections \
            -fno-builtin -fno-exceptions -fno-unwind-tables")

set(LDSCRIPT "ldscript.ld")
set(DEFS "${DEFS} ${USER_DEFS} -DSTM32F0 -DPRINTF_DISABLE_SUPPORT_FLOAT")
set(FP_FLAGS "${FP_FLAGS} -msoft-float")
set(ARCH_FLAGS "${ARCH_FLAGS} -mthumb -mcpu=cortex-m0 ${FP_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPT} ${DEBUG} ${CSTD} ${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPT} ${DEBUG} ${CXXSTD} ${CXXFLAGS}")

set(CXXFLAGS
    "-Wall -Wextra -Wshadow -Wredundant-decls -Weffc++ \
     -fno-common -ffunction-sections -fdata-sections -fno-threadsafe-statics \
     -fno-builtin -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit")

set(LINKER_FLAGS
    "--static -nostartfiles \
    -T${LINKERSCRIPT_DIR}/${LDSCRIPT} ${ARCH_FLAGS} ${DEBUG} \
    -Wl,-Map=${PROJECT_NAME}.map -Wl,--cref \
    -Wl,--gc-sections -Wl,--no-wchar-size-warning \
    -Wl,--start-group -lc -lgcc -lstdc++ -lnosys -Wl,--end-group -lm")

add_executable(baremetal_elf
    src/bootstrap.cpp
    src/main.cpp
)

target_compile_definitions(baremetal_elf PRIVATE ${FLAGS})
set_target_properties(baremetal_elf PROPERTIES COMPILE_FLAGS
    "${DEFS} ${ARCH_FLAGS}")
set_target_properties(baremetal_elf PROPERTIES LINK_FLAGS
    "${LINKER_FLAGS}")
get_target_property(PROP baremetal_elf COMPILE_FLAGS)

# Create additional targets for size / flashing
define_flash_target(baremetal)
define_size_target(baremetal)
include_gdbinit()
