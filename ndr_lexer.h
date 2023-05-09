
/*********************************************************************************
*                                     NDR Lexer                                  *
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

#ifndef NDRLEXER_H
#define NDRLEXER_H

/** @brief Configure the lexer based on a text input file so that the lexer is aware of the allowed tokens
* 
* @param fileName in the name of a text file filled with allowd tokens
* @return Thes success status of the function. 0 for success and non-zero for error
*/
int NDR_Configure_Lexer(char* fileName);
/** @brief Compare the tokens configured in function NDR_Configure_Lexer with the text found in a provided code file
*
* @param fileName is the name of a provided code file that is to be processed
* @return The success status of the function. 0 for success and non-zero for error
*/
int NDR_Lex(char* fileName);

/** @brief Print all of the tokens and associated regex found during parsing */
void NDR_PrintSymbolTable();
/** @brief Print all of the toekns and associated keywords found during parsing */
void NDR_PrintTokenTable();
/** @brief Print all of the toekn locations of tokens found during parsing */
void NDR_PrintTokenTableLocations();

#endif
