#include <kernel/psf.h>

uint8_t get_PSF_version(char *_font_structure){
   PSFv1_Font *_v1_font = (PSFv1_Font*) _font_structure;
   if( _v1_font->magic[0] == MAGIC_V1_0 && _v1_font->magic[1] == MAGIC_V1_1){
       return 1;
   }
   PSF_font *_v2_font = (PSF_font *)_font_structure;
   if(_v2_font->magic == MAGIC_V2){
       return 2;
   }
   return 0;
}
