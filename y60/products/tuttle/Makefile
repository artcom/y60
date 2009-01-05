
VARIANT   := DBG
DEFINES   := -DXP_UNIX -DDEBUG -D_GNU_SOURCE -D_PTHREADS -DLINUX -DDEBUG_VARIANT
LIBRARIES := -lclish -llub -ltinyrl -ltinyxml -lpthread -lrt -ljsspidermonkey$(VARIANT) -laslbase$(VARIANT)

PRO_INCLUDES := -I$(PRO)/include
PRO_LIBDIRS  := -L$(PRO)/lib

LIB_INCLUDES := -I/usr/local/include
LIB_LIBDIRS  := -L/usr/local/lib

GCC_VERSION  := $(shell gcc -dumpversion)
GCC_MACHINE  := $(shell gcc -dumpmachine)
GCC_INCLUDES := -I$(shell gcc --print-file-name=include)
GXX_INCLUDES := -I/usr/include/c++/$(GCC_VERSION)/$(GCC_MACHINE) -I/usr/include/c++/$(GCC_VERSION)

INCLUDES = $(PRO_INCLUDES) $(LIB_INCLUDES) $(GXX_INCLUDES) $(GCC_INCLUDES)
LIBDIRS  = $(PRO_LIBDIRS)  $(LIB_LIBDIRS)

CXXFLAGS += -g -Wall $(DEFINES) $(INCLUDES)
LDFLAGS  += -g $(LIBDIRS) $(LIBRARIES)

XMLS = $(wildcard xml/*.xml)

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

src_for_obj = $(patsubst %.o,%.cpp,$(1))
obj_for_src = $(patsubst %.cpp,%.o,$(1))

all: tuttle
.PHONY: all

lint:
	xmllint --loaddtd --noout $(XMLS)
.PHONY: lint

clean:
	rm -f tuttle $(OBJS) *~ *.bak semantic.cache xml/*~ xml/*.bak xml/semantic.cache
	rm Makefile.dep
	echo "# DO NOT DELETE THIS LINE -- make depend depends on it." > Makefile.dep
.PHONY: clean

depend:
	makedepend -f Makefile.dep $(DEFINES) $(INCLUDES) $(SRCS)
.PHONY: depend

# main program
tuttle: $(OBJS)
	# XXX: hardcoded static lib
	$(CC) -o tuttle $(LDFLAGS) $(OBJS) /usr/lib/libsigsegv.a

# object files
$(foreach src,$(SRCS),$(eval $(call obj_for_src,$(src)): $(src))) 

# header dependencies
include Makefile.dep

