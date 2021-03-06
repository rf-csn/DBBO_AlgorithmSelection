## Makefile to build C example programs included with the IOHprofiler distribution
##
## NOTE: We have tried to make this Makefile as generic and portable
## as possible. However, there are many (incompatible) versions of
## make floating around. We regularly test using GNU make and BSD make
## from FreeBSD. If you have trouble compiling the examples, please
## try to use GNU make. 
##
## On Windows it is best to use either the included NMakefile by running
##
##   nmake -f NMakefile
##
## or installing Cygwin and running GNU make from within Cygwin.

LDFLAGS += -lm
CCFLAGS = -g -ggdb -std=c89 -pedantic -Wall -Wunused-function -Wsign-conversion -Wextra -Wstrict-prototypes -Wshadow -Wno-sign-compare -Wconversion

########################################################################
## Toplevel targets
all: user_experiment

clean:
	rm -f IOHprofiler.o 
	rm -f user_experiment.o user_experiment 

########################################################################
## Programs 
user_experiment: user_experiment.o IOHprofiler.o
	${CC} ${CCFLAGS} -o user_experiment IOHprofiler.o user_experiment.o ${LDFLAGS}  

########################################################################
## Additional dependencies
IOHprofiler.o: IOHprofiler.h IOHprofiler.c
	${CC} -c ${CCFLAGS} -o IOHprofiler.o IOHprofiler.c
user_experiment.o: IOHprofiler.h IOHprofiler.c user_experiment.c
	${CC} -c ${CCFLAGS} -o user_experiment.o user_experiment.c