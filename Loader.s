    global loader
    global inb
    global ink
    global outb
    global loadgdt
    global gdtstart
    global loadidt
    global divzero
    global paging_enable
    global paging_disable

    bits 32

	  STACK_SIZE equ 4096
    MODULE_ALIGN equ 0x1

    MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
    FLAGS        equ 0x0            ; multiboot flags
    CHECKSUM     equ -(MAGIC_NUMBER + MODULE_ALIGN)  ; calculate the checksum
    ;VM_BASE     equ 0xC0000000
    ;PDE_INDEX   equ (VM_BASE >> 22)
    ;PSE_BIT     equ 0x00000010
    ;PG_BIT      equ 0x80000000

    section .text
	
    align 4                         ; the code must be 4 byte aligned
        dd MAGIC_NUMBER              
        dd MODULE_ALIGN
        dd CHECKSUM                 ; and the checksum


    times 1024 db 8

    global exestart
    exestart:

    inb:
        mov edx, [esp + 4]
        in al, dx
        ret

    ink:
      in eax, 0x60
      ret

    outb:
        xor edx, edx
        xor al, al
        mov al, [esp + 8]
        mov dx, [esp + 4]
        out dx, al
        ret

    global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
global syscall
 
global load_idt

extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler
extern syscall_handler

;global TEMP_PAGE_DIRECTORY
  ;align 4096
  ;TEMP_PAGE_DIRECTORY:
  ;  dd 0x00000083
  ;  times(PDE_INDEX - 1) dd 0
  ;  dd 0x00000083
  ;  times(4096 - PDE_INDEX - 1) dd 0 

irq0:
  pusha
  call irq0_handler
  popa
  iret
 
irq1:
  pusha
  call irq1_handler
  popa
  iret
 
irq2:
  pusha
  call irq2_handler
  popa
  iret
 
irq3:
  pusha
  call irq3_handler
  popa
  iret
 
irq4:
  pusha
  call irq4_handler
  popa
  iret
 
irq5:
  pusha
  call irq5_handler
  popa
  iret
 
irq6:
  pusha
  call irq6_handler
  popa
  iret
 
irq7:
  pusha
  call irq7_handler
  popa
  iret
 
irq8:
  pusha
  call irq8_handler
  popa
  iret
 
irq9:
  pusha
  call irq9_handler
  popa
  iret
 
irq10:
  pusha
  call irq10_handler
  popa
  iret
 
irq11:
  pusha
  call irq11_handler
  popa
  iret
 
irq12:
  pusha
  call irq12_handler
  popa
  iret
 
irq13:
  pusha
  call irq13_handler
  popa
  iret
 
irq14:
  pusha
  ;call irq14_handler
  popa
  iret
 
irq15:
  pusha
  ;call irq15_handler
  popa
  iret

syscall:
  push ecx
  push eax
  call syscall_handler
  pop eax
  pop ecx
  iret
 
load_idt:
  cli
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

    loadgdt:
        lgdt [esp + 4]
        
        mov eax, cr0
        or eax, 1
        mov cr0, eax

        jmp 0x08:loadss
        loadss:
        extern Protected
        call Protected
        
        .loop:
            hlt
            jmp .loop

    global Jump
    Jump:
      mov edx, heap
      jmp [esp + 4]

    global shrtptr
    shrtptr:
      mov eax, dword [esp + 4]
      ret

    loader:
      extern Main  
      mov esp, stack + STACK_SIZE
      push ebx
      call Main
      
    .loop:
        hlt
        jmp .loop

    global exeend
    exeend:

modules:

global stack

section .bss
    global datastart
    stack:
            resb 4096
    global heap
    heap:
        resb 4000000
    global dataend

    ; RamDisk is last in memory and most likely to run out

    global ramdisk
    ramdisk:


