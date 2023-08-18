
/*********************************************************************************
*                                    NDR Debug                                   *
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

#ifndef TOGGLES_H
#define TOGGLES_H

/** @brief STAT is the toggle for printing status messages for the successful completion of the NDR_Configure_Lexer, NDR_Configure_Parser, NDR_Lex, NDR_Parse functions */
extern bool NDR_STAT;
/** @brief ST is the toggle for printing the symbolTable */
extern bool NDR_ST;
/** @brief TT is the toggle for printing the tokenTable */
extern bool NDR_TT;
/** @brief TL is the toggle for printing the tokenLocations */
extern bool NDR_TL;
/** @brief M is the toggle for printing the matching process during the parsing process */
extern bool NDR_M;
/** @brief R is the toggle for regex symbol compilation */
extern bool NDR_R;
/** @brief PT is the toggle for printing the parseTable */
extern bool NDR_PT;

/** @brief NDR_Set_Toggles allows for toggles to be set through command line arguments that will print debugging information used in the NDR_Configure_Lexer, NDR_Configure_Parser, NDR_Lexer, NDR_Parser functions
*
* @param argc represents the number of toggles present
* @param argv is a list of strings. The strings can be any of the provided toggles.
*/
void NDR_Set_Toggles(int argc, char* argv[]);

#endif
