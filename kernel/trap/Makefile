INCLUDES := -I../../include

target = kernelVec.o process.o trap.o kclock.o trampoline.o elfLoader.o syscall.o

.PHONY: build clean


build: $(target)

%.o: %.S
	$(GCC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(GCC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -rf *~ *.o *.d

include ../../include.mk