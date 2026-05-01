# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

# ================== Standards ==================
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===== Pico VS Code Extension (NE PAS MODIFIER) =====
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ==================================================

# ===== Cible matérielle =====
set(PICO_BOARD pico CACHE STRING "Board type")

# ===== Pico SDK =====
include(pico_sdk_import.cmake)

# ===== Projet =====
project(obstacle_detection C CXX ASM)

# ===== Initialisation SDK =====
pico_sdk_init()

# ===== Exécutable =====
add_executable(obstacle_detection
    obstacle_detection.c my_lib/motor.c my_lib/hcsr04.c
)

# ===== Infos programme =====
pico_set_program_name(obstacle_detection "obstacle_detection")
pico_set_program_version(obstacle_detection "0.1")

# ===== Entrées / sorties =====
pico_enable_stdio_uart(obstacle_detection 0)
pico_enable_stdio_usb(obstacle_detection 1)


pico_generate_pio_header(obstacle_detection ${CMAKE_CURRENT_LIST_DIR}/my_lib/hcsr04.pio)

# ===== Librairies =====
target_link_libraries(obstacle_detection
    pico_stdlib
    hardware_gpio
    hardware_timer
    hardware_adc 
    hardware_pwm
    hardware_pio 
)

# ===== Includes =====
target_include_directories(obstacle_detection PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# ===== Génération .uf2 / .bin / .elf =====
pico_add_extra_outputs(obstacle_detection)
