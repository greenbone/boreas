# Copyright (C) 2020 Greenbone Networks GmbH
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
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

## Program

## Dependency checks

include (FindPkgConfig)

if (NOT PKG_CONFIG_FOUND)
  message(FATAL_ERROR "pkg-config executable not found. Aborting.")
endif (NOT PKG_CONFIG_FOUND)

pkg_check_modules (LIBGVM_BASE REQUIRED libgvm_base>=22.4)
pkg_check_modules (LIBGVM_BOREAS REQUIRED libgvm_boreas>=22.4)
pkg_check_modules (LIBGVM_UTIL REQUIRED libgvm_util>=22.4)
pkg_check_modules (GLIB REQUIRED glib-2.0>=2.42)

message (STATUS "Looking for pcap...")
find_library (PCAP pcap)
message (STATUS "Looking for pcap... ${PCAP}")
if (NOT PCAP)
  message (SEND_ERROR "The pcap library is required.")
endif (NOT PCAP)

message (STATUS "Looking for pcap-config...")
find_program (PCAP_CONFIG pcap-config)

if (PCAP_CONFIG)
  message (STATUS "Looking for pcap-config... ${PCAP_CONFIG}")
  execute_process (COMMAND pcap-config --libs
    OUTPUT_VARIABLE PCAP_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND pcap-config --cflags
    OUTPUT_VARIABLE PCAP_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
else (PCAP_CONFIG)
  message (STATUS "pcap-config not found, using defaults...")
  set (PCAP_LDFLAGS "-L/usr/lib -lpcap")
  set (PCAP_CFLAGS "-I/usr/include")
endif (PCAP_CONFIG)

add_executable (boreas boreas.c)

set_target_properties (boreas PROPERTIES LINKER_LANGUAGE C)

include_directories (${GLIB_INCLUDE_DIRS}
                     ${LIBGVM_BASE_INCLUDE_DIRS}
                     ${LIBGVM_BOREAS_INCLUDE_DIRS})

set (CMAKE_C_FLAGS_DEBUG        "${CMAKE_C_FLAGS_DEBUG} -Werror -Wextra")

if (BOREAS_VERSION)
  add_definitions (-BOREAS_VERSION="${BOREAS_VERSION}")
endif (BOREAS_VERSION)

add_definitions (-DPREFIX="${CMAKE_INSTALL_PREFIX}")

if (BOREAS_RUN_DIR)
  add_definitions (-DBOREAS_RUN_DIR="${BOREAS_RUN_DIR}")
endif (BOREAS_RUN_DIR)

if (BOREAS_SYSCONF_DIR)
  add_definitions (-DBOREAS_SYSCONF_DIR="${BOREAS_SYSCONF_DIR}")
endif (BOREAS_SYSCONF_DIR)

if (BOREAS_LOG_DIR)
  add_definitions (-DBOREAS_LOG_DIR="${BOREAS_LOG_DIR}")
endif (BOREAS_LOG_DIR)

if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  set (EXECINFO_LDFLAGS "execinfo")
endif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")

target_link_libraries (boreas
                       ${LIBGVM_BASE_LDFLAGS}
                       ${LIBGVM_UTIL_LDFLAGS}
                       ${LIBGVM_BOREAS_LDFLAGS}
                       ${GLIB_LDFLAGS}
                       ${PCAP_LDFLAGS}
                       ${LINKER_HARDENING_FLAGS})

## Tests

## Static analysis

add_custom_target (cppcheck COMMENT "Running cppcheck..."
                   COMMAND sh -c \"cppcheck ${CMAKE_CURRENT_SOURCE_DIR}\")

add_custom_target (check COMMENT "Checking code...")
add_dependencies (check cppcheck)

## Tag files

set (C_FILES "boreas.c")
add_custom_target (etags COMMENT "Building TAGS..."
                   COMMAND etags ${C_FILES})
add_custom_target (ctags COMMENT "Building tags..."
                   COMMAND ctags ${C_FILES})
add_custom_target (tags COMMENT "Building tags files...")
add_dependencies (tags etags ctags)

## Install

install (TARGETS boreas
         RUNTIME DESTINATION ${BINDIR}
         PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE
         GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install (FILES ${CMAKE_BINARY_DIR}/src/boreas_log.conf
         DESTINATION ${BOREAS_SYSCONF_DIR})

install (FILES ${CMAKE_BINARY_DIR}/doc/boreas.8
         DESTINATION share/man/man8 )

## End

