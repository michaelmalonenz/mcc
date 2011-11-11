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
