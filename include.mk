CROSS_COMPILE	:= riscv64-unknown-elf-
CFLAGS 	= -Wall -O -fno-omit-frame-pointer -ggdb -g -mcmodel=medany -I.
GCC			  := $(CROSS_COMPILE)gcc
CC            := $(CROSS_COMPILE)gcc
CFLAGS 		  := -Wall -O -fno-omit-frame-pointer -ggdb -g -mcmodel=medany -I.
LD            := $(CROSS_COMPILE)ld
OBJDUMP       := $(CROSS_COMPILE)objdump
LDFLAGS	:= -z max-page-size=4096
QEMU	:= qemu-system-riscv64
OPENSBI	:=./opensbi/fw_dynamic.bin
QEMUOPTS 	= -machine sifive_u -m 1G -nographic -bios $(OPENSBI) -drive file=fs.img,if=sd,format=raw
