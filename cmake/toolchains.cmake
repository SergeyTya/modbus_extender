
if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

    set_target_properties(${TARGET_NAME} PROPERTIES
            LINKER_FILE    "${CMAKE_CURRENT_SOURCE_DIR}/startup/GNU/${MCU_NAME}.ld"
            STARTUP_FILE   "${CMAKE_CURRENT_SOURCE_DIR}/startup/GNU/startup_${MCU_NAME}.s"
            DEVICE        ${MCU_NAME}
            CPU           ${MCU_TYPE}
            )

    set(CPU_FLAGS
            -mcpu=${MCU_TYPE}
            -mfloat-abi=${MCU_FLOAT_TYPE}
            -mfpu=${MCU_FPU_NAME}
            -mthumb
            )

    target_compile_options(${TARGET_NAME} PUBLIC $<$<COMPILE_LANGUAGE:ASM>:${CFLAGS} -x assembler-with-cpp>)

    target_compile_options(${TARGET_NAME}
            PUBLIC ${CPU_FLAGS}
            PUBLIC -fdata-sections
            PUBLIC -ffunction-sections
            PUBLIC -fsigned-char
            PUBLIC -ffreestanding
            PUBLIC -nostartfiles
            PUBLIC -nostdlib
            PUBLIC -O0
            PUBLIC -std=gnu17
            PUBLIC -Wextra
            PUBLIC -Wall
            )

    target_link_options(${TARGET_NAME}
            PUBLIC ${CPU_FLAGS}
            -T $<TARGET_PROPERTY:LINKER_FILE>
            -lgcc -lc
            -specs=nano.specs 
            -specs=nosys.specs 
            -Wl,-Map=${TARGET_NAME}.map,--cref,--print-memory-usage
            -Wl,-gc-sections
            )

    # https://www.opennet.ru/docs/RUS/gcc/gcc1-2.html
    # https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html
    # https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
    # https://gcc.gnu.org/onlinedocs/gnat_ugn/Compilation-options.html
    # https://gcc.gnu.org/onlinedocs/gcc/Spec-Files.html

    #firmware_size(${TARGET_NAME})

#    #-- Custom commands ------------------------------------------------------------
    ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} "-Oihex" ${TARGET_NAME}.elf ${CMAKE_BINARY_DIR}/${TARGET_NAME}.hex
            COMMAND ${CMAKE_OBJCOPY} "-Obinary" ${TARGET_NAME}.elf ${CMAKE_BINARY_DIR}/${TARGET_NAME}.bin
            COMMAND ${CMAKE_OBJDUMP} "-DS" ${TARGET_NAME}.elf > ${CMAKE_BINARY_DIR}/${TARGET_NAME}.dasm
            #COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
            )

endif()

