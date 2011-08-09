SUBDIR = cpupowerutils

all:
	$(MAKE) -C $(SUBDIR) CROSS=$$CROSS_COMPILER STATIC=true CPUFRQ_BENCH=true

install:
	-ln -f $(SUBDIR)/bench/cpufreq-bench ../../bin/
	-ln -f $(SUBDIR)/bench/example.cfg ../../bin/cpufreq-bench.conf

clean:
	$(MAKE) -C $(SUBDIR) clean
