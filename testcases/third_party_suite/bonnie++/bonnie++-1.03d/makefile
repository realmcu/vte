EXES=bonnie++ zcav

all: $(EXES)

WFLAGS=-Wall -W -Wshadow -Wpointer-arith -Wwrite-strings -pedantic -ffor-scope
CFLAGS=-O2  -DNDEBUG $(WFLAGS)
CXX=$(CROSS_COMPILER)g++ $(CFLAGS)

BONSRC=bon_io.cpp bon_file.cpp bon_time.cpp semaphore.cpp forkit.cpp \
 bon_suid.cpp
BONOBJS=$(BONSRC:.cpp=.o)

ZCAVSRC=bon_suid.cpp
ZCAVOBJS=$(ZCAVSRC:.cpp=.o)

ALLOBJS=$(BONOBJS) $(ZCAVOBJS)

bonnie++: bonnie++.cpp $(BONOBJS)
	$(CXX) bonnie++.cpp -o bonnie++ $(BONOBJS) $(LFLAGS)

zcav: zcav.cpp $(ZCAVOBJS)
	$(CXX) zcav.cpp -o zcav $(ZCAVOBJS) $(LFLAGS)


#modify by Victor 2008.10.31
BIN_DIR=../../..

install:
	ln -f $(EXES) $(BIN_DIR)/bin


%.o: %.cpp %.h bonnie.h port.h
	$(CXX) -c $<

bon_suid.o: bon_suid.cpp bonnie.h port.h
	$(CXX) -c $<

clean:
	rm -f $(EXES) $(ALLOBJS) build-stamp install-stamp
	rm -rf debian/tmp core debian/*.debhelper
	rm -f debian/{substvars,files} config.log

