#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#define CLONE_NEWNS 0x00020000

#include <linux/unistd.h>

#ifndef _syscall0
#define _syscall0(type,name) \
type name(void) \
{\
        return syscall(__NR_##name);\
}
#endif

#ifndef _syscall1
#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{\
        return syscall(__NR_##name, arg1);\
}
#endif

#ifndef HAVE_UNSHARE

#if __i386__
#    define __NR_unshare 310
#elif __x86_64__
#    define __NR_unshare 272
#elif __ia64__
#    define __NR_unshare 1296
#elif __s390x__
#    define __NR_unshare 303
#elif __powerpc__
#    define __NR_unshare 282
#else
#    error "Architecture not supported"
#endif

#endif /* HAVE_UNSHARE */

#ifndef CLONE_NEWNET
#define CLONE_NEWNET		0x20000000
#endif

#ifndef unshare
static inline _syscall1 (int,  unshare, unsigned long, flags)
#endif

static const char* procname;

static void usage(const char *name)
{
	printf("usage: %s [-h] [command [arg ...]]\n", name);
	printf("\n");
	printf("  -h		this message\n");
	printf("\n");
	exit(1);
}

int main(int argc, char *argv[])
{	
	int c;
	unsigned long flags = CLONE_NEWNS;

	procname = basename(argv[0]);
       
        printf("child %s\n",argv[1]);

	while ((c = getopt(argc, argv, "+muiUnNphI:")) != EOF) {
		switch (c) {
		case 'h':
		default:
			usage(procname);
		}
	};
    
	argv = &argv[optind];
	argc = argc - optind;	
	
	if (unshare(flags) == -1) {
		perror("unshare");
		return 1;
	} 
	
	if (argc) {
		execve(argv[0], argv, __environ);
		perror("execve");
		return 1;
	}

	return 0;
}

