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
#include "list.h"

//A list is probably fine here.  If we need to resolve the filename, it's probably an error condition
typedef struct file_info {
   unsigned short fileno;
   char *filename;
} file_info_t;

static unsigned short current_file_number = 0;
static mcc_List_t *fileList = NULL;

FILE *mcc_OpenFile(const char *filename, const char *flags,
                   unsigned short *out_fileno)
{
   FILE *file = NULL;
   file_info_t *newFile = (file_info_t *) malloc(sizeof(file_info_t));
   size_t filename_len = strlen(filename);
   file = fopen(filename, flags);
   if (file == NULL)
   {
      mcc_Error("Couldn't open the file '%s'\n", filename);
   }
   newFile->fileno = current_file_number++; 
   *out_fileno = newFile->fileno;
   newFile->filename = (char *) malloc((filename_len + 1) * sizeof(char));
   strncpy(newFile->filename, filename, filename_len + 1);

   if (fileList == NULL)
   {
      fileList = mcc_ListCreate();
   }
   mcc_ListAppendData(fileList, newFile);
   
   return file;
}

static void DeleteFileInfo(void *deathRow)
{
   file_info_t *toDelete = (file_info_t *) deathRow;
   free(toDelete->filename);
   toDelete->filename = NULL;
   free(toDelete);
}

void mcc_FileOpenerDelete(void)
{
   mcc_ListDelete(fileList, &DeleteFileInfo);
}

const char *mcc_ResolveFileNameFromNumber(const unsigned short fileno)
{
   mcc_ListIterator_t *iter = NULL;
   file_info_t *temp = NULL;

   if (fileList == NULL)
      return NULL;

   iter = mcc_ListGetIterator(fileList);
   temp = (file_info_t *) mcc_ListGetNextData(iter);
   while (temp != NULL)
   {
      if (temp->fileno == fileno)
      {
         mcc_ListDeleteIterator(iter);
         return temp->filename;
      }
      temp = (file_info_t *) mcc_ListGetNextData(iter);
   }

   /* We couldn't find it, so free the iterator and return NULL */
   mcc_ListDeleteIterator(iter);
   return NULL;
}

char *mcc_FindLocalInclude(const char *filename)
{
   char *result = (char *) malloc(strlen(filename) + 1);
   strncpy(result, filename, strlen(filename) + 1);
   return result;
}

char *mcc_FindSystemInclude(const char *filename)
{
   char *result = (char *) malloc(strlen(filename) + 1);
   strncpy(result, filename, strlen(filename) + 1);
   return result;
}
