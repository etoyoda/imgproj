#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "imgproj.h"

  int
imgspec_parse(struct georefimg *img, const char *spec)
{
  char *buf = malloc(strlen(spec));
  char *opts;
  char *token;  /* for strtok_r(3) */
  char *saveptr;  /* for strtok_r(3) */
  if (!buf) { return EOF; }
  strcpy(buf, spec);
  opts = buf;
  if (opts[0] == '-') { opts++; }
  while (NULL != (token = strtok_r(opts, ",", &saveptr))) {
    opts = NULL;
    printf("token <%s>\n", token);
  }
  free(buf);
  return 0;
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
    if (argv[i][0] == '-') {
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
