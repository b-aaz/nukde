#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "spawn.h"
pid_t spawn (char ** args, int * fd,enum spawn_flag mode)
{
	pid_t pid;
	int cstdin [2] ;
	int cstdout [2] ;

	if (mode  & SPAWN_READ)
	{
		pipe (cstdout);
		fd[0] = cstdout [0];
	}

	if (mode & SPAWN_WRITE)
	{
		pipe (cstdin);
		fd[1] = cstdin [1];
	}

	pid = fork() ;

	switch (pid)
	{
		case -1 :
			exit (0);

		case 0 :

			/*We are in the child*/
			if (mode  & SPAWN_READ)
			{
				dup2 (cstdout[1],1)  ;
				close (cstdout [0]) ;
				close (cstdout [1]) ;
			}

			if (mode & SPAWN_WRITE)
			{
				dup2 (cstdin[0],0)  ;
				close (cstdin [0]) ;
				close (cstdin [1]) ;
			}

			execv (*args,args) ;
			exit (0);

		default :
			break ;
	}

	if (mode & SPAWN_READ)
	{
		close (cstdout [1]) ;
	}

	if (mode & SPAWN_WRITE)
	{
		close (cstdin [0]) ;
	}

	return pid;
}

