SUBDIR = c-ray-1.1

all: c-ray-f
	$(MAKE) -C $(SUBDIR) CC=$(CROSS_COMPILER)gcc

c-ray-f:
	$(CC) $(CFLAGS) $(SUBDIR)/c-ray-f.c -o c-ray-f -lm

install:
	ln -f $(SUBDIR)/c-ray-mt ../../bin/
	ln -f c-ray-f ../../bin/
	ln -f $(SUBDIR)/sphfract ../../bin/
	ln -f $(SUBDIR)/scene ../../bin/
	ln -f c-ray.sh ../../bin/

clean:
	$(MAKE) -C $(SUBDIR) clean
	rm -f c-ray-f
