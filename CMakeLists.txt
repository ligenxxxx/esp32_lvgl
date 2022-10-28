# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/uart")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/shell")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/led")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/i2c")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/spi")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/lcd")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/joystick")
set(EXTRA_COMPONENT_DIRS "$(EXTRA_COMPONENT_DIRS) components/lvgl")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_lvgl)
