#include <stdio.h>
#include <errno.h>
#include "imgproj.h"

  struct georefimg *
loadimg(const char *spec, struct georefimg *next)
{
  struct georefimg *r = new_georefimg();
  errno = ENOMEM;
  return r;
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
  for (i = 0; i < argc; i++) {
    if (argv[i][0] == '-') {
      imgchain = loadimg(argv[i], imgchain);
      if (imgchain == NULL) {
        perror(argv[i]);
	r = 15;
	break;
      }
    } else {
      r = outimg(imgchain, argv[i]);
    }
  }
  if (r) {
    fputs("usage: imgproj -p[options,]input.png ... z/x/y.png ...\n", stderr);
  }
  return r;
}