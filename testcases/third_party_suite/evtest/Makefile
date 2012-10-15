TARGET = evtest
SOURCE_DIR = evtest-1.29 

all: config
	$(MAKE) -C ${SOURCE_DIR} evtest	

config:
	cd $(SOURCE_DIR);./autogen.sh --host=arm-none-linux-gnueabi-

install:
	-ln -f ${SOURCE_DIR}/${TARGET} ../../../testcases/bin/ 

clean:
