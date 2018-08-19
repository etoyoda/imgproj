#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <png.h>
#include "imgproj.h"

  struct georefimg *
new_georefimg(void)
{
  struct georefimg *r = malloc(sizeof *r);
  if (r == NULL) { return r; };
  /* double-typed fields are initialized by NaN. The rest is rescued later.  */
  memset(r, 0xFF, sizeof *r);
  r->img_width = r->img_height = 0;
  r->img_vector = NULL;
  r->img_next = NULL;
  return r;
}


  int
loadimg(struct georefimg *img, const char *fnam)
{
  FILE *fp;
  png_structp png;
  png_infop info;
  png_byte ucoltype;
  png_byte udepth;
  png_bytep *vector;

  fp = fopen(fnam, "rb");
  /* errno := ENOENT, EPERM etc. */
  if (fp == NULL) { return EOF; }

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) { errno = EPNGFATAL; return EOF; }

  info = png_create_info_struct(png);
  if (!info) { errno = EPNGFATAL; return EOF; }

  if (setjmp(png_jmpbuf(png))) { errno = EPNGFATAL; return EOF; }

  png_init_io(png, fp);
  png_read_info(png, info);

  img->img_width = png_get_image_width(png, info);
  img->img_height = png_get_image_height(png, info);
  ucoltype = png_get_color_type(png, info);
  udepth = png_get_bit_depth(png, info);
  printf("%s: width=%zu height=%zu type=%u depth=%u\n",
    fnam, (size_t)img->img_width, (size_t)img->img_height, ucoltype, udepth);

  /* configuration to read (and convert to) RGBA image */
  if (udepth == 16) {
    png_set_strip_16(png);
  }
  if (ucoltype == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }
  if ((ucoltype == PNG_COLOR_TYPE_GRAY) && (udepth < 8)) {
    png_set_expand_gray_1_2_4_to_8(png);
  }
  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }
  switch (ucoltype) {
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_PALETTE:
      png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
      break;
    default:  break;
  }
  switch (ucoltype) {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      png_set_gray_to_rgb(png);
      break;
    default:  break;
  }
  png_read_update_info(png, info);

  if (png_get_rowbytes(png, info) > 4 * img->img_width) {
    errno = EPNGFATAL;  return EOF;
  }

  img->img_vector = malloc(img->img_height * sizeof(png_bytep));
  vector = (png_bytep *)img->img_vector;
  if (vector == NULL) { return EOF; }
  for (int j = 0; j < img->img_height; j++) {
    vector[j] = (png_byte *)malloc(img->img_width * 4);
    if (vector[j] == NULL) { return EOF; }
  }
  png_read_image(png, vector);
  fclose(fp);

  return 0;
}

  int
makeimg(const struct outparams *op, const struct georefimg *img)
{
  return -1; 
}
