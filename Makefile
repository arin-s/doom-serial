ifneq ($(DOOMBUDS),1)
	$(error Do not use this makefile manually, please check the README for build instructions)
endif

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#includes
subdir-ccflags-y += \
	-Iapps/doom/src/ \
	-Iapps/doom/src/targets/buds \
	-Iapps/doom/thirdparty/JPEGENC/src
C_SRC := $(shell find $(cur_dir)src/ -name '*.c' -not -path "*/targets/headless/*")
CPP_SRC := $(shell find $(cur_dir)src/ -name '*.cpp' -not -path "*/targets/headless/*")

obj-y += $(C_SRC:.c=.o)
obj-y += $(CPP_SRC:.cpp=.o)

#jpegenc
obj-y += thirdparty/JPEGENC/src/JPEGENC.o

#feature flags
subdir-ccflags-y += \
	-D__LINUX__ \
	-DDOOMBUDS

#shareware wad
obj-y += shareware.o
apps/doom/shareware.o: $(cur_dir)doom1.wad
	arm-none-eabi-objcopy \
		--input-target binary \
		--output-target elf32-littlearm \
		--binary-architecture arm \
		--rename-section .data=.rodata,alloc,load,readonly,data,contents \
		--add-symbol=doom_wad_data_start=.rodata:0,global \
		--add-symbol=doom_wad_data_end=.rodata:$(shell stat -c%s $(cur_dir)doom1.wad),global \
		$(cur_dir)doom1.wad \
		apps/doom/shareware.o