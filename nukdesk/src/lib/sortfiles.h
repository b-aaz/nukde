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


#ifndef SORTFILES_H
	#define SORTFILES_H

enum sorttype
{
    NAME=0,
    SIZE=1,
    TYPE=2,
    MTIME=3,
    CTIME=4,
    ATIME=5
};
struct sortby
{
    short int ac;
    enum sorttype st;
};
void sortfiles (struct fileinfo ** files, size_t  fnum , struct sortby st); 

#endif 
