#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <png.h>
#include "imgproj.h"

#define EARTH_E2        0.006694380022903415749574948586
#define EARTH_B_A       0.996647189318816362
#define EARTH_A         6371137.0
#define GEO_R           42164000.0
#define DEG(x)          (180.0 * (x) * M_1_PI)
#define RADIAN(x)       (M_PI * (x) / 180.0)

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

  png_byte *
findpixel(const struct georefimg *img, double lat, double lon)
{
  double sinlat = sin(lat);
  /* normal from ellipsoid to axis of the Earth */
  double nn = 1.0 / sqrt(1.0 - EARTH_E2 * sinlat * sinlat);
  double coslat = cos(lat);
  double lam = lon - RADIAN(img->img_lc);
  double x = nn * coslat * cos(lam);
  double y = nn * coslat * sin(lam);
  double z = EARTH_B_A * EARTH_B_A * nn * sinlat;
  double scale = GEO_R / (GEO_R - x * EARTH_A); 
  double i = img->img_cw + y * img->img_sw * scale;
  double j = img->img_ch - z * img->img_sh * scale;
  unsigned ui, uj;
  if (i < 0.0) { goto next; }
  if (i > (img->img_width - 1)) { goto next; }
  if (j < 0.0) { goto next; }
  if (j > (img->img_height - 1)) { goto next; }
  ui = floor(i + 0.5);
  uj = floor(j + 0.5);
  return (png_byte *)(img->img_vector[uj]) + ui * 4;

next:
  if (img->img_next) {
    return findpixel(img, lat, lon);
  } else {
    return NULL;
  }
}


  int
makeimg(const struct outparams *op, const struct georefimg *img)
{
  unsigned owidth = op->xz - op->xa + 1; 
  unsigned oheight = op->yz - op->ya + 1;
  png_byte *obuf;
  png_bytep *ovector;
  png_byte *ipix;
  png_byte *opix;

  printf("z%u x%u..%u y%u..%u f=%s\n",
    op->z, op->xa, op->xz, op->ya, op->yz, op->filename);
  if (op->xz < op->xa) { fputs("reverse x scan", stderr); goto quit; }
  if (op->yz < op->ya) { fputs("reverse y scan", stderr); goto quit; }

  obuf = malloc(sizeof(png_byte [4]) * owidth * oheight);
  if (obuf == NULL) { return EOF; }
  memset(obuf, 0, 4 * owidth * oheight);
  ovector = malloc(sizeof(png_bytep) * oheight);
  if (ovector == NULL) { free(obuf); return EOF; }
  for (unsigned j = op->ya; j <= op->yz; j++) {
    ovector[j] = obuf + owidth * 4;
  }

  for (unsigned j = op->ya; j <= op->yz; j++) {
    /* latitude in radian */
    double lat = asin(tanh(
      (1.0 - ldexp((int)j + 0.5, -7 - (int)op->z)) * M_PI
    ));
    for (unsigned i = op->xa; i <= op->xz; i++) {
      /* longitude in radian */
      double lon = 2 * M_PI * (ldexp((int)i + 0.5, -8 - (int)op->z) - 0.5);
      if (imgproj_debug) {
        printf("# %03u %03u %9.3f %8.3f\n", j, i, DEG(lat), DEG(lon));
      }
      opix = (png_byte *)(ovector[j]) + i * 4;
      ipix = findpixel(img, lat, lon);
      if (ipix) {
        memcpy(opix, ipix, 4);
      }
    }
  }

  return 0; 
quit:
  errno = EINVAL;
  return 0;
}
