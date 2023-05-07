cmake_minimum_required(VERSION 3.00)

project(ndr_lap
    VERSION 0.1
    DESCRIPTION "NDR Lexer and Parser"
    LANGUAGES C)


add_library(ndr_astnode STATIC ndr_astnode.c)
add_library(ndr_astnodequeue STATIC ndr_astnodequeue.c)
add_library(ndr_astnodestack STATIC ndr_astnodestack.c)
add_library(ndr_asttokeninformation STATIC ndr_asttokeninformation.c)
add_library(ndr_fileprocessor STATIC ndr_fileprocessor.c)
add_library(ndr_lexer STATIC ndr_lexer.c)
add_library(ndr_parser STATIC ndr_parser.c)
add_library(ndr_regexstate STATIC ndr_regexstate.c)
add_library(ndr_sequenceinformation STATIC ndr_sequenceinformation.c)
add_library(ndr_tokeninformation STATIC ndr_tokeninformation.c)
