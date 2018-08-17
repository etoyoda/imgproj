#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "imgproj.h"

#define SYM2(c1,c2) ((((unsigned)(c1) << 8) & 0xFF00u) | ((unsigned)(c2) & 0xFFu))

int imgproj_debug = 0;

  int
imgspec_parse(struct georefimg *img, const char *spec)
{
  char *specbuf = malloc(strlen(spec));
  char *spec2;  /* for strtok_r(3) */
  char *token;  /* for strtok_r(3) */
  char *saveptr;  /* for strtok_r(3) */
  int r = 0;
  if (!specbuf) { return EOF; }
  strcpy(specbuf, spec);
  spec2 = specbuf;
  if (spec2[0] == '-') { spec2++; }
  while (NULL != (token = strtok_r(spec2, ",", &saveptr))) {
    /* spec2, 1st arg to strtok_r() must be NULL after the first call */
    spec2 = NULL;
    if (imgproj_debug) { printf("token <%s>\n", token); }
    switch (SYM2(token[0], token[1])) {
      case SYM2('p', PT_PERSPECTIVE):
      case SYM2('p', PT_STEREOGRAPHIC):
      case SYM2('p', PT_RECTANGULAR):
        img->img_projtype = token[1];
	break;
      case SYM2('b','a'):  img->img_ba = atof(token+2);  break;
      case SYM2('b','z'):  img->img_bz = atof(token+2);  break;
      case SYM2('l','a'):  img->img_la = atof(token+2);  break;
      case SYM2('l','z'):  img->img_lz = atof(token+2);  break;
      case SYM2('l','c'):  img->img_lc = atof(token+2);  break;
      case SYM2('c','h'):  img->img_ch = atof(token+2);  break;
      case SYM2('c','w'):  img->img_cw = atof(token+2);  break;
      case SYM2('s','h'):  img->img_sh = atof(token+2);  break;
      case SYM2('s','w'):  img->img_sw = atof(token+2);  break;
      default:
        fprintf(stderr, "unknown parameter (%s)\n", token);
	r = errno = EINVAL;
	goto ret;
	break;
    }
  }
  switch (img->img_projtype) {
    case PT_PERSPECTIVE:
#define CHECK_FPAR(xx) \
      if (isnan(img->img_ ## xx)) { \
	fputs("parameter -" #xx " missing\n", stderr); \
	r = errno = EINVAL; \
	goto ret; \
      }
      CHECK_FPAR(lc)
      CHECK_FPAR(sw)
      CHECK_FPAR(sh)
      CHECK_FPAR(cw)
      CHECK_FPAR(ch)
      break;
#if 0
    case PT_STEREOGRAPHIC:
      break;
#endif
    case PT_RECTANGULAR:
      CHECK_FPAR(ba)
      CHECK_FPAR(bz)
      CHECK_FPAR(la)
      CHECK_FPAR(lz)
      break;
    default:
      fputs("parameter -p missing\n", stderr);  r = errno = EINVAL;  goto ret;  break;
  }
ret:
  free(specbuf);
  return r;
}

  struct georefimg *
params_parse(const char *spec, struct georefimg *next)
{
  struct georefimg *img = new_georefimg();
  if (!img) { return NULL; }
  if (0 != imgspec_parse(img, spec)) {
    /* we can free memory for img, but that's just waste of time */
    return NULL;
  }
  return img;
}

  int
loadimg(struct georefimg *img, const char *fnam)
{
  return -1;
}

  int
outimg(const struct georefimg *img, const char *fnam)
{
  return -1;
}

/* usage:
 * $ imgproj -p[options],input.png [-param ...] z/x/y.png ...
 *
 * -pp,lc140.7,cw511,ch512,sh494.9,sw491.9
 *   the input.png is full disc image from a geostationary satellite
 *   located at 140.7 degree east, with the center of field of view at
 *   pixel coordinates (511, 512), north-south scale 494.9 (pixels per semiaxis), and
 *   east-west scale 491.9 (pixels per semiaxis).
 * -pr,ba50,bz19,la116,lb150
 *   equidistant cylindrical projection with latitude range 116..150 and longitude
 *   range 50..19.  Note the parameter /ba/ can be greater than /bz/.
 */

  int
main(int argc, const char **argv)
{
  struct georefimg *imgchain = NULL;
  int r = 1;
  int waiting_fnam = 0;
  int i;  /* index */
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-d")) {
      imgproj_debug = 1;
    } else if (argv[i][0] == '-') {
      imgchain = params_parse(argv[i], imgchain);
      if (!imgchain) { r = EOF; break; }
      waiting_fnam = 1;
    } else if (waiting_fnam) {
      r = loadimg(imgchain, argv[i]);
      if (r != 0) { break; }
      waiting_fnam = 0;
    } else {
      if (!imgchain) { break; }
      r = outimg(imgchain, argv[i]);
      if (r != 0) { break; }
    }
  }
  if (r) {
    perror(argv[0]);
    fputs("usage: imgproj -params input.png [-params input.png ...] z/x/y.png ...\n",
      stderr);
  }
  return r;
}
