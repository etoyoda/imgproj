/* sat2tile.h */

struct georefimg {
  /* members to hold image */
  int   img_width;
  int   img_height;
  /* actual type (png_bytep *) */
  void **img_vector;
  /* members to describe map projection */
  /* pointer to chain to the next image */
  struct georefimg *img_next;
};

extern struct georefimg *new_georefimg(void);
