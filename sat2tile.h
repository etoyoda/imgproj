/* sat2tile.h */

struct georefimg {
  /* members to hold image */
  int   width;
  int   height;
  png_bytep *vector;
  /* members to describe map projection */
  /* pointer to chain to the next image */
  struct georefimg *next;
};
