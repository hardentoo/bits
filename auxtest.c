#include <stdio.h>
#include <elf.h>
#include <sys/auxv.h>
#include <errno.h>

int main()
{
	unsigned long plat, rplat, sec, hw, fn;

	sec = getauxval(AT_SECURE);
	if (!errno) printf("secure: %lu\n", sec);
	errno = 0;
	plat = getauxval(AT_PLATFORM);
	if (!errno) printf("platform: %s\n", (char *)plat);
	errno = 0;
	rplat = getauxval(AT_BASE_PLATFORM);
	if (!errno) printf("real platform: %s\n", (char *)rplat);
	errno = 0;
	hw = getauxval(AT_HWCAP);
	if (hw) printf ("HWCAP: 0x%lX\n", hw);
	hw = getauxval(AT_HWCAP2);
	if (hw) printf ("HWCAP2: 0x%lX\n", hw);
	fn = getauxval(AT_EXECFN);
	if (fn) printf("file: %s\n", (char *)fn);
}
