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


#ifndef SPAWN
	#define SPAWN

enum spawn_flag
{
	SPAWN_NONE=0,
	SPAWN_READ = 2,
	SPAWN_WRITE=4,
	SPAWN_RW=6
};
pid_t spawn (char ** args, int * fd,enum spawn_flag mode);

#endif 
