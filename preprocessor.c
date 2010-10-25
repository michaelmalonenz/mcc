#include <fcntl.h>
#include <unistd.h>
#include "mcc.h"

void mcc_PreprocessFile(const char *in_filename, const char *out_filename)
{
	int in_fd;
	int out_fd;
	in_fd  = open(in_filename, O_RDONLY);
	if (in_fd == 0)
	{
		mcc_Error("Couldn't open %s", in_filename);
	}
	out_fd = open(out_filename, O_RDWR | O_CREAT);
	if (out_fd == 0)
	{
		mcc_Error("CHECK THE ERRNO AND GIVE A DECENT ANSWER");
	}
}
