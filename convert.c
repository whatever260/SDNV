#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "convert.h"

#define DATA_PROCESS_SIZE 6

unsigned char ANDmask = 0x7F;
unsigned char ORmask = 0x80;

// assumes that the pointer will point to the start of the array generated in sdnv_conversion
unsigned char * fix_array(unsigned char * i, size_t i_len) {
	int c = 0;
	int d = i_len;

	//we need to find the size of the array that is not dead area it is assumed that
	//anything not in use will be the value of zero
	 
	for (c;;c++) {
		if (i[c] != 0)
			break;
	}
	//probably a better way of doing this
	for (d;;d--){
		if (i[d] != 0)
		  d++;
		  break;
	}
	
	// c now equals the place where data begins d now equals the place where the data ends
	// d - c = sizeof data

	unsigned char *out = malloc((d-c) * sizeof(unsigned char));
	memcpy(out, &i[c], (d-c));
	return out;
}

unsigned char * SDNV_conversion(unsigned char *data, unsigned int data_len) {
	unsigned char working_byte = 0;
	unsigned long long stack = 0;
	char bobbin = (data_len-1); // -1 to account for zero ordering of data
	char bytes = DATA_PROCESS_SIZE; // bytes is six because when including zero a loop using it will run seven times which is the ammount of
	//bytes that we are processing in one go processing 7 bytes creates a one byte remainder which neatly fits into an 8 byte temp value
	 
	size_t output_size = data_len + ((data_len/7) + 1);
	unsigned char *out = calloc(output_size, sizeof(unsigned char));
	
	unsigned char saved_bytes = 0;
	char out_bobbin = output_size-1;
	unsigned char *shrunk_arr;

	while (bobbin >= 0)
	{
		// if bobbin is less than seven then we need to adjust the value of bytes so that when bobbin - bytes the
		// result will equal zero as that will be the most significant byte of the data
		if (bobbin < 7) {
			char abval = abs(bobbin - bytes);
			bytes -= abval;
			saved_bytes = bytes;
		}

		//use bytes as an egg timer to thread data onto the fake stack
		//this will then be used to bit shift the data adding bits as necessary

		for (bytes; bytes >= 0 ; bytes--) {
			stack |= data[bobbin - bytes];
			if (bytes > 0)
				stack <<= 8;
		}
		
		// by this point stack will contain 7 bytes of data in the same endianess as data
		// run it through the conversion algorithm below bytes can be reused as an egg timer

		bytes = (saved_bytes > 0) ? saved_bytes: DATA_PROCESS_SIZE;
		do {
		  working_byte = (stack & ANDmask) |((bobbin != (data_len - 1)) ? ORmask: 0);
			stack >>= 7;
			*(out + out_bobbin--) = working_byte;
			bobbin--;
		} while (--bytes >= 0);

		// the first seven bytes are guarenteed but the conversion process likely caused extra bits to be shifted into the
		// Most significant byte of stack

		if (stack > 0){
			working_byte = (stack & ANDmask) | ORmask;
			*(out + out_bobbin--) = working_byte;
			stack = 0;
		}
		//reset bytes in preperation for another loop this will still be set on the final iteration which is technically useless but
		//i dont care/its not worth wrapping it in an if statement computationaly speaking
		bytes = 6;
	}
	shrunk_arr = fix_array(out, output_size);
	free(out);
	return shrunk_arr;
}

char * SDNV_deconversion(unsigned char * pdata) {
	unsigned long long stack = 0;
	unsigned char working_byte = 0;
	unsigned char saved_bytes = 0;
	unsigned char *out;
	unsigned char out_counter = 0;
	char bytes = 7;
  char count = 0;
	long long rStack = 0;

	//get len data ends at the first octet that does not have the MSB 1

	char *pcounter = pdata;
	while(1) {
		if ((*pcounter & 0x80) == 128) {
			count++;
			pcounter++;
		} else {
			break;
		}
	}
	// at this point pdata[count] will get the final element in the array
	
	out = calloc(count, sizeof(unsigned char));
	out_counter = count;
	
	while (count >= 0) {
		
		if (count < 7) {
			char abval = abs(count - bytes);
			bytes -= abval;
			saved_bytes = bytes;
		}

		//could possibly replace this with a memcpy
		for (bytes; bytes >= 0; bytes--) {
			stack |= pdata[count - bytes];
			if (bytes > 0)
				stack <<= 8;
		}

		unsigned char right_mask = 0x7F;
		unsigned short left_mask = 0x100;
		
		bytes = (saved_bytes > 0) ? saved_bytes: 7;
		do {
			working_byte = stack & right_mask;
			working_byte |= (stack & left_mask) >> 1;
			
			right_mask >>= 1;
			left_mask >>= 1;
			left_mask |= left_mask << 1;
		  stack >>= 9;

		  *(out + out_counter) = working_byte;
			
		} while (bytes-- >= 0);
		bytes = 7;
		count -= bytes;
	}
}
