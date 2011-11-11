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

//A list is probably fine here.  If we need to resolve the filename, it's probably an error condition
//This is my second list, should I write a generic list yet?
typedef struct file_list {
   unsigned short fileno;
   char *filename;
   struct file_list *next;
} file_list_t;

static unsigned short current_file_number = 0;
static file_list_t *list_head;
static file_list_t *list_tail;

FILE *mcc_OpenFile(const char *filename, char *flags, unsigned short *out_fileno)
{
   FILE *file = NULL;
   file_list_t *newFile = (file_list_t *) malloc(sizeof(file_list_t));
   size_t filename_len = strlen(filename);
   file = fopen(filename, flags);
   if (file == NULL)
   {
      mcc_Error("Couldn't open the file '%s'\n", filename);
   }
   newFile->fileno = current_file_number++; 
   *out_fileno = newFile->fileno;
   newFile->filename = (char *) malloc((filename_len + 1) * sizeof(char));
   memcpy(newFile->filename, filename, filename_len);
   newFile->filename[filename_len] = '\0';
   if (list_head == NULL)
   {
      list_head = newFile;
      list_tail = newFile;
   }
   else
   {
      list_tail->next = newFile;
      list_tail = newFile;
   }
   return file;
}

const char *mcc_ResolveFileNameFromNumber(const unsigned short UNUSED(fileno))
{
   file_list_t *temp = list_head;
   while (temp != NULL)
   {
      if (temp->fileno == fileno)
      {
         return temp->filename;
      }
      temp = temp->next;
   }
   return NULL;
}

const char *mcc_FindLocalInclude(const char *filename)
{
   return filename;
}

const char *mcc_FindSystemInclude(const char *filename)
{
   return filename;
}
