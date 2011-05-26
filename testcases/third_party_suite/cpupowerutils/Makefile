SUBDIR = cpupowerutils

all:
	$(MAKE) -C $(SUBDIR) CROSS=$$CROSS_COMPILER STATIC=true CPUFRQ_BENCH=true

install:
	-ln -s $(SUBDIR)/bench/cpufreq-bench ../../bin/
	-ln -s $(SUBDIR)/bench/example.cfg ../../bin/cpufreq-bench.conf

clean:
	$(MAKE) -C $(SUBDIR) clean
