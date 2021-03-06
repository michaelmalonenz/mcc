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
#include <stdio.h>
#include <string.h>

#include "mcc.h"
#include "liberal.h"
#include "options.h"

char *options[] = {__FILE__, "-o", "outputFilename", "--ignore"};
const int numOptions = 4;

static void test_BasicOptions(void)
{
   mcc_ParseOptions(numOptions, options);
   MCC_ASSERT(strncmp(mcc_global_options.outputFilename,
                      options[2],
                      strlen(options[2])) == 0);
   mcc_TearDownOptions();
}

int main(void)
{
   test_BasicOptions();

   return 0;
}
