

#CC = $(CROSS_COMPILER)gcc
#CFLAGS += -O3 
#LDFLAGS += 

SUBDIRS = tools

TARGET = scripts/ipuv3_test.sh 

all:	
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done 
install:
	@set -e; for i in $(SUBDIRS);do  $(MAKE) -C $$i install; done 
	-ln -f $(TARGET) ../../../bin
clean:
	@set -e; for i in $(SUBDIRS);do  $(MAKE) -C $$i clean; done 
	
