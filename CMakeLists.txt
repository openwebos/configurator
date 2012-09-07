# @@@LICENSE
#
#      Copyright (c) 2007-2012 Hewlett-Packard Development Company, L.P.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# LICENSE@@@

cmake_minimum_required(VERSION 2.8.7)

project(configurator CXX)

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC2)
webos_component(2 0 0)

webos_add_compiler_flags(ALL "-DMOJ_LINUX -DMOJ_X86")

webos_machine_dep()

include_directories(src)

include(FindPkgConfig)

pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${GLIB2_CFLAGS})

pkg_check_modules(LUNASERVICE REQUIRED luna-service2)
include_directories(${LUNASERVICE_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LUNASERVICE_CFLAGS})

pkg_check_modules(CJSON cjson)
include_directories(${CJSON_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${CJSON_CFLAGS})

#pkg_check_modules(MOJODB mojodb)
#include_directories(${MOJODB_INCLUDE_DIRS})
#webos_add_compiler_flags(ALL ${MOJODB_CFLAGS})

find_library(MJCORE NAMES mojocore libmojocore)
if(MJCORE STREQUAL "MJCORE-NOTFOUND")
        message(FATAL_ERROR "Failed to find libmojocore. Please install openwebos/db8")
endif()

find_library(MJLUNA NAMES mojoluna libmojoluna)
if(MJLUNA STREQUAL "MJLUNA-NOTFOUND")
        message(FATAL_ERROR "Failed to find libmojoluna.  Please install openwebos/db8")
endif()

find_library(MJDB NAMES mojodb libmojodb)
if(MJDB STREQUAL "MJDB-NOTFOUND")
        message(FATAL_ERROR "Failed to find mojodb. Please install openwebos/db8")
endif()

find_path(MJ_INC NAMES mojodb/core/MojErr.h) 
if(MJ_INC STREQUAL "MJ_INC-NOTFOUND")
        message(FATAL_ERROR "Failed to find mojodb header. Please install openwebos/db8")
endif()
include_directories(${MJ_INC}/mojodb)

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(configurator ${SOURCE_FILES} )

target_link_libraries(configurator
                   	-L. 
  			${MJDB}
			${MJCORE}
			${MJLUNA}
                   	${GLIB2_LDFLAGS}
                   	${LUNASERVICE_LDFLAGS}
                   	${CJSON_LDFLAGS}
                   )

webos_configure_header_files(src)
webos_build_program()
webos_build_system_bus_files()
