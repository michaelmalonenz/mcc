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
	FILE *file;
	char *filename;
	unsigned int line_no;
	unsigned int bufferIndex;
	unsigned char buffer[FILE_BUFFER_SIZE];
} mcc_FileBuffer_t;


//Do I actually want the fd_out to be a FILE *out so that the std* vars just work */
void mcc_PreprocessFile(FILE UNUSED(*inFile), FILE UNUSED(*outFile))
{
	//should probably make sure that the string to be pre-processed is NUL-terminated
	/* preprocessor directives:
	 * #define
	 * #undef
	 * #define(xxx) //function-ish macros; do I need to handle variadic macros separately?
	 * #if
	 * #ifdef
	 * #ifndef
	 * #elif
	 * #else
	 * #endif
	 * #error
	 * #pragma
	 * #include
	 */
}
