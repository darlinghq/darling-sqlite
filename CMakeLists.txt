project(sqlite3)

cmake_minimum_required(VERSION 3.13)
enable_language(C ASM)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2 -msse3 -w -nostdinc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__DARWIN_UNIX03 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DSQLITE_ENABLE_FTS5)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

set(DYLIB_INSTALL_NAME "/usr/lib/libsqlite3.0.dylib")
set(DYLIB_COMPAT_VERSION "9.0.0")
set(DYLIB_CURRENT_VERSION "252.0.0")

add_darling_library(sqlite3 SHARED sqlite3.c)
make_fat(sqlite3)
target_link_libraries(sqlite3 system)
set_target_properties(sqlite3 PROPERTIES OUTPUT_NAME "sqlite3.0")

InstallSymlink("libsqlite3.0.dylib" "${CMAKE_INSTALL_PREFIX}/libexec/darling/usr/lib/libsqlite3.dylib")
install(TARGETS sqlite3 DESTINATION libexec/darling/usr/lib)

if (NOT DARLING_NO_EXECUTABLES)
	include(darling_exe)
	add_darling_executable(sqlite shell.c)
	target_link_libraries(sqlite sqlite3)

	install(TARGETS sqlite DESTINATION libexec/darling/usr/bin)
endif (NOT DARLING_NO_EXECUTABLES)

