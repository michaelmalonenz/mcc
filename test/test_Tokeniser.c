#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"

const char *string_to_tokenise = "\
lkjfa;lsdkfjas;lkfja;lkj\
";

char tmp_filename[L_tmpnam];

int main(int UNUSED(argc), char UNUSED(**argv))
{
   int temp_fd = 0;
   ssize_t bytes_written = 0;
   ssize_t tok_str_len = strlen(string_to_tokenise);

   strncpy(tmp_filename, "mcc_test_XXXXXX", L_tmpnam);
   MCC_ASSERT(mkstemp(tmp_filename) != -1);

   temp_fd = open(tmp_filename, O_CREAT | O_WRONLY, S_IWUSR);
   MCC_ASSERT(temp_fd != -1);

   bytes_written = write(temp_fd, string_to_tokenise, tok_str_len);
   while (bytes_written != tok_str_len)
   {
      bytes_written += write(temp_fd, &string_to_tokenise[bytes_written],
                             tok_str_len - bytes_written);
   }

   close(temp_fd);

   unlink(tmp_filename);

   return 0;
}
