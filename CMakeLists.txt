cmake_minimum_required(VERSION 3.15) # Or a more appropriate minimum version

project(USB_ALS C CXX) # Adjust language support as needed

# Toolchain Configuration (RISC-V Embedded)
#  - You may need to adjust these paths based on your actual installation.

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(wch.cmake) # Include the toolchain configuration
enable_language(C ASM)

include_directories(Core Debug Peripheral/inc User)

# Source Files
file(GLOB_RECURSE SOURCES
    "Core/*.c"
    "User/*.c"
    "User/*.cpp"
    "Debug/*.c"
    "Peripheral/src/*.c"
    "Startup/*.S"
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    # add_compile_options(-O0 -g3)
endif ()

# Add executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})
target_link_options(${PROJECT_NAME}.elf PUBLIC -T${LINKER_SCRIPT})

# Target Link Libraries (if any - add them here.  This example has none that can be directly translated from the makefile)
# target_link_libraries(CH32X033F8P6 ...)

# Post-Build Commands (Generate HEX, LST, SIZ)
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

# add_custom_command(TARGET CH32X033F8P6 POST_BUILD
#     COMMAND ${RISCV_OBJCOPY} -O ihex $<TARGET_FILE:CH32X033F8P6> CH32X033F8P6.hex
#     COMMAND ${RISCV_OBJDUMP} --all-headers --demangle --disassemble -M xw $<TARGET_FILE:CH32X033F8P6> > CH32X033F8P6.lst
#     COMMAND ${RISCV_SIZE} --format=berkeley $<TARGET_FILE:CH32X033F8P6>
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
# )

# # Artifacts to install (optional)
# install(TARGETS CH32X033F8P6 DESTINATION bin)
# install(FILES CH32X033F8P6.hex CH32X033F8P6.lst CH32X033F8P6.siz DESTINATION bin) # Output files in binary dir

