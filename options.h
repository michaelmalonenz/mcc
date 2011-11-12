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

#include "mcc.h"
#include "list.h"

typedef struct {
   bool_t preprocess_only;
   mcc_List_t *filenames;
   char *outputFilename;
}mcc_Options_t;

extern mcc_Options_t mcc_global_options;

void mcc_ParseOptions(int *argc, const char **argv);

#endif /* _MCC_OPTIONS_H_ */

