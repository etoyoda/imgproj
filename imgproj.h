/* sat2tile.h */

enum projtype {
  /* satellite full disc */
  PT_PERSPECTIVE = 'p', 
  /* centered at 90N pole */
  PT_STEREOGRAPHIC = 's',
  /* meteorologists call it latitude-longitude projection */ 
  PT_RECTANGULAR = 'r'
};

struct georefimg {
  /* --- members to hold image --- */
  int   		img_width;
  int   		img_height;
  /* actual type <png_bytep *>, here <void *> is used to conseal png.h dependency. */
  void			**img_vector;
  /* --- members to describe map projection --- */
  enum projtype		img_projtype;
  /* latitude of the first grid point */
  double		img_ba;
  /* latitude of the last grid point */
  double		img_bz;
  /* longitude of the first grid point */
  double		img_la;
  /* longitude of the last grid point */
  double		img_lz;
  /* longitude of center of projection */
  double		img_lc;
  double		img_cw;
  double		img_ch;
  double		img_sw;
  double		img_sh;
  /* --- pointer to chain to the next image --- */
  struct georefimg	*img_next;
};

struct outparams {
   unsigned z;  /* zoom level */
   unsigned xa;  /* global x index of first pixel (= 256 xfirst) */
   unsigned xz;  /* global x index of last pixel (= 256 xlast + 255) */
   unsigned ya;  /* global y index of first pixel (= 256 yfirst) */
   unsigned yz;  /* global y index of last pixel (= 256 ylast + 255) */
   const char *filename;
};

extern struct georefimg *new_georefimg(void);
extern int loadimg(struct georefimg *img, const char *fnam);
extern int makeimg(const struct outparams *op, const struct georefimg *img);

#define EPNGFATAL ENETDOWN
