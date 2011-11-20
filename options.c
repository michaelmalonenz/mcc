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

#include "options.h"

mcc_Options_t mcc_global_options;

void mcc_ParseOptions(int argc, char **argv)
{
   int i;
   mcc_global_options.filenames = mcc_ListCreate();

   for (i = 1; i < argc; i++)
   {
      if (strncmp(argv[i], "-o", strlen(argv[i])) == 0)
      {
         i++;
         if (i == argc)
         {
            mcc_Error("The -o flag requires an output file argument\n");
         }
         mcc_global_options.outputFilename = argv[i];
      }
      else if (argv[i][0] == '-')
      {
         //do nothing;  Silently drop all unused arguments
      }
      else
      {
         mcc_ListAppendData(mcc_global_options.filenames, (void *) argv[i]);
      }
   }
}

mcc_ListIterator_t *mcc_OptionsFileListGetIterator(void)
{
   return mcc_ListGetIterator(mcc_global_options.filenames);
}

void mcc_TearDownOptions(void)
{
   mcc_ListDelete(mcc_global_options.filenames, NULL);
}
