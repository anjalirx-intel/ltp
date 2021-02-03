/*
 *
 *   Copyright (c) International Business Machines  Corp., 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * NAME
 *	rename04
 *
 * DESCRIPTION
 *	This test will verify that rename(2) failed when newpath is
 *      a non-empty directory and return EEXIST or ENOTEMPTY
 *
 * ALGORITHM
 *	Setup:
 *		Setup signal handling.
 *		Create temporary directory.
 *		Pause for SIGUSR1 if option specified.
 *              create the "old" directory and the "new" directory
 *              create a file uner the "new" directory
 *
 *	Test:
 *		Loop if the proper options are given.
 *                  rename the "old" to the "new" directory
 *                  verify rename() failed and returned ENOTEMPTY
 *
 *	Cleanup:
 *		Print errno log and/or timing stats if options given
 *		Delete the temporary directory created.
 *
 * USAGE
 *	rename04 [-c n] [-e] [-i n] [-I x] [-P x] [-t]
 *	where,  -c n : Run n copies concurrently.
 *		-e   : Turn on errno logging.
 *		-i n : Execute test n times.
 *		-I x : Execute test for x seconds.
 *		-P x : Pause for x seconds between iterations.
 *		-t   : Turn on syscall timing.
 *
 * HISTORY
 *	07/2001 Ported by Wayne Boyer
 *
 * RESTRICTIONS
 *	None.
 */
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "test.h"
#include "safe_macros.h"

void setup();
void cleanup();

char *TCID = "rename04";
int TST_TOTAL = 1;

int fd;

char fdir[255] = "/tmp/rename04_fdir";
char mdir[255] = "/tmp/rename04_mdir";
char tstfile[255] = "/tmp/rename04_mdir/rename04_tstfile";
struct stat buf1, buf2;
dev_t olddev, olddev1;
ino_t oldino, oldino1;

int main(int ac, char **av)
{
	int lc;

	/*
	 * parse standard options
	 */
	tst_parse_opts(ac, av, NULL, NULL);

	/*
	 * perform global setup for test
	 */
	setup();

	/*
	 * cleanup and exit
	 */
	cleanup();

}

/*
 * setup() - performs all ONE TIME setup for this test.
 */
void setup(void)
{

	tst_sig(NOFORK, DEF_HANDLER, cleanup);

	TEST_PAUSE;

	/* Create a temporary directory and make it current. */
	tst_tmpdir();

	/* create "old" directory */
	SAFE_MKDIR(cleanup, fdir, 0770);

	/* create another directory */
	SAFE_MKDIR(cleanup, mdir, 0770);

	SAFE_TOUCH(cleanup, tstfile, 0700, NULL);

}

/*
 * cleanup() - performs all ONE TIME cleanup for this test at
 *              completion or premature exit.
 */
void cleanup(void)
{

	/*
	 * Remove the temporary directory.
	 */
	tst_rmdir();
}
