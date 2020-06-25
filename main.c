#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include "argparse.h"

#define CONFIG_DEV_NAME	"/dev/mem"
#define MYNAME "memctl"

static const char *const usage[] = {
    MYNAME " options <address> [options args]",
    NULL,
};

const char *desc =
	"\nThis is a program allowing user to read or write register and memory in linux user mode.\n"
	"Note that all number should be hexadecimal.\n"
	"Usage example:\n"
	"	memctl -r 0x400000\n"
	"	memctl -r 0x400000 -l 0x8\n"
	"	memctl -w 0x400000 -d '0x12 0x34 0x56 0xab'";

int main(int argc, char *argv[]) {
	int r = 0;
	int w = 0;
	const char *d = NULL;
	int l = 0;
	struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Options"),
        OPT_INTEGER('r', "read",   &r, "read data"),
        OPT_INTEGER('w', "write",  &w, "write data"),
        OPT_STRING ('d', "data",   &d, "specify the data"),
        OPT_INTEGER('l', "length", &l, "specify the length(in word, a word is 4 bytes)"),
        OPT_END(),
    };
	struct argparse argparse;
	unsigned int addr;
	unsigned int words;
	int fd;
	unsigned int *base;
	unsigned int i;
	char *p;
	unsigned int val;

	/*
	 * get the args
	 */
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse, desc, NULL);
	argc = argparse_parse(&argparse, argc, (const char **)argv);

	/*
	 * arg process
	 */
	if (r && w) {
		printf("%s: wrong parameter\n", MYNAME);
		return -EINVAL;
	}

	if (w && !d) {
		printf("%s: wrong parameter\n", MYNAME);
		return -EINVAL;
	}

	addr = r ? r : w;
	words = l ? l : 1;

	fd = open(CONFIG_DEV_NAME, O_RDWR);;
	if (fd < 0) {
		printf("%s: error in opening file:%s\n", MYNAME, CONFIG_DEV_NAME);
		return -EIO;
	}

	base = (unsigned int *)mmap(NULL, words * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if (!base || (long int)base == -1) {
		printf("%s: error in mmap\n", MYNAME);
		close(fd);
		return -EIO;
	}

	if (r) {
		for (i = 0; i < words; i++)
			printf("0x%08x\n", base[i]);
	} else {
		for (p = (char *)d, i = 0; p; i++) {
			base[i] = val = strtoul(p, &p, 16);
			printf("0x%08x: 0x%08x\n", addr + i * 4, val);
		}
	}

	munmap(base, words);
	close(fd);
	return 0;
}

