# GD32F3x0_Firmware_Library_V2.5.0.cmake

message("--- GD32F3x0 lib"  ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F3x0_Firmware_Library_V2.5.0/CMSIS
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F3x0_Firmware_Library_V2.5.0/CMSIS/GD/GD32F3x0/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F3x0_Firmware_Library_V2.5.0/Include
)

FILE(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F3x0_Firmware_Library_V2.5.0/CMSIS/GD/GD32F3x0/Source/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F3x0_Firmware_Library_V2.5.0/Source/*.c
        )

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )