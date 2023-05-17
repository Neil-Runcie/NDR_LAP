
/*
BSD 3-Clause License

Copyright (c) 2023, Neil Runcie

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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


