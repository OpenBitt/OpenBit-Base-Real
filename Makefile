OSNAME 	= SalernOS

GNUEFI 	= ../gnu-efi
OVMFDIR = ../OVMFbin
LDS 	= kernel.ld
CC 		= gcc
ASMC 	= nasm
LD 		= ld

CINCLUDE = -I "include" -I "src"
CFLAGS 	 = -ffreestanding -fshort-wchar -mno-red-zone $(CINCLUDE)
ASMFLAGS = 
LDFLAGS  = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR 	 := src
OBJDIR 	 := lib
BUILDDIR  = bin
BOOTEFI  := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC    = $(call rwildcard,$(SRCDIR),*.c)          
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS   = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJS  += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC)) 
DIRS   = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link

$(OBJDIR)/Interrupts/handlers.o: $(SRCDIR)/Interrupts/handlers.c
	@ echo !==== COMPILING INTERRUPTS UNOPTIMIZED $^
	@ mkdir -p $(@D)
	$(CC) $(CINCLUDE) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@


$(OBJDIR)/Syscall/%.o: $(SRCDIR)/Syscall/%.c
	@ echo !==== COMPILING UNOPTIMIZED $^
	@ mkdir -p $(@D)
	$(CC) $(CINCLUDE)  -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -O3 -c $^ -o $@
	
$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING ASM $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

	
link:
	@ echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img -f 1440 ::
	mmd -i 	   $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i 	   $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mmd -i     $(BUILDDIR)/$(OSNAME).img ::/openbit
	mmd -i 	   $(BUILDDIR)/$(OSNAME).img ::/openbit/bin
	mmd -i 	   $(BUILDDIR)/$(OSNAME).img ::/openbit/assets
	mcopy -i   $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i   $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i   $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::openbit/bin
	mcopy -i   $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernelfont.psf ::openbit/assets

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none
