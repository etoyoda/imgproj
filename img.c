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

  return EOF;
}

  int
makeimg(const struct outparams *op, const struct georefimg *img)
{
  return -1; 
}
