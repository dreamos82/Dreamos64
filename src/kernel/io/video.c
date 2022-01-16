#include <multiboot.h>
#include <stdint.h>
#include <kernel/video.h>
#include <base/numbers.h>
#ifdef DEBUG
	#include <qemu.h>
#endif
//#include <image.h>

char *VIDEO_MEM = (char *) _VIDEO_MEM_START;
char *VIDEO_PTR = (char *) _VIDEO_MEM_START;

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

void _printStringAndNumber(char *string, unsigned long number){
    char buffer[30];
    _printStr(string);
    _printHex(buffer, number);
    _printNewLine();
}

void _printNumber(char *buffer, unsigned long number, int base){
    switch(base){
        case 10: 
            break;        
        case 16:
            _getHexString(buffer, number, false);
            break;
    }
    _printStr(buffer);

}

void _printHex(char *buffer, unsigned long hexnumber){
    /* First count how many digit is*/
    _getHexString(buffer, hexnumber, true);
    _printStr(buffer);

}

void _printNewLine(){
    VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (_SCR_W * 2)) + 1) * (_SCR_W * 2));
    #ifdef DEBUG
			qemu_write_char('\n');
	#endif
    _scrollUp();
}

int _getLineNumber(){
    unsigned int absolute_position = VIDEO_PTR - VIDEO_MEM;
    return absolute_position / (_SCR_W*2);
}

void _scrollUp(){
    //This scrolling is very basic, and don't works in all cases, but since this is for the legacy VGA driver
    //and it is needed more for debug purposes, probably it will not be developed much and will be replaced in
    //the future (if i will be still developing the os)
    char *i;

    if (VIDEO_PTR >= VIDEO_MEM + ((_SCR_H) * _SCR_W * 2)) {
        for (i=VIDEO_MEM ; i<= (VIDEO_MEM + ((_SCR_H) * _SCR_W * 2) + (_SCR_W * 2));i++)
          *i = i[_SCR_W * 2];
        VIDEO_PTR = VIDEO_MEM + ((((VIDEO_PTR - VIDEO_MEM) / (_SCR_W * 2)) - 1) * (_SCR_W * 2));
    }
}


