cmake_minimum_required(VERSION 3.22)

project(NDR_LAP
    VERSION 0.1.0
    DESCRIPTION "NDR Lexer and Parser"
    LANGUAGES C)

include(ExternalProject)

ExternalProject_Add(pcre2
GIT_REPOSITORY https://github.com/PCRE2Project/pcre2.git
CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/regex_engines
)

add_library(ndr_lap STATIC ndr_astnode.c ndr_asttokeninformation.c ndr_fileprocessor.c ndr_lexer.c ndr_parser.c ndr_debug.c ndr_regexstate.c ndr_sequenceinformation.c ndr_tokeninformation.c)

ADD_LIBRARY(libpcre2_1 STATIC IMPORTED)
ADD_LIBRARY(libpcre2_2 STATIC IMPORTED)

SET_TARGET_PROPERTIES(libpcre2_1 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/regex_engines/lib/libpcre2-8.a)
SET_TARGET_PROPERTIES(libpcre2_2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/regex_engines/lib/libpcre2-posix.a)

target_link_libraries(ndr_lap libpcre2_1)
target_link_libraries(ndr_lap libpcre2_2)


