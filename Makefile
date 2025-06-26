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
apps/doom/shareware.o:
	arm-none-eabi-ld -r -b binary $(cur_dir)doom1.wad -o apps/doom/shareware.tmp.o
	arm-none-eabi-objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents apps/doom/shareware.tmp.o apps/doom/shareware.o
	rm apps/doom/shareware.tmp.o