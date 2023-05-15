cmake_minimum_required(VERSION 3.22)

project(NDR_LAP
    VERSION 0.1.0
    DESCRIPTION "NDR Lexer and Parser"
    LANGUAGES C)

include(ExternalProject)

ExternalProject_Add(pcre2
GIT_REPOSITORY https://github.com/PCRE2Project/pcre2.git
CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/src/regex_engines/PCRE2
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(ndr_lap STATIC src/ndr_astnode.c src/ndr_asttokeninformation.c src/ndr_fileprocessor.c src/ndr_lexer.c src/ndr_parser.c src/ndr_debug.c src/ndr_regexstate.c src/ndr_sequenceinformation.c src/ndr_tokeninformation.c)

ADD_LIBRARY(libpcre2_1 STATIC IMPORTED)
ADD_LIBRARY(libpcre2_2 STATIC IMPORTED)

SET_TARGET_PROPERTIES(libpcre2_1 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/regex_engines/PCRE2/lib/libpcre2-8.a)
SET_TARGET_PROPERTIES(libpcre2_2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/regex_engines/PCRE2/lib/libpcre2-posix.a)

target_link_libraries(ndr_lap libpcre2_1)
target_link_libraries(ndr_lap libpcre2_2)

configure_file(${CMAKE_SOURCE_DIR}/src/ndr_lap.h ${CMAKE_SOURCE_DIR}/include/ndr_lap.h)

add_custom_command(TARGET ndr_lap POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/src/regex_engines/PCRE2/lib/ ${CMAKE_SOURCE_DIR}/lib/)
                       
add_custom_command(TARGET ndr_lap POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/src/regex_engines/PCRE2/include/ ${CMAKE_SOURCE_DIR}/include/)


