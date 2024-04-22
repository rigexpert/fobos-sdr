# Copyright 2024
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.10)

message(>>> "CMAKE_VERSION: " ${CMAKE_VERSION})

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
   project(fobos_sdr)
else()
   project(fobos_sdr C)
endif()

set(CMAKE_C_FLAGS, "${CMAKE_C_FLAGS} -std=c99")

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)

########################################################################
# Compiler specific setup
########################################################################
if(CMAKE_COMPILER_IS_GNUCC AND NOT WIN32)
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-Wextra)
    ADD_DEFINITIONS(-Wno-unused-parameter)
    ADD_DEFINITIONS(-Wno-unused)
    ADD_DEFINITIONS(-Wsign-compare)
    ADD_DEFINITIONS(-ansi)
    # ADD_DEFINITIONS(-Wdeclaration-after-statement)
    #http://gcc.gnu.org/wiki/Visibility
    add_definitions(-fvisibility=hidden)
elseif(MSVC14 OR MSVC14)
#pthread-w32 issue, timespec is now part of time.h
    ADD_DEFINITIONS(-D_TIMESPEC_DEFINED)
endif()

########################################################################
# Find build dependencies
########################################################################
#find_package(Threads)
find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBUSB libusb-1.0 IMPORTED_TARGET)
    if(LIBUSB_LINK_LIBRARIES)
        set(LIBUSB_LIBRARIES "${LIBUSB_LINK_LIBRARIES}")
    endif()
else()
#    set(LIBUSB_LIBRARIES "" CACHE STRING "manual libusb path")
#    set(LIBUSB_INCLUDE_DIRS "" CACHE STRING "manual libusb includepath")

    set(LIBUSB_LIBRARIES ${CMAKE_SOURCE_DIR}/libusb/MS64/dll)
    set(LIBUSB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libusb/include/libusb-1.0)
	
endif()

message(>>> "OS: " ${CMAKE_SYSTEM_NAME})
message(>>> "LIBUSB_LIBRARIES: " ${LIBUSB_LIBRARIES})
message(>>> "LIBUSB_INCLUDE_DIRS: " ${LIBUSB_INCLUDE_DIRS})

if(PKG_CONFIG_FOUND AND NOT LIBUSB_FOUND)
    message(FATAL_ERROR "LibUSB 1.0 required")
endif()

########################################################################
# Install udev rules
########################################################################
option(INSTALL_UDEV_RULES "Install udev rules for Fobos SDR" OFF)
if (INSTALL_UDEV_RULES)
    install (
        FILES fobos-sdr.rules
        DESTINATION "/etc/udev/rules.d"
        COMPONENT "udev"
        )
else (INSTALL_UDEV_RULES)
    message (STATUS "Udev rules not being installed, install them with -DINSTALL_UDEV_RULES=ON")
endif (INSTALL_UDEV_RULES)

option(DETACH_KERNEL_DRIVER "Detach kernel driver if loaded" OFF)
if (DETACH_KERNEL_DRIVER)
    message (STATUS "Building with kernel driver detaching enabled")
    add_definitions(-DDETACH_KERNEL_DRIVER=1)
else (DETACH_KERNEL_DRIVER)
    message (STATUS "Building with kernel driver detaching disabled, use -DDETACH_KERNEL_DRIVER=ON to enable")
endif (DETACH_KERNEL_DRIVER)

option(ENABLE_ZEROCOPY "Enable usbfs zero-copy support" OFF)
if (ENABLE_ZEROCOPY)
    message (STATUS "Building with usbfs zero-copy support enabled")
    add_definitions(-DENABLE_ZEROCOPY=1)
else (ENABLE_ZEROCOPY)
    message (STATUS "Building with usbfs zero-copy support disabled, use -DENABLE_ZEROCOPY=ON to enable")
endif (ENABLE_ZEROCOPY)

########################################################################

add_executable(${PROJECT_NAME} recorder/main.c fobos/fobos.c wav/wav_file.c)

target_compile_options(${PROJECT_NAME} PUBLIC -std=c99)

target_link_libraries(${PROJECT_NAME} ${LIBUSB_LIBRARIES})


if(${CMAKE_VERSION} VERSION_LESS "3.13.0")
    project(${PROJECT_NAME})
else()
    target_link_directories(${PROJECT_NAME} PRIVATE ${LIBUSB_LIBRARIES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}>
  ${LIBUSB_INCLUDE_DIRS}
  )
  
