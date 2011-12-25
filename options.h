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
#ifndef _MCC_OPTIONS_H_
#define _MCC_OPTIONS_H_

#include <stdint.h>

#include "mcc.h"
#include "list.h"

enum { PREPROCESS = 1,
       COMPILE    = (1 << 1),
       LINK       = (1 << 2)};

typedef struct {
   uint32_t stages;
   mcc_List_t *filenames;
   const char *outputFilename;
}mcc_Options_t;

extern mcc_Options_t mcc_global_options;

void mcc_ParseOptions(int argc, char **argv);

mcc_ListIterator_t *mcc_OptionsFileListGetIterator(void);

/**
 * A method mainly for unit testing.  It destroys the dynamic memory associated
 * with the option parameters, which in the real world, need to be in use for
 * the lifetime of the program.
 */ 
void mcc_TearDownOptions(void);

#endif /* _MCC_OPTIONS_H_ */
