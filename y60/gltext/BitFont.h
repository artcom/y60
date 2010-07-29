#ifndef _BitFont_h_
#define _BitFont_h_

#include "y60_gltext_settings.h"

/// Structure to store the data of one Bitmap Character
struct BitChar {
  int width;
  int height;
  float xorig;
  float yorig;
  float advance;
  const unsigned char *bitmap;
} ;

/**
  Structure for storing all the characters of one Bitmap Font.

  There are no functions to enumrate the various fonts, retrieve them by name
  or do other fancy stuff. This would require a constructor to run for each
  font that has been compiled in - a confusing perspective...
 */

struct BitFont {
  const char *name;
  int num_chars;
  int first;
  const BitChar **ch;
  int ascent;
  int descent;
  const char *short_name;
};

Y60_GLTEXT_DECL void
getStringSize(const BitFont & font, const char * text,
              int & x_ret, int & y_ret,
              int & w_ret, int & h_ret);

Y60_GLTEXT_DECL void
getFontSize(int & ascent, int & descent, const BitFont & font);

#endif // _BitFont_h_
