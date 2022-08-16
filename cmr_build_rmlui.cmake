# ****************************************************************************
#  Project:  LibCMaker
#  Purpose:  A CMake build scripts for build libraries with CMake
#  Author:   NikitaFeodonit, nfeodonit@yandex.com
# ****************************************************************************
#    Copyright (c) 2017-2022 NikitaFeodonit
#
#    This file is part of the LibCMaker project.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published
#    by the Free Software Foundation, either version 3 of the License,
#    or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#    See the GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program. If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************

#-----------------------------------------------------------------------
# The file is an example of the convenient script for the library build.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Lib's name, version, paths
#-----------------------------------------------------------------------

set(RMLUI_lib_NAME "RmlUi")
set(RMLUI_lib_VERSION "5.0" CACHE STRING "RMLUI_lib_VERSION")  # "5.0.20220812"
set(RMLUI_lib_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "RMLUI_lib_DIR")


# To use our Find<LibName>.cmake.
list(APPEND CMAKE_MODULE_PATH "${RMLUI_lib_DIR}/cmake/modules")


#-----------------------------------------------------------------------
# LibCMaker_<LibName> specific vars and options
#-----------------------------------------------------------------------

option(COPY_RMLUI_CMAKE_BUILD_SCRIPTS "COPY_RMLUI_CMAKE_BUILD_SCRIPTS" ON)


#-----------------------------------------------------------------------
# Library specific vars and options
#-----------------------------------------------------------------------

if(APPLE)
  option(BUILD_UNIVERSAL_BINARIES
    "Build universal binaries for all architectures supported"
    OFF
  )
endif()

option(NO_FONT_INTERFACE_DEFAULT
  "Do not include the default font engine in the build. Allows building without the FreeType dependency, but a custom font engine must be created and set."
  ON
)

if(NOT BUILD_SHARED_LIBS)
  message("-- Building static libraries. Make sure to #define RMLUI_STATIC_LIB before including RmlUi in your project.")
endif()

option(NO_THIRDPARTY_CONTAINERS "Only use standard library containers." OFF)
if(NO_THIRDPARTY_CONTAINERS)
  message("-- No third-party containers will be used: Make sure to #define RMLUI_NO_THIRDPARTY_CONTAINERS before including RmlUi in your project.")
endif()

option(CUSTOM_CONFIGURATION
  "Customize RmlUi configuration files for overriding the default configuration and types."
  OFF
)
option(ENABLE_TRACY_PROFILING
  "Enable profiling with Tracy. Source files can be placed in Dependencies/tracy."
  OFF
)
option(ENABLE_LOTTIE_PLUGIN
  "Enable plugin for Lottie animations. Requires the rlottie library."
  OFF
)
option(ENABLE_SVG_PLUGIN
  "Enable plugin for SVG images. Requires the lunasvg library."
  OFF
)
option(DISABLE_RTTI_AND_EXCEPTIONS
  "Build with rtti and exceptions disabled."
  OFF
)
option(ENABLE_PRECOMPILED_HEADERS "Enable precompiled headers" ON)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." OFF)


#-----------------------------------------------------------------------
# Build, install and find the library
#-----------------------------------------------------------------------

cmr_find_package(
  LibCMaker_DIR   ${LibCMaker_DIR}
  NAME            ${RMLUI_lib_NAME}
  VERSION         ${RMLUI_lib_VERSION}
  LIB_DIR         ${RMLUI_lib_DIR}
  REQUIRED
  CONFIG
)
