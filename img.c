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
  memset(r, '\0', sizeof *r);
  r->img_next = NULL;
  return r;
}
