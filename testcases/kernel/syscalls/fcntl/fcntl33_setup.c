// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015 Fujitsu Ltd.
 * Author: Guangwen Feng <fenggw-fnst@cn.fujitsu.com>
 */

/*
 * DESCRIPTION
 *  Test for feature F_SETLEASE of fcntl(2).
 *  "F_SETLEASE is used to establish a lease which provides a mechanism:
 *   When a process (the lease breaker) performs an open(2) or truncate(2)
 *   that conflicts with the lease, the system call will be blocked by
 *   kernel, meanwhile the kernel notifies the lease holder by sending
 *   it a signal (SIGIO by default), after the lease holder successes
 *   to downgrade or remove the lease, the kernel permits the system
 *   call of the lease breaker to proceed."
 */

#include <errno.h>

#include "tst_test.h"
#include "tst_timer.h"
#include "tst_safe_macros.h"

/*
 * MIN_TIME_LIMIT is defined to 5 senconds as a minimal acceptable
 * amount of time for the lease breaker waiting for unblock via
 * lease holder voluntarily downgrade or remove the lease, if the
 * lease breaker is unblocked within MIN_TIME_LIMIT we may consider
 * that the feature of the lease mechanism works well.
 *
 * The lease-break-time is set to 45 seconds for timeout in kernel.
 */
#define MIN_TIME_LIMIT	5

#define OP_OPEN_RDONLY	0
#define OP_OPEN_WRONLY	1
#define OP_OPEN_RDWR	2
#define OP_TRUNCATE	3

#define FILE_MODE	(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID)
#define PATH_LS_BRK_T	"/proc/sys/fs/lease-break-time"

static void do_test(unsigned int);
static void do_child(unsigned int);

static int fd;
static int ls_brk_t;
static long type;
static sigset_t newset, oldset;

/* Time limit for lease holder to receive SIGIO. */
static struct timespec timeout = {.tv_sec = 5};

static struct test_case_t {
	int lease_type;
	int op_type;
	char *desc;
} test_cases[] = {
	{F_WRLCK, OP_OPEN_RDONLY,
		"open(O_RDONLY) conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_WRLCK, OP_OPEN_WRONLY,
		"open(O_WRONLY) conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_WRLCK, OP_OPEN_RDWR,
		"open(O_RDWR) conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_WRLCK, OP_TRUNCATE,
		"truncate() conflicts with fcntl(F_SETLEASE, F_WRLCK)"},
	{F_RDLCK, OP_OPEN_WRONLY,
		"open(O_WRONLY) conflicts with fcntl(F_SETLEASE, F_RDLCK)"},
	{F_RDLCK, OP_OPEN_RDWR,
		"open(O_RDWR) conflicts with fcntl(F_SETLEASE, F_RDLCK)"},
	{F_RDLCK, OP_TRUNCATE,
		"truncate() conflicts with fcntl(F_SETLEASE, F_RDLCK)"},
};

static void setup(void)
{
	tst_timer_check(CLOCK_MONOTONIC);

	/* Backup and set the lease-break-time. */
	SAFE_FILE_SCANF(PATH_LS_BRK_T, "%d", &ls_brk_t);
	SAFE_FILE_PRINTF(PATH_LS_BRK_T, "%d", 45);

	switch ((type = tst_fs_type("."))) {
	case TST_NFS_MAGIC:
	case TST_RAMFS_MAGIC:
	case TST_TMPFS_MAGIC:
		tst_brk(TCONF,
			"Cannot do fcntl(F_SETLEASE, F_WRLCK) on %s filesystem",
			tst_fs_type_name(type));
	default:
		break;
	}

	SAFE_TOUCH("/tmp/file", FILE_MODE, NULL);

	sigemptyset(&newset);
	sigaddset(&newset, SIGIO);

	if (sigprocmask(SIG_SETMASK, &newset, &oldset) < 0)
		tst_brk(TBROK | TERRNO, "sigprocmask() failed");
}

static void do_test(unsigned int i)
{
    tst_res(TPASS, "No test function defined");
}

static void do_child(unsigned int i)
{}

static void cleanup(void)
{}

static struct tst_test test = {
	.forks_child = 1,
	.needs_root = 1,
	.needs_checkpoints = 1,
	.tcnt = ARRAY_SIZE(test_cases),
	.setup = setup,
	.test = do_test,
	.cleanup = cleanup
};
