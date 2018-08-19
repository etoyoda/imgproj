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
  return -1;
}

  int
makeimg(const struct outparams *op, const struct georefimg *img)
{
  
}
