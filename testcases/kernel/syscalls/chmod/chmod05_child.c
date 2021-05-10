/// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   Copyright (c) International Business Machines  Corp., 2001
 */

/*
 * Test Name: chmod05
 *
 * Test Description:
 *  Verify that, chmod(2) will succeed to change the mode of a directory
 *  but fails to set the setgid bit on it if invoked by non-root (uid != 0)
 *  process with the following constraints,
 *	- the process is the owner of the directory.
 *	- the effective group ID or one of the supplementary group ID's of the
 *	  process is not equal to the group ID of the directory.
 *
 * Expected Result:
 *  chmod() should return value 0 on success and though succeeds to change
 *  the mode of a directory but fails to set setgid bit on it.
 *
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>


#define MODE_RWX	(mode_t)(S_IRWXU | S_IRWXG | S_IRWXO)
#define DIR_MODE	(mode_t)(S_ISVTX | S_ISGID | S_IFDIR)
#define PERMS		(mode_t)(MODE_RWX | DIR_MODE)
#define TESTDIR		"/tmp/testdir"

int main(void)
{
	struct stat stat_buf;
	mode_t dir_mode;
	int ret;
	printf("Executing chmod05_child\n");
	
	ret = chmod(TESTDIR, PERMS);
	if (ret == -1) {
		printf("TFAIL | TERRNO, chmod(%s, %#o) failed\n", TESTDIR, PERMS);
		return -1;
	}

	stat(TESTDIR, &stat_buf);
	dir_mode = stat_buf.st_mode;
	if ((PERMS & ~S_ISGID) != dir_mode) {
		printf("TFAIL, %s: Incorrect modes 0%03o, "
				"Expected 0%03o\n", TESTDIR, dir_mode,
				PERMS & ~S_ISGID);
	} else {
		printf("TPASS, Functionality of chmod(%s, %#o) successful\n",
				TESTDIR, PERMS);
	}
	return 1;
}



