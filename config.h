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
#ifndef MCC_CONFIG_H
#define MCC_CONFIG_H

/** Only one of the following three should be defined at any one time **/
#define MCC_USE_HASH_TABLE_FOR_MACROS (1)
#define MCC_USE_B_TREE_FOR_MACROS (0)
#define MCC_USE_LIST_FOR_MACROS (0)


#ifndef MCC_DEBUG
#define MCC_DEBUG (1)
#endif

#define MCC_USE_DYNAMIC_STRING_BUFFER_TOKENISER (1)

#define MCC_C99_COMPATIBLE (1)

#endif
