#include <numbers.h>

int _getDecString(char *buffer, unsigned long number){
    char *pointer, *pointerbase;
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

int _getHexString(char *buffer, unsigned long hexnumber){
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
    for(; shift >=0; shift--){
        tmpnumber = hexnumber;
        tmpnumber>>=(4*shift);
        tmpnumber&=0xF;

        if(tmpnumber < 10){
            *hexstring++ = '0' + tmpnumber; //Same as decimal number
        } else {
            *hexstring++ = 'A' + tmpnumber-10; //11-15 Are letters
        }
        *hexstring = '\0';
    }
    return size;
}
