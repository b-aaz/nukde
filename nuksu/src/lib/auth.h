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


#ifndef AUTH_H
	#define AUTH_H

enum auth_return
{
	AUTH_FAIL = 0,	/* Authentication failed . 		*/
	AUTH_SUCCESSES = 1,	/* Authentication was successful .	*/
	AUTH_NOPASS = 2 ,	/* Account does not have a password .	*/
	AUTH_LOCKED = -1 ,	/* Account is temporarily locked .	*/
	AUTH_NOLOGIN = -2 ,	/* Account does not allow logins .	*/
};

enum auth_return auth (char * enterdpasswd,char * passwd_fileloction);
#endif 
