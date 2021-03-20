#include <multiboot.h>
#include <stdint.h>
#include <kernel/video.h>
#ifdef DEBUG
	#include <qemu.h>
#endif
//#include <image.h>

char *VIDEO_MEM = (char *) 0xc00b8000;
char *VIDEO_PTR = (char *) 0xc00b8020;

/*void test_image(){
	unsigned int *px_addr = FRAMEBUFFER_MEM;
    int k=0;
    for (int i=0; i<72; i++){
        unsigned int row = i * FRAMEBUFFER_PITCH;
        for (int j=0; j<88; j++){
            //HEADER_PIXEL(header_data, px_addr);
            px_addr[j*4] = MagickImage[i+j + k];
            px_addr[j*4+1] = MagickImage[i+j+1 + k];
            px_addr[j*4+2] = MagickImage[i+j+2 + k];
            k +=3;
            //px_addr = FRAMEBUFFER_MEM + row + j * FRAMEBUFFER_BPP; 
        }
        px_addr +=3200;
    }
}*/


void _printCh(char c, character_color color){
    *VIDEO_PTR++ = c;
    *VIDEO_PTR++ = color;
}

void _printStr(char *string){
    while (*string != '\0'){
        if (*string == '\n'){
            _printNewLine();
        } else {
            _printCh(*string, WHITE);
        }
        #ifdef DEBUG
			qemu_write_char(*string);
		#endif
        string++;
    }
}

void _printNumber(char *buffer, unsigned int number, int base){
    char *pointer, *pointerbase;
    int mod;
    pointer = buffer;
    pointerbase = buffer;
    while (number > 0) {
        mod = number % base;
        *pointer++ = mod + '0';
        number = number / base;
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

    _printStr(buffer);

}

void _printHex(char *buffer, unsigned int hexnumber){
    /* First count how many digit is*/
    buffer = _getHexString(buffer, hexnumber);
    _printStr(buffer);

}

void _printNewLine(){
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (_SCR_W * 2)) + 1) * (_SCR_W * 2));
    #ifdef DEBUG
			qemu_write_char('\n');
	#endif
}

char *_getHexString(char *buffer, unsigned int hexnumber){
    unsigned int tmpnumber = hexnumber;
    int shift = 0;
    char *hexstring = buffer;
    while (tmpnumber >= 16){
        tmpnumber >>= 4;
        shift++;
    }
    
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
    return buffer;
}
