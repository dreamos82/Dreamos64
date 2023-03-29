#include <numbers.h>

int _getDecString(char *buffer, long number) {
    int size = 0;
    if(number < 0) {
        *buffer++ = '-';
        size = 1 + _getUnsignedDecString(buffer, ((unsigned long) number * -1));

    } else {
        size = _getUnsignedDecString(buffer, number);
    }
    return size;
}

int _getUnsignedDecString(char *buffer, unsigned long number) {
    char *pointer;
    char *pointerbase;
    int mod;
    int size = 0;
    pointer = buffer;
    pointerbase = buffer;
    if(number == 0) {
        *pointer = '0';
        return 1;
    }
    while (number > 0) {
        mod = number % 10;
        *pointer++ = mod + '0';
        number = number / 10;
        size++;
    }
    *pointer--=0;
    while(pointer > pointerbase){
        char swap;
        swap = *pointer;
        *pointer = *pointerbase;
        *pointerbase = swap;
        pointerbase++;
        pointer--;
    }

    return size;
}

int _getHexString(char *buffer, unsigned long hexnumber, bool use_capital) {
    unsigned long tmpnumber = hexnumber;
    int shift = 0;
    int size = 0;
    char *hexstring = buffer;
    while (tmpnumber >= 16){
        tmpnumber >>= 4;
        shift++;
    }
    size = shift + 1;
    /* Now i can print the digits masking every single digit with 0xF
     * Each hex digit is 4 bytes long. So if i mask number&0xF
     * I obtain exactly the number identified by the digit
     * i.e. number is 0xA3 0XA3&0xF=3
     **/
    char hex_base = 'a';
    if(use_capital == true) {
        hex_base = 'A';
    }
    for(; shift >=0; shift--){
        tmpnumber = hexnumber;
        tmpnumber>>=(4*shift);
        tmpnumber&=0xF;

        if(tmpnumber < 10){
            *hexstring++ = '0' + tmpnumber; //Same as decimal number
        } else {
            *hexstring++ = hex_base + tmpnumber-10; //11-15 Are letters
        }
        *hexstring = '\0';
    }
    return size;
}

// Unsigned
int _getNumericString(char *buffer, unsigned long number, int base, bool use_capital) {
	if (base < 1 || base > 36)
		return 0;
	char *pointer;
	char *pointerbase;
	int mod;
	int size = 0;
	char letter_base = 'a';
	if (use_capital) {
		letter_base = 'A';
	}
	pointer = buffer;
	pointerbase = buffer;

	if(number == 0) {
		*pointer = '0';
		return 1;
	}
	while (number > 0) {
		mod = number % base;
		if (mod < 10) {
			*pointer++ = '0' + mod;
		} else {
			*pointer++ = letter_base + mod - 10;
		}
		number = number / base;
		size++;
	}
	*pointer-- = 0;
	while(pointer > pointerbase){
		char swap;
		swap = *pointer;
		*pointer = *pointerbase;
		*pointerbase = swap;
		pointerbase++;
		pointer--;
	}

	return size;
}