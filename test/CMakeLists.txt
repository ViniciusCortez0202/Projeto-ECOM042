# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(ZEPHYR_EXTRA_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../modules/camera_service/")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(projeto)

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../src
)

target_sources(app PRIVATE         
        src/main.c
        src/test_plate.c
        src/test_radar_processing.c

        ${CMAKE_CURRENT_LIST_DIR}/../src/plate.c
        ${CMAKE_CURRENT_LIST_DIR}/../src/radar_processing.c
        ${CMAKE_CURRENT_LIST_DIR}/../src/radar_display.c
        ${CMAKE_CURRENT_LIST_DIR}/../src/radar_control.c
        ${CMAKE_CURRENT_LIST_DIR}/../src/sensor.c
        ${CMAKE_CURRENT_LIST_DIR}/../src/vehicle_infraction_plate.c
    )
