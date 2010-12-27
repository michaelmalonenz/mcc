#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "mcc.h"

/* To test this correctly, I will either have to do only the function
 * (static) declarations at the top of the file, having them automatic
 * when they're defined. And, of course give them external linkage when
 * MCC_DEBUG is defined.  I might ask John Carter whether he thinks this 
 * is even vaguely reasonable.  I suspect he'll give an answer similar to
 * "Used your own judgement" :)
 */

int main(int UNUSED(argc), char UNUSED(**argv))
{
   return 0;
}
