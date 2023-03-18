set -x
CC=i686-elf-gcc
CFLAGS='-std=gnu99 -ffreestanding -O2 -Wall -Wextra'
nasm -felf32 boot.s -o boot.o
$CC -c ./include/nanoprintf/nanoprintf.c -o nanoprintf.o $CFLAGS
$CC -c main.c -o main.o $CFLAGS
$CC -c monitor.c -o monitor.o $CFLAGS
$CC -c common.c -o common.o $CFLAGS
$CC -T link.ld -o kernel -ffreestanding -O2 -nostdlib boot.o nanoprintf.o monitor.o common.o main.o -lgcc
if grub-file --is-x86-multiboot kernel; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi
mkdir -p isodir/boot/grub
cp kernel isodir/boot/kernel
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o kernel.iso isodir
