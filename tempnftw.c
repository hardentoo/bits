#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ftw.h>


struct tempfix {
	char *modname;
	char *unitspec;
	int D_to_dC;
	int A_to_dC;
};

struct tempfix tempfixes[] = {
	{"acerhdf", NULL, 100, 0 },
	NULL
};

int showtemp(const char *fpath, const struct stat *st, int typeflag, struct FTW *ftwbuf)
{
	int dfd, cfile;
	char modpath[4096], *modname, *tempbuf = 0;
	ssize_t bytes;

	if (typeflag != FTW_SL)
		return 0;
	dfd = open(fpath, O_RDONLY);
	if (dfd < 0)
		return 0;
	//open temp, read
	cfile = openat(dfd, "temp", O_RDONLY);
	if (cfile > -1){
		tempbuf = malloc(16);
		if (!tempbuf)
			return -1;
		bytes = read(cfile, tempbuf, 15);
		if (bytes > 0) {
			tempbuf[bytes] = 0;
			puts(tempbuf);
		}
		free(tempbuf);
		close(cfile);
	}
	close(dfd);
	return 0;
	
	//todo: fixup temp
	
}

int main(int argc, char **argv)
{
	nftw("/sys/class", showtemp, 512, FTW_PHYS|FTW_MOUNT);
	return 0;
}

