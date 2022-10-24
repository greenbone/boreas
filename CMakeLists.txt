# Copyright (C) 2020 Greenbone Networks GmbH
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

cmake_minimum_required (VERSION 3.0)

message ("-- Configuring Boreas...")

project (boreas
  VERSION 22.4.1
  LANGUAGES C)

if (POLICY CMP0005)
  cmake_policy (SET CMP0005 NEW)
endif (POLICY CMP0005)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

OPTION (ENABLE_COVERAGE "Enable support for coverage analysis" OFF)

include (FindPkgConfig)

## Retrieve git revision (at configure time)
include (GetGit)

if (NOT CMAKE_BUILD_TYPE MATCHES "Release")
  if (EXISTS "${CMAKE_SOURCE_DIR}/.git/")
    if (GIT_FOUND)
      Git_GET_REVISION(${CMAKE_SOURCE_DIR} ProjectRevision)
      set (GIT_REVISION "~git-${ProjectRevision}")
    endif (GIT_FOUND)
  endif (EXISTS "${CMAKE_SOURCE_DIR}/.git/")
endif (NOT CMAKE_BUILD_TYPE MATCHES "Release")

## make format
message (STATUS "Looking for clang-format...")
find_program (CLANG_FORMAT clang-format)
if (CLANG_FORMAT)
  message (STATUS "Looking for clang-format... ${CLANG_FORMAT}")
  add_custom_target(format COMMAND ${CLANG_FORMAT} "-i" "./src/*.c" "./src/*.h"
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
else (CLANG_FORMAT)
  message (STATUS "clang-format not found...")
endif (CLANG_FORMAT)

# Set dev version if this is a development version and not a full release,
# unset (put value 0 or delete line) before a full release and reset after.
set (PROJECT_DEV_VERSION 1)

# If PROJECT_DEV_VERSION is set, the version string will be set to:
#   "major.minor.patch~dev${PROJECT_DEV_VERSION}${GIT_REVISION}"
# If PROJECT_DEV_VERSION is NOT set, the version string will be set to:
#   "major.minor.patch${GIT_REVISION}"
# For CMAKE_BUILD_TYPE "Release" the git revision will be empty.
if (PROJECT_DEV_VERSION)
  set (PROJECT_VERSION_SUFFIX "~dev${PROJECT_DEV_VERSION}")
endif (PROJECT_DEV_VERSION)

set (PROJECT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${PROJECT_VERSION_SUFFIX}${GIT_REVISION}")

## CPack configuration

set (CPACK_CMAKE_GENERATOR "Unix Makefiles")
set (CPACK_GENERATOR "TGZ")
set (CPACK_INSTALL_CMAKE_PROJECTS ".;boreeas;ALL;/")
set (CPACK_MODULE_PATH "")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_TOPLEVEL_TAG "")
set (CPACK_SYSTEM_NAME "")
set (CPACK_TOPLEVEL_TAG "")

set (CPACK_PACKAGE_VERSION "${PROJECT_VERSION_STRING}${PROJECT_VERSION_GIT}")

set (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_PACKAGE_VENDOR "The Boreas Project")
set (CPACK_SOURCE_IGNORE_FILES
"${CMAKE_BINARY_DIR}"
"/.git/"
)

include (CPack)

## Variables

if (NOT SYSCONFDIR)
  set (SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")
endif (NOT SYSCONFDIR)

set (BOREAS_SYSCONF_DIR "${SYSCONFDIR}/boreas")

if (NOT LOCALSTATEDIR)
  set (LOCALSTATEDIR "${CMAKE_INSTALL_PREFIX}/var")
endif (NOT LOCALSTATEDIR)

if (NOT BOREAS_LOG_DIR)
	set (BOREAS_LOG_DIR "${LOCALSTATEDIR}/log/gvm")
endif (NOT BOREAS_LOG_DIR)

if (NOT EXEC_PREFIX)
  set (EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif (NOT EXEC_PREFIX)

if (NOT BINDIR)
  set (BINDIR "${EXEC_PREFIX}/bin")
endif (NOT BINDIR)

message ("-- Install prefix: ${CMAKE_INSTALL_PREFIX}")

## Version

set (BOREAS_VERSION "${PROJECT_VERSION_STRING}")

if (BOREAS_VERSION)
  add_definitions (-DBOREAS_VERSION="${BOREAS_VERSION}")
endif (BOREAS_VERSION)

# Configure Doxyfile with version number
configure_file (doc/Doxyfile.in doc/Doxyfile @ONLY)
configure_file (doc/Doxyfile_full.in doc/Doxyfile_full @ONLY)
configure_file (doc/Doxyfile_xml.in doc/Doxyfile_xml @ONLY)
configure_file (doc/boreas.8.in doc/boreas.8 @ONLY)
configure_file (VERSION.in VERSION @ONLY)
configure_file (src/boreas_log_conf.cmake_in src/boreas_log.conf)

## Testing

enable_testing ()

## Program

if (ENABLE_COVERAGE)
  set (COVERAGE_FLAGS "--coverage")
endif (ENABLE_COVERAGE)

set (HARDENING_FLAGS            "-Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -fstack-protector")
set (LINKER_HARDENING_FLAGS     "-Wl,-z,relro -Wl,-z,now")
set (GPGME_C_FLAGS              "-D_FILE_OFFSET_BITS=64 -DLARGEFILE_SOURCE=1")

set (CMAKE_C_FLAGS_RELEASE      "${CMAKE_C_FLAGS_RELEASE} ${HARDENING_FLAGS}")
set (CMAKE_C_FLAGS_DEBUG        "${CMAKE_C_FLAGS_DEBUG} ${COVERAGE_FLAGS}")
set (CMAKE_C_FLAGS              "${CMAKE_C_FLAGS} -Wall -D_BSD_SOURCE -D_ISOC99_SOURCE -D_SVID_SOURCE -D_DEFAULT_SOURCE")

if (NOT SKIP_SRC)
  add_subdirectory (src)
endif (NOT SKIP_SRC)

## Documentation

add_subdirectory (doc)

## End
