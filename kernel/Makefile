driver_dir	:= driver
memory_dir	:= memory
boot_dir	:= boot
trap_dir	:= trap
utility_dir := utility
modules		:= $(driver_dir) $(memory_dir) $(boot_dir) $(trap_dir) $(utility_dir)

.PHONY:	build clean $(modules)

build: $(modules)

$(modules):
	$(MAKE) build --directory=$@

clean:
	for d in $(modules); \
		do \
			$(MAKE) --directory=$$d clean; \
		done
	rm -rf *.o *~

include ../include.mk
