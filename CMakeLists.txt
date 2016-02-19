project(sqlite3)

cmake_minimum_required(VERSION 2.4.0)
enable_language(C ASM)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2 -msse3 -w -nostdinc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__DARWIN_UNIX03 -fPIC -U__linux__")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib -Wl,--version-script=${DARLING_TOP_DIRECTORY}/darwin.map")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -Wl,--version-script=${DARLING_TOP_DIRECTORY}/darwin.map")

add_definitions(-DHAVE_CONFIG_H)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${DARLING_TOP_DIRECTORY}/platform-include)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/darling")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(sqlite3 SHARED sqlite3.c)
target_link_libraries(sqlite3 system)

install(TARGETS sqlite3 DESTINATION ${CMAKE_INSTALL_LIBDIR}/darling)

if (NOT DARLING_NO_EXECUTABLES)
	include(darling_exe)
	add_darling_executable(sqlite shell.c)
	target_link_libraries(sqlite sqlite3)

	install(TARGETS sqlite DESTINATION libexec/darling/usr/bin)
endif (NOT DARLING_NO_EXECUTABLES)

