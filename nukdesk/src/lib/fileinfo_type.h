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


#include "../../../colibs/bool.h"
#include <sys/event.h>
     #include <sys/stat.h>
#include <threads.h>

struct magicline
{
    char * humanreadable;
    char * mime;
    char * encode;
};
struct thrd_icon_load_args
{
    bool genid;
    thrd_t thrd;
    char * type;
    char * icon_path;
    long int icon_size;
    unsigned char * return_data;
} ;

struct fileinfo
{
    bool deletded;
    bool isselected;
    struct kevent f_change;
    struct kevent f_event;
    int fd;
    char * name;
    char * description;
    char * path;
    struct nk_image * return_image;
    long long unsigned int f_size;
    struct stat f_stat;
    struct thrd_icon_load_args icon_load_args;
    struct magicline magic;
};
