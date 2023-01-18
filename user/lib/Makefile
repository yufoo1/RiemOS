INCLUDES := -I../../include -I../include

target = fsipc.o ipc.o printf.o uLib.o uMain.o uStart.o file.o console.o pipe.o fd.o file.o debugf.o

.PHONY: build clean

%.o: %.S
	$(GCC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(GCC) $(CFLAGS) $(INCLUDES) -c $<

build: $(target)

clean:
	rm -rf *~ *.o *.d

include ../../include.mk