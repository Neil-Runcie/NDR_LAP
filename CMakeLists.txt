cmake_minimum_required(VERSION 3.22)

project(NDR_LAP
    VERSION 1.0.0
    DESCRIPTION "NDR Lexer and Parser"
    LANGUAGES C)

include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)


ExternalProject_Add(pcre2
 GIT_REPOSITORY https://github.com/PCRE2Project/pcre2.git
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/regex_engines
)

add_library(ndr_lap STATIC ndr_astnode.c ndr_asttokeninformation.c ndr_fileprocessor.c ndr_lexer.c ndr_parser.c ndr_regexstate.c ndr_sequenceinformation.c ndr_tokeninformation.c)
