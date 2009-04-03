
CFLAGS+= -Wall -I../../../../include -D_LARGEFILE64_SOURCE
LDLIBS+= -L../../../../lib -lltp

TARGETS=growfiles

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(TARGETS)

growfiles: growfiles.o
		 $(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS)

install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i ../../../bin/$$i ; done ;
	ln -f growfiles_list.sh ../../../bin/

clean:
	rm -f $(OBJS) $(TARGETS)

