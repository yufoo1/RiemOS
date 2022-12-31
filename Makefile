kernel_dir	:= 	kernel
linker_dir	:= 	linkscript
user_dir	:= 	user
driver_dir	:= 	$(kernel_dir)/driver
memory_dir	:= 	$(kernel_dir)/memory
boot_dir	:=	$(kernel_dir)/boot
trap_dir	:=	$(kernel_dir)/trap
app_dir		:=  $(user_dir)/app
user_lib_dir	:=	$(user_dir)/lib
fs_dir 		:=  $(user_dir)/fs
utility_dir	:=  $(kernel_dir)/utility
linkscript	:= 	$(linker_dir)/Qemu.ld
vmlinux_img	:=	vmlinux.img
fs_img		:=	fs.img

modules := 	$(kernel_dir) $(user_dir)
objects	:=	$(boot_dir)/*.o \
		$(driver_dir)/*.o \
		$(memory_dir)/*.o \
		$(trap_dir)/*.o \
		$(utility_dir)/*.o \
		$(app_dir)/*.x \
		$(fs_dir)/*.x

.PHONY: build clean $(modules) run

all : vmlinux.img fs.img

vmlinux.img: build

build: $(modules)
	$(LD) $(LDFLAGS) -T $(linkscript) -o $(vmlinux_img) $(objects)
	for d in $(modules); \
		do \
			$(MAKE) --directory=$$d clean; \
		done; \

$(modules):
	$(MAKE) build --directory=$@

clean:
	for d in $(modules); \
		do \
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ *.img null.d

run: vmlinux.img fs.img
	$(QEMU) -kernel $(vmlinux_img) $(QEMUOPTS)

USER_APP := forkTest.b \
			ipcTest.b \
			ProcessA.b \
			ProcessB.b

FSIMGFILES := user/fs/motd user/fs/newmotd

fs.img: $(FSIMGFILES)
	dd if=/dev/zero of=./fs.img bs=4096 count=1024 2>/dev/null
	gcc ./tool/fsformat.c -o fsformat -m32
	chmod +x fsformat
	./fsformat ./fs.img $(FSIMGFILES)


include include.mk
