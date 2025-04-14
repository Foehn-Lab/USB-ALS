set(TOOLCHAIN_PATH "D:\\MounRiver\\MounRiver_Studio2\\resources\\app\\resources\\win32\\components\\WCH\\Toolchain\\RISC-V Embedded GCC12\\bin")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
# Try to locate toolchain, you may want to set toolchain path manually
find_program(RISCV_GCC NAMES riscv-wch-elf-gcc PATHS ${TOOLCHAIN_PATH})
find_program(RISCV_GXX NAMES riscv-wch-elf-g++ PATHS ${TOOLCHAIN_PATH})
find_program(RISCV_AR riscv-wch-elf-ar ${TOOLCHAIN_PATH})
find_program(RISCV_AS riscv-wch-elf-as ${TOOLCHAIN_PATH})
find_program(RISCV_OBJCOPY riscv-wch-elf-objcopy ${TOOLCHAIN_PATH})
find_program(RISCV_OBJDUMP riscv-wch-elf-objdump ${TOOLCHAIN_PATH})
find_program(RISCV_SIZE riscv-wch-elf-size ${TOOLCHAIN_PATH})


if(NOT RISCV_GCC)
    message(FATAL_ERROR "RISC-V GCC compiler not found.  Please set CMAKE_C_COMPILER and CMAKE_CXX_COMPILER.")
endif()

if(NOT RISCV_OBJCOPY)
    message(FATAL_ERROR "RISC-V objcopy not found. Please check your toolchain installation.")
endif()

if(NOT RISCV_OBJDUMP)
    message(FATAL_ERROR "RISC-V objdump not found. Please check your toolchain installation.")
endif()

if(NOT RISCV_SIZE)
    message(FATAL_ERROR "RISC-V size not found. Please check your toolchain installation.")
endif()

set(CMAKE_C_COMPILER ${RISCV_GCC})
set(CMAKE_CXX_COMPILER ${RISCV_GXX}) # Use the same compiler for both C and C++ for embedded.  If you have a separate C++ compiler, adjust accordingly.
set(CMAKE_AR ${RISCV_AR})
set(CMAKE_ASM_COMPILER ${RISCV_AS})

# Flags (Mirroring Makefile settings)
add_compile_options(
    -march=rv32imacxw
    -mabi=ilp32
)

# set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=rv32imacxw -mabi=ilp32 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msmall-data-limit=8  -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g") #Note:  Assumes C++ should use the same flags, adjust if needed

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Ld/Link.ld)
# Linker flags
add_link_options(-Wl,--gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(
    -nostartfiles
    --specs=nano.specs
    --specs=nosys.specs
)
add_link_options(-u _printf_float)

