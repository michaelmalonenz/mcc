#include <stdio.h>

#include "mcc.h"

FILE *mcc_OpenFile(const char *filename, char *flags)
{
   FILE *file = NULL;
   file = fopen(filename, flags);
   if (file == NULL)
   {
      mcc_Error("Couldn't open %s\n", filename);
   }
   return file;
}

const char *mcc_FindLocalInclude(const char *filename)
{
   return filename;
}

const char *mcc_FindSystemInclude(const char *filename)
{
   return filename;
}
