#include <stdio.h>
#include <unistd.h>

#include "convert.c"


void main(void) {
	unsigned char in[9] = {23, 1, 23, 34, 56, 98, 12, 119, 84};

	unsigned char * out = SDNV_conversion(in, 9);

	printf("int = %d -- most significant byte\n", out[0]);
	printf("int = %d\n", out[1]);
	printf("int = %d\n", out[2]);
	printf("int = %d\n", out[3]);
	printf("int = %d\n", out[4]);
	printf("int = %d\n", out[5]);
	printf("int = %d\n", out[6]);
	printf("int = %d\n", out[7]);
	printf("int = %d\n", out[8]);
	printf("int = %d -- least significant byte\n", out[9]);

	unsigned char *convert = SDNV_deconversion(out);

	printf("int = %d\n", convert[9]);

	free(out);

}
