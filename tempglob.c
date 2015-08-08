#include <glob.h>
#include <stdio.h>

int main(void)
{
	int i, temp;
	glob_t temps;
	FILE *fp;
	
	i = glob("/sys/class/*/*/temp", 0, NULL, &temps);
	glob("/sys/class/thermal/*/temp*_input", i?0:GLOB_APPEND, NULL, &temps);
	for(i=0; i < temps.gl_pathc; i++) {
		if ((fp = fopen(temps.gl_pathv[i],"r")) != NULL) {
			if (fscanf(fp, "%d", &temp)) {
				if ((temp>999||temp<-999) && !(temp%100))
					temp /= 100;
				printf("Thermal %d: %d.%d degrees C\n", i,
					temp/10, temp%10);
			}
			fclose(fp);
		}

	}
	globfree(&temps);
	return 0;
}
