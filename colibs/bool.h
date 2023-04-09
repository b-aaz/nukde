/*
Copyright 2022, 2023 B-aaz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
 any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * __attribute__ is a gnu c extension we do not want to use with 
 * other compilers 
 */
#ifndef NUKDE_BOOL
	#define NUKDE_BOOL

#ifdef __GNUC__
	#define NOPAD __attribute__((packed))
#else 
	#define NOPAD
#endif

typedef 
NOPAD
enum
{false=0, true=!false} bool;
#endif
