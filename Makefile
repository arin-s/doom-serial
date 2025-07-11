ifneq ($(DOOMBUDS),1)
	$(error Do not use this makefile manually, instead use the cmake files inside the examples folder)
endif

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#includes
subdir-ccflags-y += -Iapps/doom/src/DOOM \
					-Iapps/doom/src/common \
					-Iapps/doom/thirdparty/JPEGENC/src \
					-Iapps/doom/examples/SerialBuds
					
C_SRC := $(shell find $(cur_dir)src/ -name '*.c')
C_SRC += $(shell find $(cur_dir)examples/SerialBuds -name '*.c')

CPP_SRC := $(shell find $(cur_dir)src/ -name '*.cpp')
CPP_SRC += $(shell find $(cur_dir)examples/SerialBuds -name '*.cpp')

obj-y += $(C_SRC:.c=.o)
obj-y += $(CPP_SRC:.cpp=.o)

#jpegenc
obj-y += thirdparty/JPEGENC/src/JPEGENC.o

#feature flags
subdir-ccflags-y += \
    -DDOOM_IMPLEMENT_EXIT \
	-D__LINUX__ \
	-DDOOMBUDS
	#-DDOOM_IMPLEMENT_MALLOC
	#-DDOOM_IMPLEMENT_PRINT
	#-DDOOM_IMPLEMENT_FILE_IO
	#-DDOOM_IMPLEMENT_GETTIME
    #-DDOOM_IMPLEMENT_GETENV # remove later
	# -DDOOM_IMPLEMENT_SOCKETS

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