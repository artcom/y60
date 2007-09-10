#!/bin/sh

# This script extracts the keysyms from a gdkkeysyms.h header and
# generates appropriate javascript property definitions.
#
# Currently, this should be run on windows because our ancient GTK
# doesn't provide all the keys that gtk provides on linux.

if [ -z "$1" ]; then
  echo "Usage: generate-keysyms.sh \$GTK_INCLUDE_DIR/gdk/gdkkeysyms.h"
  exit 1
fi

KEYSYM_HEADER=$1
KEYSYM_LIST=JSGdkKeysyms.txt

PROPERTY_DEFINITIONS=JSGdkKeysymPropertyDefinitions.txt
PROPERTY_NUMBERS=JSGdkKeysymPropertyNumbers.txt


generated() {
  rm -f $1
  echo "// This file was generated using generate-keysyms.sh" > $1
}


generated $KEYSYM_LIST
grep '^#define GDK_' $KEYSYM_HEADER | gawk '{ print $2; }' >> $KEYSYM_LIST

generated $PROPERTY_DEFINITIONS
grep -v '^//' $KEYSYM_LIST | sed -e 's/.*/DEFINE_GDK_PROP(\0),/' >> $PROPERTY_DEFINITIONS

generated $PROPERTY_NUMBERS
grep -v '^//' $KEYSYM_LIST | sed -e 's/.*/PROP_\0,/' >> $PROPERTY_NUMBERS

