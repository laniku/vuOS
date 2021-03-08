## Build vuOS

cat << "EOF"
██╗   ██╗██╗   ██╗ ██████╗ ███████╗
██║   ██║██║   ██║██╔═══██╗██╔════╝
██║   ██║██║   ██║██║   ██║███████╗
╚██╗ ██╔╝██║   ██║██║   ██║╚════██║
 ╚████╔╝ ╚██████╔╝╚██████╔╝███████║
  ╚═══╝   ╚═════╝  ╚═════╝ ╚══════╝             
EOF
sudo apt install build-essential mtools nasm xorriso grub-pc-bin -y
mkdir -p ./iso/boot/grub/
nasm -f elf32 Loader.s -o Loader.o && gcc -c Kernel.c -m32 -nostdlib -nostdinc -ffixed-ecx -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -W
ld -T Link.ld -melf_i386 Loader.o Kernel.o  -o ./iso/boot/vuos-seatray.elf
cp grub.cfg ./iso/boot/grub
grub-mkrescue --output vuos-i686.iso --compress xz iso/
echo "Success! vuos-i686.iso made."
