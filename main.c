#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "imgproj.h"

  const char *
imgspec_parse(struct georefimg *img, const char *spec)
{
  char *buf = malloc(strlen(spec));
  char *opts;
  char *token;  /* for strtok_r(3) */
  char *saveptr;  /* for strtok_r(3) */
  const char *fnam = NULL;
  if (!buf) { return NULL; }
  strcpy(buf, spec);
  opts = buf + 1;
  while (NULL != (token = strtok_r(opts, ",", &saveptr))) {
    opts = NULL;
    printf("token <%s>\n", token);
    fnam = spec + (token - buf);
  }
  return fnam;
}

  struct georefimg *
loadimg(const char *spec, struct georefimg *next)
{
  struct georefimg *img = new_georefimg();
  const char *fnam;
  if (!img) { return NULL; }
  fnam = imgspec_parse(img, spec);
  printf("fnam <%s>\n", fnam);

  errno = ENOMEM;
  return img;
}

  int
outimg(const struct georefimg *img, const char *spec)
{
  return -1;
}

/* usage:
 * $ imgproj -p[options],input.png [-param ...] z/x/y.png ...
 *
 * -pgms,140.7,511,512,494.9,491.9
 *   the input.png is full disc image from a geostationary satellite
 *   located at 140.7 degree east, with the center of field of view at
 *   pixel coordinates (511, 512), north-south scale 494.9 (pixels per semiaxis), and
 *   east-west scale 491.9 (pixels per semiaxis).
 * -pll,lat1,lat2,lon1,lon2 [unimplemented]
 *   equidistant cylindrical projection with latitude range lat1..lat2 and longitude
 *   range lon1..lon2.
 */

  int
main(int argc, const char **argv)
{
  struct georefimg *imgchain = NULL;
  unsigned r = 1;
  int i;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      imgchain = loadimg(argv[i], imgchain);
      if (!imgchain) {
        perror(argv[i]);
	r = 15;
	break;
      }
    } else {
      if (!imgchain) { break; }
      r = outimg(imgchain, argv[i]);
    }
  }
  if (r) {
    fputs("usage: imgproj -p[options,]input.png ... z/x/y.png ...\n", stderr);
  }
  return r;
}
