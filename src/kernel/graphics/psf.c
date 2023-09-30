#include <psf.h>

#if AUTOMATION == 1
char FONT_VARIABLE(SELECTED_FONT, size);
char FONT_VARIABLE(SELECTED_FONT, start)[];
char FONT_VARIABLE(SELECTED_FONT, end);
#endif

uint8_t psf_font_version;

uint8_t get_PSF_version(char *_font_structure){
   PSFv1_Font *_v1_font = (PSFv1_Font*) _font_structure;
   if( _v1_font->magic[0] == MAGIC_V1_0 && _v1_font->magic[1] == MAGIC_V1_1){
       return PSF_V1;
   }
   PSF_font *_v2_font = (PSF_font *)_font_structure;
   if(_v2_font->magic == MAGIC_V2){
       return PSF_V2;
   }
   return 0;
}

uint8_t* get_glyph(uint8_t symbolnumber, uint8_t version){
    if (version == PSF_V1){
        PSFv1_Font* loaded_font = (PSFv1_Font *) &_binary_fonts_default_psf_start;
        return (uint8_t *) loaded_font + sizeof(PSFv1_Font) + (symbolnumber * loaded_font->charsize);
    } else if (version == PSF_V2) {
        PSF_font* loaded_font = (PSF_font *)&_binary_fonts_default_psf_start;
        return  (uint8_t*) loaded_font + loaded_font->headersize + (symbolnumber * loaded_font->bytesperglyph);
    }
    return 0;
}

uint32_t get_width(uint8_t version){
    if ( version == PSF_V1) {
        return 8;
    }
    return ((PSF_font *) &_binary_fonts_default_psf_start)->width;
}

uint32_t get_height(uint8_t version){
    if ( version == PSF_V1) {
        return ((PSFv1_Font *) &_binary_fonts_default_psf_start)->charsize;
    }
    return ((PSF_font *) &_binary_fonts_default_psf_start)->height;
}
