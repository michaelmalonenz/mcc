#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mcc.h"
#include "liberal.h"

char tmp_filename[L_tmpnam];

const char *mcc_TestUtils_DumpStringToTempFile(const char *string, int string_len)
{
   int temp_fd = 0;
   ssize_t bytes_written = 0;
   strncpy(tmp_filename, "mcc_test_XXXXXX", L_tmpnam);
   MCC_ASSERT(mkstemp(tmp_filename) != -1);

   temp_fd = open(tmp_filename, O_CREAT | O_WRONLY, S_IWUSR);
   MCC_ASSERT(temp_fd != -1);

   bytes_written = write(temp_fd, string, string_len);
   while (bytes_written != string_len)
   {
      bytes_written += write(temp_fd, &string[bytes_written],
                             string_len - bytes_written);
   }
   
   close(temp_fd);

   return tmp_filename;
}
