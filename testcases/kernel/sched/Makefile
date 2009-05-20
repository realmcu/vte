#SUBDIRS = cfs-scheduler clisrv hyperthreading nptl process_stress pthreads sched_stress tool
#2009.2.6, Spring, hyperthreading is only applicable on x86
SUBDIRS = cfs-scheduler clisrv nptl process_stress pthreads sched_stress tool
all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


