#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"

#define NUM_TEST_STRINGS 1
const char *strings_to_tokenise[NUM_TEST_STRINGS] = { 
   "#include \"12some_header.h\"",
};

char tmp_filename[L_tmpnam];

int main(int UNUSED(argc), char UNUSED(**argv))
{
   int i;
   for (i = 0; i < NUM_TEST_STRINGS; i++)
   {
      int temp_fd = 0;
      ssize_t bytes_written = 0;
      ssize_t tok_str_len = strlen(strings_to_tokenise[i]);

      strncpy(tmp_filename, "mcc_test_XXXXXX", L_tmpnam);
      MCC_ASSERT(mkstemp(tmp_filename) != -1);

      temp_fd = open(tmp_filename, O_CREAT | O_WRONLY, S_IWUSR);
      MCC_ASSERT(temp_fd != -1);

      bytes_written = write(temp_fd, strings_to_tokenise[i], tok_str_len);
      while (bytes_written != tok_str_len)
      {
         bytes_written += write(temp_fd, &strings_to_tokenise[i][bytes_written],
                                tok_str_len - bytes_written);
      }

      close(temp_fd);

      mcc_TokeniseFile(tmp_filename);

      unlink(tmp_filename);
   }
   return 0;
}
