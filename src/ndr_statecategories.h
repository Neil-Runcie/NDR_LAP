
/*********************************************************************************
*                               NDR State Categories                             *
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

#ifndef NDRSTATECATEGORIES_H
#define NDRSTATECATEGORIES_H

typedef enum NDR_StateCategories{
    NDR_STATE_INVALID, NDR_STATE_ACCEPT, NDR_STATE_IGNORE, NDR_STATE_ERROR, NDR_STATE_STARTSTATE, NDR_STATE_ALLOWSTATE, NDR_STATE_ESCAPESTATE, NDR_STATE_ENDSTATE, NDR_STATE_KEYWORD, NDR_STATE_LITERAL, NDR_STATE_STATES, NDR_STATE_ITEMS, NDR_STATE_SETTING, NDR_STATE_NOTAPPLICABLE
} NDR_StateCategories;

#endif
