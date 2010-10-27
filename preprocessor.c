#include <fcntl.h>
#include <unistd.h>
#include "mcc.h"

/* A good, meaty base-2 chunk of a file, so we don't start reading the thing
 * from disk a character at a time
 */
#define FILE_BUFFER_SIZE 32768

/* Should avoid allocating these on the stack - it could use up memory much too quickly
 * (stacks are usually limited to around 8MB) Which, I suppose is big enough to hold a 
 * decent amount, but don't I want to leave plenty of room for other stuff?
 */
typedef struct FileBuffer {
	int fd;
	char *filename;
	unsigned int line_no;
	unsigned int bufferIndex;
	unsigned char buffer[FILE_BUFFER_SIZE];
} mcc_FileBuffer_t;


void mcc_PreprocessFile(const int UNUSED(fd_in), const int UNUSED(fd_out))
{
	
}
