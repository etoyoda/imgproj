#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h> /* for regex */
#include <regex.h>
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
  saveptr = spec2 = specbuf;
  if (spec2[0] == '-') { spec2++; }
  while (NULL != (token = strtok_r(spec2, ",", &saveptr))) {
    /* spec2, 1st arg to strtok_r() must be NULL after the first call */
    spec2 = NULL;
    if (imgproj_debug) { fprintf(stderr, "token <%s>\n", token); }
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
      case SYM2('o', OF_REDWHITE):
      case SYM2('o', OF_GREENWHITE):
      case SYM2('o', OF_BLUEWHITE):
      case SYM2('o', OF_CYANYELLOW):
      case SYM2('o', OF_UNCHO):
        img->img_of = token[1];
	break;
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

struct re_outspec {
  /* standard z/x/y.png style filename */
  regex_t       zxy;
  /* filename with colon-separated parameters */
  regex_t       colon;
  regex_t       colon2;
};

  struct re_outspec *
outspec_regcomp(void)
{
  static struct re_outspec regs;
  char msg[128];
  int r;
  /* --- pattern 0: z/y/x tile --- */
  r = regcomp(&regs.zxy,
    "([0-9]+)/([0-9]+)/([0-9]+)\\.png$",
    REG_EXTENDED);
  if (r) { goto err; }
  /* --- pattern 1: general filename w/pattern --- */
  r = regcomp(&regs.colon,
    ":z([0-9]+)[:,]?x([0-9]+)-([0-9]+)[:,]?y([0-9]+)-([0-9]+):(.*\\.png)$",
    REG_EXTENDED | REG_NEWLINE);
  if (r) { goto err; }
  /* --- pattern 2: general filename w/pattern --- */
  r = regcomp(&regs.colon2,
    ":z([0-9]+)[:,]?x([0-9]+)[:,]?y([0-9]+):(.*\\.png)$",
    REG_EXTENDED | REG_NEWLINE);
  if (r) { goto err; }
  return &regs;
err:
  regerror(r, NULL, msg, sizeof msg);
  fputs(msg, stderr);
  errno = EINVAL;
  return NULL;
}

  void
regexmsg(int e, regex_t *re)
{
  char buf[256];
  regerror(e, re, buf, sizeof buf);
  fputs(buf, stderr);
}

  int
outspec_parse(struct re_outspec *regs,
  const struct georefimg *img,
  const char *fnam)
{
  const int NMATCH = 10;
  struct outparams op;
  regmatch_t md[NMATCH];
  char msgbuf[256];
  unsigned long u;
  int r;

  /* --- pattern 0: z/x/y tile --- */
  r = regexec(&regs->zxy, fnam, NMATCH, md, 0);
  if (r == REG_NOMATCH) { goto try_colon; }
  if (r) { regexmsg(r, &regs->zxy); goto regerr; }
  errno = 0;
  op.z = strtoul(fnam + md[1].rm_so, NULL, 10);
  u = strtoul(fnam + md[2].rm_so, NULL, 10);
  op.xa = 256u * u;
  op.xz = 256u * u + 255u;
  u = strtoul(fnam + md[3].rm_so, NULL, 10);
  op.ya = 256u * u;
  op.yz = 256u * u + 255u;
  op.filename = fnam;
  r = makeimg(&op, img);
  return r;

try_colon:
  /* --- pattern 1: explicit parameter --- */
  r = regexec(&regs->colon, fnam, NMATCH, md, 0);
  if (r == REG_NOMATCH) { goto try_colon2; }
  if (r) { regexmsg(r, &regs->colon); goto regerr; }
  errno = 0;
  op.z = strtoul(fnam + md[1].rm_so, NULL, 10);
  op.xa = 256u * strtoul(fnam + md[2].rm_so, NULL, 10);
  op.xz = 256u * strtoul(fnam + md[3].rm_so, NULL, 10) + 255u;
  op.ya = 256u * strtoul(fnam + md[4].rm_so, NULL, 10);
  op.yz = 256u * strtoul(fnam + md[5].rm_so, NULL, 10) + 255u;
  op.filename = fnam + md[6].rm_so;
  r = makeimg(&op, img);
  return r;

try_colon2:
  /* --- pattern 2: explicit parameter --- */
  r = regexec(&regs->colon2, fnam, NMATCH, md, 0);
  if (r == REG_NOMATCH) { goto nomatch; }
  if (r) { regexmsg(r, &regs->colon); goto regerr; }
  errno = 0;
  op.z = strtoul(fnam + md[1].rm_so, NULL, 10);
  op.xa = 256u * strtoul(fnam + md[2].rm_so, NULL, 10);
  op.xz = op.xa + 255u;
  op.ya = 256u * strtoul(fnam + md[3].rm_so, NULL, 10);
  op.yz = op.ya + 255u;
  op.filename = fnam + md[4].rm_so;
  r = makeimg(&op, img);
  return r;

regerr:
nomatch:
  errno = EINVAL;
  return 1;
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
  struct re_outspec *regs;
  regs = outspec_regcomp();
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-d")) {
      imgproj_debug = 1;
    } else if (argv[i][0] == '-') {
      imgchain = params_parse(argv[i], imgchain);
      if (!imgchain) { r = EOF; goto ret; }
      waiting_fnam = 1;
    } else if (waiting_fnam) {
      r = loadimg(imgchain, argv[i]);
      if (r != 0) { goto ret; }
      waiting_fnam = 0;
    } else {
      if (!imgchain) { goto ret; }
      r = outspec_parse(regs, imgchain, argv[i]);
      if (r != 0) { goto ret; }
    }
  }
ret:
  if (r) {
    perror(argv[0]);
    fputs("usage: imgproj -params input.png [-params input.png ...] z/x/y.png ...\n",
      stderr);
  }
  return r;
}
