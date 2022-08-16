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

# Part of "LibCMaker/cmake/cmr_get_download_params.cmake".

  if(version VERSION_EQUAL "5.0")  # "5.0.20220812"
    set(arch_file_sha
      "f443a10872682306be30d830cba33dca933e7782f6b467e144dee1a92a0e089b")
    set(src_commit
      "1a13d4d07f30658f4ecc3521308e129e65f3bab3")
  endif()

  # https://github.com/mikke89/RmlUi/archive/1a13d4d07f30658f4ecc3521308e129e65f3bab3.tar.gz
  set(base_url "https://github.com/mikke89/RmlUi/archive")
  set(src_dir_name    "rmlui-${version}")
  set(arch_file_name  "${src_dir_name}.tar.gz")
  set(unpack_to_dir   "${unpacked_dir}/${src_dir_name}")

  set(${out_ARCH_SRC_URL}   "${base_url}/${src_commit}.tar.gz" PARENT_SCOPE)
  set(${out_ARCH_DST_FILE}  "${download_dir}/${arch_file_name}" PARENT_SCOPE)
  set(${out_ARCH_FILE_SHA}  "${arch_file_sha}" PARENT_SCOPE)
  set(${out_SHA_ALG}        "SHA256" PARENT_SCOPE)
  set(${out_UNPACK_TO_DIR}  "${unpack_to_dir}" PARENT_SCOPE)
  set(${out_UNPACKED_SOURCES_DIR}
    "${unpack_to_dir}/RmlUi-${src_commit}" PARENT_SCOPE
  )
  set(${out_VERSION_BUILD_DIR} "${build_dir}/${src_dir_name}" PARENT_SCOPE)
