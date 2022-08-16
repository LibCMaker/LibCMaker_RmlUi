# ****************************************************************************
#  Project:  LibCMaker
#  Purpose:  A CMake build scripts for build libraries with CMake
#  Author:   NikitaFeodonit, nfeodonit@yandex.com
# ****************************************************************************
#    Copyright (c) 2017-2025 NikitaFeodonit
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

# Part of "LibCMaker/cmake/cmr_build_rules.cmake".

  # Copy CMake build scripts.
  if(COPY_RMLUI_CMAKE_BUILD_SCRIPTS)
    cmr_print_status("Copy CMake build scripts to unpacked sources.")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${lib_BASE_DIR}/patch/rmlui-${lib_VERSION}
        ${lib_SRC_DIR}/
    )
  endif()

  # Configure library.
  add_subdirectory(${lib_SRC_DIR} ${lib_VERSION_BUILD_DIR})
