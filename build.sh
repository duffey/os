set -x
CC=i686-elf-gcc
CFLAGS='-std=gnu99 -ffreestanding -O2 -Wall -Wextra'

i686-elf-as boot.s -o boot.o
nasm -felf32 gdt.s -o gdt.o
nasm -felf32 interrupt.s -o interrupt.o

$CC -c ./include/nanoprintf/nanoprintf.c -o nanoprintf.o $CFLAGS
$CC -c main.c -o main.o $CFLAGS
$CC -c monitor.c -o monitor.o $CFLAGS
$CC -c common.c -o common.o $CFLAGS
$CC -c isr.c -o isr.o $CFLAGS
$CC -c descriptor_tables.c -o descriptor_tables.o $CFLAGS
$CC -c timer.c -o timer.o $CFLAGS

$CC -T link.ld -o kernel -ffreestanding -O2 -nostdlib boot.o gdt.o interrupt.o nanoprintf.o monitor.o common.o isr.o descriptor_tables.o timer.o main.o -lgcc

if grub-file --is-x86-multiboot kernel; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi

mkdir -p isodir/boot/grub
cp kernel isodir/boot/kernel
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o kernel.iso isodir
