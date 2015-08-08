#include <dirent.h>
#include <stdio.h>



int main(int argc,char **argv)
{
	DIR *crd;
	struct dirent *cre;
	char crt[2] = "?";
	char **dirs = (char *[]){".", NULL};
	if (argc-1) dirs = argv + 1;

	for( ; *dirs; dirs++) {
		crd = opendir(dirs[0]);
		if (!crd) continue;
		do {
			if ((cre = readdir(crd)) != NULL) {
				if (cre->d_type == DT_BLK) { 
					*crt = 'b';
				} else if (cre->d_type == DT_CHR) {
					*crt = 'c';
				} else if (cre->d_type == DT_DIR) {
					*crt = 'd';
				} else if (cre->d_type == DT_FIFO) {
					*crt = '=';
				} else if (cre->d_type == DT_LNK) {
					*crt = 'l';
				} else if (cre->d_type == DT_REG) {
					*crt = 'f';
				} else if (cre->d_type == DT_SOCK) {
					*crt = 's';
				} else { *crt ='?'; }

				printf("%lld\t%s\t%s\n", cre->d_ino, crt, 
					cre->d_name);
			}
		} while (cre);
		closedir(crd);
	}
	return 0;	
}
