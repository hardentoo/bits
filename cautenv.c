/* Written by Isaac Dunham 2015 A.D.
 * Dedicated to the public domain where permitted via CC0
 *
 * Cauterize environment:
 * clean all variables, fds, ... in order to guarantee that a program
 * behaves reproducibly.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

void _Noreturn usage(char *progname)
{
	dprintf(2, "usage: %s [-c DIR] [-u] [-x] COMMAND [ARGS]\n"
		"Run COMMAND in a clean environment.\n"
		" -c DIR Change directory to DIR (default /)\n"
		" -u     Save user-specific variables:"
		"USER SHELL HOME EDITOR PAGER\n"
		" -x     Save X11 variables: DISPLAY XAUTH\n"
		"PATH is always set to /bin:/usr/bin "
		"(or /sbin:/bin:/usr/sbin:/usr/bin for root)\n", progname);
	exit(1);
}
void _Noreturn die(char *msg)
{
	perror(msg);
	_exit(5);
}

void _Noreturn die_twice(char *msg)
{
	dprintf(2, "%s can only be specified once\n", msg);
	exit(1);
}

/* Return NULL or a malloc'd copy of an environment variable
 * Rather than rely on manipulating environ directly, we use getenv().
 */
char * dupenv(char *var)
{
	char *newvar, *val;

	val = getenv(var);
	if (!val) return NULL;
	newvar = malloc(strlen(var) + strlen(val) + 2);
	if (!newvar) die("error duplicating environment");
	sprintf(newvar, "%s=%s", var, val);
	return newvar;
}

int main(int argc, char **argv)
{
	int curopt = 0, dirfd, cfd;
	DIR *fds;
	struct dirent *fdent;
	char *options = "?c:ux", *dir = "/";
	char *user = NULL, *shell = NULL, *home = NULL, *editor = NULL,
		*pager = NULL, *display = NULL, *xauth = NULL;

	optind = 1;
	putenv("POSIXLY_CORRECT=1"); /* don't permute arguments */
	while ((curopt = getopt(argc, argv, options)) != -1) {
		switch (curopt) {
			case 'c':
				dir = optarg;
				break;
			case 'u':
				if (user || shell || home || editor || pager)
					die_twice("-u");
				user = dupenv("USER");
				shell = dupenv("SHELL");
				home = dupenv("HOME");
				editor = dupenv("EDITOR");
				pager = dupenv("PAGER");
				break;
			case 'x':
				if (display || xauth) die_twice("-x");
				display = dupenv("DISPLAY");
				xauth = dupenv("XAUTH");
				break;
			case '?':
				usage(argv[0]);
				break;
		}
	}
	if (optind >= argc || errno) usage(argv[0]);

	if (chdir(dir)) {
		dprintf(2, "%s: could not chdir() to %s\n", argv[0], dir);
	}
	dirfd = open("/proc/self/fd/", O_RDONLY|O_DIRECTORY);
	if (dirfd < 0 || (fds = fdopendir(dirfd))==NULL) {
		dprintf(2, "%s: cannot enumerate fds to close\n", argv[0]);
		_exit(2);
	}
	while (fdent = readdir(fds)) {
		cfd = atoi(fdent->d_name);
		if (cfd > 2 && cfd != dirfd && close(cfd))
			die("cannot close fd");
	}
	closedir(fds);

	clearenv();
	if (putenv(getuid() ? "PATH=/bin:/usr/bin" 
			: "PATH=/sbin:/bin:/usr/sbin:/usr/bin"))
		die("error setting PATH");
	if ((user   ? putenv(user)   : 0) ||
	    (shell  ? putenv(shell)  : 0) ||
	    (home   ? putenv(home)   : 0) ||
	    (editor ? putenv(editor) : 0) ||
	    (pager  ? putenv(pager)  : 0) ||
	    (display? putenv(display): 0) ||
	    (xauth  ? putenv(xauth)  : 0))
		die("error setting new environment");

	execvp(argv[optind], argv+optind);
	dprintf(2, "Failed to run %s\n", argv[optind]);
	_exit(4);
}
