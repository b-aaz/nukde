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

/*                  User authentication library for FreeBSD                  */

/*#define AUTH_DEBG*/
#define USERNAME "root"
#define USERNAMEL 4
#include <stdio.h>                 // for fseek, size_t, fclose, fread, ftell
#include <stdlib.h>                // for free, NULL, malloc
#include <string.h>                // for strcmp, strncmp, strncpy, strchr
#include <unistd.h>                // for crypt, NULL

#define REDEFFUNCS
#define NUERRREDEFFUNCS
#define NUERRSTDIO
#define NUERRSTDLIB
#define NUERRCOLORRE "\e[0m"
#define NUERRCOLOR "\e[38;2;237;67;55;1;5m"
#include "../../../colibs/bool.h"  // for true, bool, false
#include "../../../colibs/err.h"   // for fopen
#include "auth.h"

/* Goes through the passwd file opened in a char string named |buffer|
 * line by line searching for the line starting with /USERNAME/ + ':' (The
 * users record) .
 * After the record has been found and its length determined a buffer is
 * allocated for it and the line will be copied to it and then returned .
 */
/* Warning : Return value of this function should be freed when not needed . */
static char * get_user_record (char * buffer)
{
	char * user_record=NULL;
	size_t char_index=0;
	bool newline = true ;

	while (char_index < strlen (buffer))
	{
		if (newline)
		{
			if (strncmp (buffer+char_index, USERNAME":", USERNAMEL+1) == 0)
			{
				buffer += char_index ;
				size_t linelength = 0;

				while (buffer[linelength] != '\n')
				{
					linelength++;
				}

				linelength++;
				user_record =
					(char *) malloc ( (linelength) * sizeof (char));
				strlcpy (user_record,buffer,linelength);
				break;
			}
		}

		newline=buffer[char_index]=='\n'?true:false;
		char_index++;
	}
	if (!user_record){
	die("%s\n","No user record found");
	}
	return user_record;
}
/* Extracts the users encrypted passwords "hash" out of the given user record
 * string |user_record| .
 * The record has many fields separated by colons (':') the second field is the
 * password hash which is the data we need . If the field is empty (Meaning the
 * user has no password) this function returns NULL . For more info see
 * master.passwd(5) and crypt(3) .
 */
/* Warning : Return value of this function should be freed when not needed . */
static char * get_hash (char * user_record)
{
	char * hash ;
	size_t hashl=0;

	user_record = strchr (user_record, ':');
	user_record++;

	while (user_record[hashl] != ':')
	{
		hashl++;
	}

	if (hashl == 0)
	{
		return NULL ;
	}

	hash = malloc ( (hashl+1) * sizeof (char));
	strncpy (hash, user_record, hashl);
	hash [hashl]='\0';
	return hash;
}

/* Extracts the hash's salt and what I started to call "paper", out of the hash
 * string given by |hash| ; Paper is a digit surrounded by a pair of
 * dollar signs ('$') behind the salt that indicates what type of hashing
 * algorithm is used to encrypt the password .
 * A typical user record looks like this :
 * "user:hash:stuff:we:do:not:need"
 * and a typical (Modular) hash looks like this :
 * "$digit$salt$the_actual_hash"
 *  |     |    |
 *  _______    |
 *   paper     |
 *  |          |
 *  ____________
 *   "papersalt"
 * Also the hash can be prepended by the phrase "*LOCKED*" to indicate that no
 * one can login to that account and it's locked out .
 * so the function returns NULL on this condition . For more info see
 * master.passwd(5) and crypt(3) .
 */
/* Warning : Return value of this function should be freed when not needed . */
static char * get_papersalt (char * hash)
{
	size_t papersaltl;
	char * papersalt;

	if (strncmp (hash, "*LOCKED*",8) ==0)
	{
		return NULL ;
	}

	char * last_dollar_sign_pos;
	last_dollar_sign_pos = strrchr (hash,'$');
	papersaltl = last_dollar_sign_pos - hash ;
	papersalt = malloc ( (papersaltl+1) * sizeof (char));
	strncpy (papersalt,hash,papersaltl);
	papersalt [papersaltl]= '\0';
	return papersalt;
}
/* Authenticates the user /USERNAME/ with the password |enterdpasswd| with the
 * user record in the file |passwd_fileloction| .
 * The function returns a value of type 'enum auth_return' depending on
 * the authentications result .
 */
enum auth_return auth (char * enterdpasswd,char * passwd_fileloction)
{
	char * user_record=NULL;
	FILE * passwd_file=NULL;
	char * buffer=NULL;
	long passwd_filesize=0;
	char * papersalt=NULL;
	char * rootusershash=NULL;
	char * enterdpasswd_hash=NULL;
	size_t paperl=0;
	size_t papersaltl=0;
	bool ret ;
	passwd_file=fopen (passwd_fileloction, "r");
	fseek (passwd_file, 0L, SEEK_END);
	passwd_filesize = ftell (passwd_file);
	fseek (passwd_file, 0L, SEEK_SET);
	buffer = (char *) malloc (passwd_filesize + 1);
	fread (buffer, sizeof (char), passwd_filesize, passwd_file);
	fclose (passwd_file);
	buffer[passwd_filesize]='\0';
	user_record = get_user_record (buffer);
	free (buffer);
	rootusershash= get_hash (user_record);

	if (rootusershash == NULL)
	{
		return AUTH_NOPASS;
	}
	else if (strcmp (rootusershash, "*") == 0)
	{
		return AUTH_NOLOGIN ;
	}

	free (user_record);
	papersalt = get_papersalt (rootusershash);

	if (papersalt == NULL)
	{
		return AUTH_LOCKED;
	}

	enterdpasswd_hash = crypt (enterdpasswd, papersalt);
	free (papersalt);
#ifdef AUTH_DEBG
	printf ("Hash of the entered password:\n"
			"%s\n"
			"The real hash:\n"
			"%s\n"
			,enterdpasswd_hash,rootusershash);
#endif
	ret = strcmp (enterdpasswd_hash, rootusershash) == 0 ? true : false ;
	free (rootusershash);
	return ret;
}
