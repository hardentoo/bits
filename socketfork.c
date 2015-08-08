/* Written by Isaac Dunham in the year of our Lord 2015
 * No rights reserved, all warranties disclaimed.
 * (You can do whatever you want, but entirely at your own risk.)
 *
 * A small forkbomb-ish test program intended to test how many trivial
 * socket-using programs can run on a processor (and their effects on
 * the scheduler).
 * It gets a pair of connected sockets and repeatedly forks, with
 * the main process reading from the socket and the children
 * writing to it.
 * On resource exhaustion, all processes will sleep for 20 seconds
 * and exit, allowing you to briefly observe practical effects
 * before the system recovers.
 *
 * Compile with -DUSLEEP if you would rather see effects of a *lot* of
 * syscalls. (For example, what does the scheduler do?)
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv)
{
	pid_t pid;
	int count, sv[2];
	char got;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
		return 32;

	while ((pid = fork()) > 0) {
		count += (read(sv[1], &got, 1) > 0 )? 1: 0 ;
		printf("pid: %lld forks: %d\n", (long long)pid, count);
	}
	if (!pid) {
		write(sv[0], "a", 1);
	}
#ifdef USLEEP
	count = 0;
	while (count < 20000) {
		usleep(1000);
		count ++;
	}
#else
	sleep(20);
#endif
	return 0;
}
