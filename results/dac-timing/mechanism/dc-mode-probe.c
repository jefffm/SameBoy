#define GB_INTERNAL
#include <Core/gb.h>
#include <stdio.h>
#include <stdlib.h>
static uint32_t pixels[160*144];
static uint32_t rgb(GB_gameboy_t *gb,uint8_t r,uint8_t g,uint8_t b){(void)gb;return (uint32_t)r<<16 | (uint32_t)g<<8 | b;}
static unsigned callbacks; static GB_sample_t last;
static void sample_cb(GB_gameboy_t *gb, GB_sample_t *sample) {(void)gb;callbacks++;last=*sample;}
static void next(GB_gameboy_t *gb) {unsigned start=callbacks;while(callbacks==start){GB_advance_cycles(gb,4);GB_apu_run(gb,true);}}
int main(void) {
 GB_gameboy_t *gb=GB_alloc();GB_init(gb,GB_MODEL_DMG_B);GB_set_pixels_output(gb,pixels);GB_set_rgb_encode_callback(gb,rgb);GB_write_memory(gb,0xff40,0);GB_set_sample_rate(gb,48000);
 GB_set_highpass_filter_mode(gb,GB_HIGHPASS_REMOVE_DC_OFFSET);GB_set_interference_volume(gb,0);GB_apu_set_sample_callback(gb,sample_cb);
 GB_apu_write(gb,GB_IO_NR52,0);GB_apu_write(gb,GB_IO_NR52,0x80);GB_apu_write(gb,GB_IO_NR50,0x77);GB_apu_write(gb,GB_IO_NR51,0x99);
 GB_apu_write(gb,GB_IO_NR22,0x08);GB_apu_write(gb,GB_IO_NR12,0x08);
 for(unsigned i=0;i<GB_BAND_LIMITED_WIDTH;i++) next(gb);
 double prev;
 do {prev=gb->apu_output.highpass_diff.left;next(gb);}while(prev!=gb->apu_output.highpass_diff.left);
 printf("before,%d,%a\n",last.left,gb->apu_output.highpass_diff.left);
 GB_apu_write(gb,GB_IO_NR12,0);
 for(unsigned i=0;i<GB_BAND_LIMITED_WIDTH;i++) {next(gb);printf("%u,%d,%a\n",i,last.left,gb->apu_output.highpass_diff.left);}
 GB_dealloc(gb);
}
