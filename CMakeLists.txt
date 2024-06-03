cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD "pico") #según la pico, se pone pico_w o pico 

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake) #siempre va, importa el SDK en el proyecto.

project(GPS) #nombre del proyecto

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(GPS main.c) #nombre del ejecutable y archivos fuente#nombre de ejecutable --> nombre de archivo fuente

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(GPS pico_stdlib hardware_irq hardware_uart hardware_gpio hardware_adc) #nombre del ejecutable y las librerías que se van a usar del SDK



pico_enable_stdio_uart(GPS 0)
pico_enable_stdio_usb(GPS 1)
# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(GPS) #genera los archivos binarios para cargar en la pico (archivo uf2)