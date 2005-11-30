
AUTOMAKE_OPTIONS        = dist-zip

man_MANS = paintlib-config.1

EXTRA_DIST = doc/*.html doc/pics/*.gif \
             doc/reference/html/*.* \
             win/TIFFEX/*.* \
             win/libcurl/*.* \
             win/VBPaintX/*.* \
             win/VCPaintX/*.* win/VCPaintX/res/*.* \
             win/libpng/*.* \
             win/libungif/*.* \
             win/piclook/*.*  win/piclook/res/*.* \
             win/testdec/*.* win/testdec/res/*.* \
             win/PaintX/*.* win/PaintX/ExcelTest/*.* \
             win/libjpeg/*.* \
             win/libtiff/*.* \
             win/paintlib/*.* \
             win/test/*.* \
             win/testdib/*.* win/testdib/res/*.* win/testdib/pics/*.* \
             common/*.vc common/*.cpp \
             $(man_MANS)

dist-hook:
	rm $(distdir)/common/config.h
	rm $(distdir)/common/plpixeldefs.h

MAINTAINERCLEANFILES =

# Directories to process
SUBDIRS = common lib 

DIST_SUBDIRS = common lib

bin_SCRIPTS = paintlib-config
