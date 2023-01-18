app_dir := app
lib_dir := lib
modules		:= $(app_dir) $(lib_dir)

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