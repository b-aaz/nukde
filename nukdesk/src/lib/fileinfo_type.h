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

#ifndef FILEINFO_TYPE
	#define FILEINFO_TYPE


#include <sys/event.h>
#include "../../../colibs/bool.h"
     #include <sys/stat.h>
#include <threads.h>

struct type
{
    char * humanreadable;
    char * mime;
    char * encode;
};
enum icon_type {
IMG,
EIE,
};
struct thrd_icon_load_args
{
    bool generateid;
    bool iconready;
    thrd_t thrd;
    enum icon_type icon_type;
    char * icon_path;
    long int icon_size;
    unsigned char * return_data;
} ;

struct fileinfo
{
    char * name;
    char * description;
    char * path;
    struct type type;
    struct stat f_stat;
    long long unsigned int f_size;
    struct nk_image * return_image;
    struct thrd_icon_load_args icon_load_args;
    bool deletded;
    bool isselected;
    struct kevent f_change;
    struct kevent f_event;
    int fd;
};

#endif 
