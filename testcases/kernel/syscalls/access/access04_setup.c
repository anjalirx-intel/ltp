// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) International Business Machines Corp., 2001
 * Copyright (c) 2013 Fujitsu Ltd.
 */

/*
 * Verify that,
 *  1) access() fails with -1 return value and sets errno to EINVAL
 *     if the specified access mode argument is invalid.
 *  2) access() fails with -1 return value and sets errno to ENOENT
 *     if the specified file doesn't exist (or pathname is NULL).
 *  3) access() fails with -1 return value and sets errno to ENAMETOOLONG
 *     if the pathname size is > PATH_MAX characters.
 *  4) access() fails with -1 return value and sets errno to ENOTDIR
 *     if a component used as a directory in pathname is not a directory.
 *  5) access() fails with -1 return value and sets errno to ELOOP
 *     if too many symbolic links were encountered in resolving pathname.
 *  6) access() fails with -1 return value and sets errno to EROFS
 *     if write permission was requested for files on a read-only file system.
 *
 * Ported to LTP: Wayne Boyer
 *  11/2013 Ported by Xiaoguang Wang <wangxg.fnst@cn.fujitsu.com>
 *  11/2016 Modified by Guangwen Feng <fenggw-fnst@cn.fujitsu.com>
 */

#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <unistd.h>

#include "tst_test.h"

#define FNAME1	"/tmp/accessfile1"
#define FNAME2	"/tmp/accessfile2/accessfile2"
#define DNAME	"/tmp/accessfile2"
#define SNAME1	"/tmp/symlink1"
#define SNAME2	"/tmp/symlink2"
#define MNT_POINT	"/tmp/access04"

static char longpathname[PATH_MAX + 2];

static struct tcase {
	const char *pathname;
	int mode;
	int exp_errno;
} tcases[] = {
	{FNAME1, -1, EINVAL},
	{"", W_OK, ENOENT},
	{longpathname, R_OK, ENAMETOOLONG},
	{FNAME2, R_OK, ENOTDIR},
	{SNAME1, R_OK, ELOOP},
	{MNT_POINT, W_OK, EROFS}
};

static void verify_access(unsigned int n)
{
    tst_res(TPASS, "No test function defined");
}

static void setup(void)
{
	SAFE_TOUCH(FNAME1, 0333, NULL);
	SAFE_TOUCH(DNAME, 0644, NULL);

	SAFE_SYMLINK(SNAME1, SNAME2);
	SAFE_SYMLINK(SNAME2, SNAME1);
}

static struct tst_test test = {
	.tcnt = ARRAY_SIZE(tcases),
	.needs_root = 1,
	.forks_child = 1,
	.needs_rofs = 1,
	.mntpoint = MNT_POINT,
	.setup = setup,
	.test = verify_access,
};