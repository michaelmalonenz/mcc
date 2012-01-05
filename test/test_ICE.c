/**
    mcc a lightweight compiler for developers, not machines
    Copyright (C) 2011 Michael Malone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#include <string.h>

#include "mcc.h"
#include "TestUtils.h"
#include "toolChainCommands.h"
#include "tokenList.h"
#include "ICE.h"

const char *ice_is_zero = "10 * (1 + 1 - 2)\n";
//const char *ice_is_nonzero_twenty = "10 * (1 + 3 - 2)\n";


int main(void)
{
   const char *file = mcc_TestUtils_DumpStringToTempFile(ice_is_zero, strlen(ice_is_zero));
   mcc_TokenListIterator_t *iter = mcc_TokenListGetIterator();
   mcc_FileOpenerInitialise();
   mcc_TokeniseFile(file, iter);
   mcc_TokenListDeleteIterator(iter);

   iter = mcc_TokenListGetIterator();
   MCC_ASSERT(mcc_ICE_EvaluateTokenString(iter) == 0);
   mcc_TokenListDeleteIterator(iter);

   mcc_FreeTokens();
   mcc_FileOpenerDelete();
   return 0;
}
