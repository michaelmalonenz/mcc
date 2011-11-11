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
#ifndef _MCC_MACRO_PRIVATE_H_
#define _MCC_MACRO_PRIVATE_H_

void delete_macro(mcc_Macro_t *macro);

mcc_Macro_t *create_macro(const char *text, const char *value);

#endif /* _MCC_MACRO_PRIVATE_H_ */
