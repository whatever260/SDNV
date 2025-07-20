#include <stdio.h>
#include <unistd.h>

#include "convert.c"


void main(void) {
	unsigned char in[9] = {23, 1, 23, 34, 56, 98, 12, 119, 84};
	printf("--- Values Before Conversion ---\n");

	for (int i = 0; i < 9; i++) {
		printf("int = %d\n", in[i]);
	}

	printf("--- End ---\n\n");

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
	printf("int = %d -- least significant byte\n\n", out[9]);

	unsigned char *convert = SDNV_deconversion(out);
	
	printf("int = %d -- most significant byte\n", convert[0]);
	printf("int = %d\n", convert[1]);
	printf("int = %d\n", convert[2]);
	printf("int = %d\n", convert[3]);
	printf("int = %d\n", convert[4]);
	printf("int = %d\n", convert[5]);
	printf("int = %d\n", convert[6]);
	printf("int = %d\n", convert[7]);
	printf("int = %d -- least significant byte\n", convert[8]);

	free(out);
	free(convert);

}
