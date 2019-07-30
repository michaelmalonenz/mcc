/**
 Copyright (c) 2012, Michael Malone
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the original author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL MICHAEL MALONE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "mcc.h"
#include "liberal.h"

//A list is probably fine here.  If we need to resolve the filename, it's probably an error condition
typedef struct file_info {
   unsigned short fileno;
   char *filename;
} file_info_t;

static unsigned short current_file_number = 0;
static mcc_List_t *fileList = NULL;

static mcc_List_t *localIncludeDirList = NULL;
static mcc_List_t *systemIncludeDirList = NULL;

void mcc_FileOpenerInitialise(void)
{
   localIncludeDirList = mcc_ListCreate();
   systemIncludeDirList = mcc_ListCreate();

// @todo also append all dirs in the env var C_INCLUDE_PATH
#if defined(__linux)
   mcc_ListAppendData(systemIncludeDirList, (uintptr_t) "/usr/include");
   mcc_ListAppendData(systemIncludeDirList, (uintptr_t) "/usr/include/linux");
# if defined(__i386__)
   mcc_ListAppendData(systemIncludeDirList, (uintptr_t) "/usr/include/i386-linux-gnu");
# elif  defined(__x86_64__)
   mcc_ListAppendData(systemIncludeDirList, (uintptr_t) "/usr/include/x86_64-linux-gnu");
   // How do I deal with the GCC version?
   mcc_ListAppendData(systemIncludeDirList, (uintptr_t) "/usr/lib/gcc/x86_64-linux-gnu/8/include");
# endif
#endif

   fileList = mcc_ListCreate();
}

static void DeleteFileInfo(uintptr_t deathRow)
{
   file_info_t *toDelete = (file_info_t *) deathRow;
   free(toDelete->filename);
   toDelete->filename = NULL;
   free(toDelete);
}

void mcc_FileOpenerDelete(void)
{
   mcc_ListDelete(fileList, &DeleteFileInfo);
   mcc_ListDelete(localIncludeDirList, NULL);
   mcc_ListDelete(systemIncludeDirList, NULL);
}

void mcc_FileOpenerLocalIncAppendDir(const char *dir)
{
   mcc_ListAppendData(localIncludeDirList, (uintptr_t) dir);
}

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

   mcc_ListAppendData(fileList, (uintptr_t) newFile);
   
   return file;
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
   mcc_ListIterator_t *iter = mcc_ListGetIterator(localIncludeDirList);
   char *currentDir = (char *) mcc_ListGetNextData(iter);
   while (currentDir != NULL)
   {
      size_t dirLen = strlen(currentDir);
      size_t filenameLen = strlen(filename);
      bool_t needsTrailingSlash = currentDir[dirLen-1] != '/';
      size_t total = dirLen + filenameLen + 1;
      int fd;
      if (needsTrailingSlash)
         total++;
      char *temp = (char *) malloc(total);
      int i = 0;
      strncpy(temp, currentDir, dirLen);
      i += dirLen;
      if (needsTrailingSlash)
      {
         temp[i++] = '/';
      }
      strncpy(&temp[i], filename, filenameLen + 1);
      fd = open(temp, O_RDONLY);
      if (fd != -1)
      {
         close(fd);
         return temp;
      }
      free(temp);
      currentDir = (char *) mcc_ListGetNextData(iter);
   }

   char *result = (char *) malloc(strlen(filename) + 1);
   strncpy(result, filename, strlen(filename) + 1);
   return result;
}

char *mcc_FindSystemInclude(const char *filename)
{
   mcc_ListIterator_t *iter = mcc_ListGetIterator(systemIncludeDirList);
   char *currentDir = (char *) mcc_ListGetNextData(iter);
   while (currentDir != NULL)
   {
      size_t dirLen = strlen(currentDir);
      size_t filenameLen = strlen(filename);
      //+1 for NUL and +1 for '/'
      char *temp = (char *) malloc(filenameLen + dirLen + 2);
      int fd;
      strncpy(temp, currentDir, dirLen);
      temp[dirLen] = '/';
      strncpy(&temp[dirLen + 1], filename, filenameLen + 1);
      fd = open(temp, O_RDONLY);
      if (fd != -1)
      {
         close(fd);
         return temp;
      }
      free(temp);
      currentDir = (char *) mcc_ListGetNextData(iter);
   }
   return NULL;
}
