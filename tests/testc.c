#include <stdio.h>
#include <stdlib.h>
#include "usng2-c.h"

int main(int argc, char **argv)
{
	char* usng;
	char* dm;
	double lon = -92.0;
	double lat = 44.0;

	if ((usng = usng2fromll84(-93.0, 45.0, 4)) == NULL) {
		printf("Error\n");
		return 1;
	}
	printf("USNG: %s\n", usng);

	if (usng2toll84(usng, &lon, &lat)) {
		printf("Error\n");
		return 1;
	}
	printf("LL84: %lf, %lf\n", lat, lon);
	free(usng);

	
	if ((dm = usng2_ll84tolldm(lon, lat)) == NULL) {
		puts("Error");
		return 1;
	}
	printf("LLDM: %s\n", dm);
	free(dm);

	return 0;
}
