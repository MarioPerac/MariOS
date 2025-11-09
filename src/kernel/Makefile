ARCH-SUFFIX = x86_64-elf-
g++ = $(ARCH-SUFFIX)g++
ld = $(ARCH-SUFFIX)ld
nasm = nasm
qemu =  qemu-system-x86_64
grub-mkrescue = grub-mkrescue

CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti \
           -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
           -I include

LDFLAGS  = -T linker.ld

ASM_SOURCES = arch/x86_64/boot.asm arch/x86_64/long_mode_entry.asm
CPP_SOURCES = kernel.cpp $(wildcard lib/*.cpp) $(wildcard drivers/*.cpp)
OBJS = $(ASM_SOURCES:.asm=.o) $(CPP_SOURCES:.cpp=.o)

TARGET = marios.bin
ISO = marios.iso

all: $(ISO)

%.o: %.asm
	$(nasm) -f elf64 $< -o $@

%.o: %.cpp
	$(g++) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(ld) $(LDFLAGS) -o $@ $(OBJS)

$(ISO): $(TARGET)
	cp $(TARGET) isodir/boot/$(TARGET)
	$(grub-mkrescue) -o $(ISO) isodir

run: $(ISO)
	$(qemu) -cdrom $(ISO)

clean:
	rm -f $(OBJS) $(TARGET) $(ISO)

.PHONY: all run clean