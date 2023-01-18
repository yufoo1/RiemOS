INCLUDES	:=	-I../include/ -I../user/include

BIN_TOC		:= ../tool/BinaryToC.c

user_lib_dir := ../user/lib

USER_LIB	:= 	$(user_lib_dir)/uMain.o \
				$(user_lib_dir)/uStart.o \
				$(user_lib_dir)/printf.o \
				$(user_lib_dir)/uLib.o \
				$(user_lib_dir)/file.o \
				$(user_lib_dir)/console.o \
				$(user_lib_dir)/pipe.o \
				$(user_lib_dir)/fd.o \
				$(user_lib_dir)/ipc.o \
				$(user_lib_dir)/fsipc.o \
				$(user_lib_dir)/debugf.o \
				fs.o \
				ide.o \

USER_TARGET := server.x

.PHONY: bintoc build clean

build: bintoc $(USER_LIB) $(USER_TARGET)

%.x: %.b.c
	$(GCC) $(CFLAGS) -c -o $@ $<

%.b.c: %.b bintoc
	./BinaryToC -f $< -o $@

%: Entry.o %.o $(USER_LIB)
	$(LD) -o $@ $(LDFLAGS) -G 0 -static -n -nostdlib -T ../linkscript/User.ld $^
	mv $@ $(MOUNT_DIR)

%.b: $(user_lib_dir)/uStart.o %.o $(USER_LIB)
	$(LD) -o $@ $(LDFLAGS) -G 0 -static -n -nostdlib -T ../linkscript/User.ld $^
	$(OBJDUMP) -D $@ > $@.txt

%.o: %.c
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.S
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	for d in $(modules); \
		do \
			$(MAKE) --directory=$$d clean; \
		done
	rm -rf *.o *~ *.d *.x *.b.txt BinaryToC *.b

bintoc:
	gcc $(BIN_TOC) -o BinaryToC
	chmod +x BinaryToC


include ../include.mk