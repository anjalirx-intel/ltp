// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2018 Linaro Limited. All rights reserved.
 * Author: Rafael David Tinoco <rafael.tinoco@linaro.org>
 */

/*
 * In the user.* namespace, only regular files and directories can
 * have extended attributes. Otherwise fgetxattr(2) will return -1
 * and set proper errno.
 *
 * There are 7 test cases:
 *
 * 1. Get attribute from a regular file:
 *    - fgetxattr(2) should succeed
 *    - checks returned value to be the same as we set
 * 2. Get attribute from a directory:
 *    - fgetxattr(2) should succeed
 *    - checks returned value to be the same as we set
 * 3. Get attribute from a symlink which points to the regular file:
 *    - fgetxattr(2) should succeed
 *    - checks returned value to be the same as we set
 * 4. Get attribute from a FIFO:
 *    - fgetxattr(2) should return -1 and set errno to ENODATA
 * 5. Get attribute from a char special file:
 *    - fgetxattr(2) should return -1 and set errno to ENODATA
 * 6. Get attribute from a block special file:
 *    - fgetxattr(2) should return -1 and set errno to ENODATA
 * 7. Get attribute from a UNIX domain socket:
 *    - fgetxattr(2) should return -1 and set errno to ENODATA
 */

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#include "tst_test.h"

#ifdef HAVE_SYS_XATTR_H
#define XATTR_TEST_KEY "user.testkey"
#define XATTR_TEST_VALUE "this is a test value"
#define XATTR_TEST_VALUE_SIZE 20

#define MNTPOINT "/tmp/mntpoint"
#define OFFSET    11
#define FILENAME "/tmp/fgetxattr02testfile"
#define DIRNAME  "/tmp/fgetxattr02testdir"
#define SYMLINK  "fgetxattr02symlink"
#define SYMLINKF "fgetxattr02symlinkfile"
#define FIFO     MNTPOINT"/fgetxattr02fifo"
#define CHR      MNTPOINT"/fgetxattr02chr"
#define BLK      MNTPOINT"/fgetxattr02blk"
#define SOCK     "fgetxattr02sock"

struct test_case {
	char *fname;
	int fd;
	int fflags;
	char *key;
	char *value;
	size_t size;
	char *ret_value;
	int flags;
	int exp_err;
	int exp_ret;
	int issocket;
};
static struct test_case tc[] = {
	{			/* case 00, get attr from reg */
	 .fname = FILENAME,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 },
	{			/* case 01, get attr from dir */
	 .fname = DIRNAME,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 },
	{			/* case 02, get attr from symlink */
	 .fname = SYMLINK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = 0,
	 .exp_ret = XATTR_TEST_VALUE_SIZE,
	 },
	{			/* case 03, get attr from fifo */
	 .fname = FIFO,
	 .fflags = (O_RDONLY | O_NONBLOCK),
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 },
	{			/* case 04, get attr from character special */
	 .fname = CHR,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 },
	{			/* case 05, get attr from block special */
	 .fname = BLK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 },
	{			/* case 06, get attr from socket */
	 .fname = SOCK,
	 .fflags = O_RDONLY,
	 .key = XATTR_TEST_KEY,
	 .value = XATTR_TEST_VALUE,
	 .size = XATTR_TEST_VALUE_SIZE,
	 .ret_value = NULL,
	 .flags = XATTR_CREATE,
	 .exp_err = ENODATA,
	 .exp_ret = -1,
	 .issocket = 1,
	 },
};

static void verify_fgetxattr(unsigned int i)
{
	const char *fname = strstr(tc[i].fname, "fgetxattr02") + OFFSET;

	TEST(fgetxattr(tc[i].fd, tc[i].key, tc[i].ret_value, tc[i].size));

	if (TST_RET == -1 && TST_ERR == EOPNOTSUPP)
		tst_brk(TCONF, "fgetxattr(2) not supported");

	if (TST_RET >= 0) {

		if (tc[i].exp_ret == TST_RET) {
			tst_res(TPASS, "fgetxattr(2) on %s passed",
					fname);
		} else {
			tst_res(TFAIL,
				"fgetxattr(2) on %s passed unexpectedly %ld",
				fname, TST_RET);
		}

		if (strncmp(tc[i].ret_value, XATTR_TEST_VALUE,
				XATTR_TEST_VALUE_SIZE)) {
			tst_res(TFAIL, "wrong value, expect \"%s\" got \"%s\"",
					 XATTR_TEST_VALUE, tc[i].ret_value);
		}

		tst_res(TPASS, "fgetxattr(2) on %s got the right value",
				fname);
	}

	/*
	 * Before kernel 3.0.0, fgetxattr(2) will set errno with 'EPERM'
	 * when the file is not a regular file and directory, refer to
	 * commitid 55b23bd
	 */
	if (tc[i].exp_err == ENODATA && tst_kvercmp(3, 0, 0) < 0)
		tc[i].exp_err = EPERM;

	if (tc[i].exp_err == TST_ERR) {
		tst_res(TPASS | TTERRNO, "fgetxattr(2) on %s passed",
				fname);
		return;
	}

	tst_res(TFAIL | TTERRNO, "fgetxattr(2) failed on %s", fname);
}

static void setup(void)
{}

static void cleanup(void)
{
	size_t i = 0;

	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		free(tc[i].ret_value);

		if (tc[i].fd > 0)
			SAFE_CLOSE(tc[i].fd);
	}
}

static struct tst_test test = {
	.setup = setup,
	.test = verify_fgetxattr,
	.cleanup = cleanup,
	.tcnt = ARRAY_SIZE(tc),
	.needs_devfs = 1,
	// .mntpoint = MNTPOINT,
	.needs_root = 1,
};

#else /* HAVE_SYS_XATTR_H */
TST_TEST_TCONF("<sys/xattr.h> does not exist");
#endif
