# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(ZEPHYR_EXTRA_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/modules/camera_service/")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(projeto)

target_sources(app PRIVATE 
        src/main.c
        src/sensor.c
        src/sensor_sim.c
        src/radar_control.c
        src/radar_processing.c
        src/radar_display.c
        src/vehicle_infraction_plate.c
        src/plate.c
    )
