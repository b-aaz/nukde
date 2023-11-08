/*
* Copyright (c) 2022-2023 B-aaz .  
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FILEINFO_TYPE_H
#define FILEINFO_TYPE_H


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
enum icon_type
{
	IMG,
	EIE,
};
struct thrd_icon_load_args
{
	bool generateid;
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
	unsigned int * ic_copy_count;
	struct thrd_icon_load_args icon_load_args;
	bool deletded;
	bool isselected;
	struct kevent f_change;
	struct kevent f_event;
	int fd;
};

#endif
