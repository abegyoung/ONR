#include <stdio.h>
#include <stdlib.h>
float getCPUtemp()
{
	FILE *fp;
	char buffer[6];
	int val;
	fp = fopen("/sys/class/thermal/thermal_zone1/temp","r");
	fgets(buffer, 6, fp);
	fclose(fp);
	val = atoi(buffer);
	return (float)val/1000.0;
}
