CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

project(NDR_LAP
    VERSION 0.2.0
    DESCRIPTION "NDR Lexer and Parser"
    LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(ndr_lap STATIC src/ndr_astnode.c src/ndr_asttokeninformation.c src/ndr_fileprocessor.c src/ndr_lexer.c src/ndr_parser.c src/ndr_debug.c src/ndr_regexstate.c src/ndr_sequenceinformation.c src/ndr_tokeninformation.c src/ndr_cregex.c src/regex_engines/ndr_regex.c src/regex_engines/ndr_regextracker.c src/regex_engines/ndr_regexnode.c)

ADD_LIBRARY(libndr_cregex STATIC IMPORTED)

SET_TARGET_PROPERTIES(libndr_cregex PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/regex_engines/NDR_CRegex/lib/libndr_cregex.a)

target_link_libraries(ndr_lap libndr_cregex)

configure_file(${CMAKE_SOURCE_DIR}/src/ndr_lap.h ${CMAKE_SOURCE_DIR}/include/ndr_lap.h)


install (TARGETS ndr_lap
         ARCHIVE DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
         LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
         RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
         
