
/*********************************************************************************
*                                    NDR Regex                                   *
**********************************************************************************/

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


#ifndef NDRREGEX_H
#define NDRREGEX_H

#include <stdbool.h>

/**
* \enum NDR_MatchResult
* \brief Provides codes for the result of the regex matching process
*/
typedef enum NDR_MatchResult {
    NDR_REGEX_FAILURE, NDR_REGEX_NOMATCH, NDR_REGEX_PARTIALMATCH, NDR_REGEX_COMPLETEMATCH
} NDR_MatchResult;

/**
* \struct NDR_RegexNode
* \brief The regex node struct acts as the nodes within the regex graphs used for regular expression comparison
*/
typedef struct NDR_RegexNode NDR_RegexNode;

/**
* \struct NDR_Regex
* \brief The regex struct provides the pattern matching functionality required for matching regular expressions
*/
typedef struct NDR_Regex {
    bool initialized;
    bool beginString;
    bool endString;
    bool isEmpty;
    char errorMessage[200];
    NDR_RegexNode* start;
} NDR_Regex;


/** @brief Initialize the NDR_Regex structure that will be used for regular expression comparision
*
* @param cRegex is an NDR_Regex pointer with sufficient memory already allocated
*/
void NDR_InitRegex(NDR_Regex* cRegex);
/** @brief Based on a provided regex string, create a reference graph for later matching the regex to other strings
*
* @param cRegex is an NDR_Regex pointer with sufficient memory already allocated
* @param regexString is the regex pattern that will be used to create the regex graph
* @return The success status of the function. 0 for success and non-zero for error
*/
int NDR_CompileRegex(NDR_Regex* cRegex, char* regexString);
/** @brief Compare a string to a pre-compiled regex graph
*
* @param cRegex is an NDR_Regex pointer with sufficient memory already allocated that has been used previously in the NDR_CompileRegex function
* @param token is the string that will be compared to the compiled regex graph
* @return The result of the match
*/
NDR_MatchResult NDR_MatchRegex(NDR_Regex* cRegex, char* token);
/** @brief Free the memory associated with items within the regex struct
*
* @param graph is a NDR_Regex pointer that has had memory assigned to it for compilation
*/
void NDR_DestroyRegex(NDR_Regex* graph);

/** @brief Get whether or not the NDR_Regex pointer has been compiled with a pattern
*
* @param ndrregex is a NDR_Regex pointer that has had memory assigned to it and has been used with the NDR_InitRegex function
* @return true or false value of whether or not compilation has been performed successfully
*/
bool NDR_Regex_IsCompiled(NDR_Regex* ndrregex);
/** @brief Get whether or not the NDR_Regex pointer expects to match starting only from the first character in the token string due to anchor in regex pattern
*
* @param ndrregex is a NDR_Regex pointer that has had memory assigned to it and has been used with the NDR_InitRegex function
* @return true or false value of whether or the NDR_Regex pointer expects to match starting only from the first character in the token string due to anchor in regex pattern
*/
bool NDR_Regex_HasBeginFlag(NDR_Regex* ndrregex);
/** @brief Get whether or not the NDR_Regex pointer expects to match exactly to the last character in the token string due to anchor in regex pattern
*
* @param ndrregex is a NDR_Regex pointer that has had memory assigned to it and has been used with the NDR_InitRegex function
* @return true or false value of whether or not the NDR_Regex pointer expects to match exactly to the last character in the token string due to anchor in regex pattern
*/
bool NDR_Regex_HasEndFlag(NDR_Regex* ndrregex);
/** @brief Get whether or not the NDR_Regex pointer was compiled with an empty regex pattern string
*
* @param ndrregex is a NDR_Regex pointer that has had memory assigned to it and has been used with the NDR_InitRegex function
* @return true or false value of whether or not the NDR_Regex pointer was compiled with an empty regex pattern string
*/
bool NDR_Regex_IsEmpty(NDR_Regex* ndrregex);
/** @brief Get the message describing what happened during compilation if an error occured
*
* @param ndrregex is a NDR_Regex pointer that has had memory assigned to it and has been used with the NDR_InitRegex function
* @return the error message string
*/
char* NDR_Regex_GetErrorMessage(NDR_Regex* ndrregex);
/** @brief Get the start node of the graph used for regex comparison
*
* @param ndrregex is a NDR_Regex pointer that has had memory assigned to it and has been used with the NDR_InitRegex function
* @return the start node of the graph used for regex comparison
*/
NDR_RegexNode* NDR_Regex_GetStartNode(NDR_Regex* ndrregex);

#endif
