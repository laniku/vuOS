bits 32

global _start

; The bootloader could load this ANYWHERE in memory
; And paging isn't a thing
; So you can't actually use labels here. (On top of the fact that this is turned into a flat binary)

section .text
    _start:
    pop edx ; Heap location

    mov [edx], byte 'H'
    mov [edx+1], byte 'e'
    mov [edx+2], byte 'l'
    mov [edx+3], byte 'l'
    mov [edx+4], byte 'o'
    mov [edx+5], byte ' '
    mov [edx+6], byte 'W'
    mov [edx+7], byte 'o'
    mov [edx+8], byte 'r'
    mov [edx+9], byte 'l'
    mov [edx+10], byte 'd'
    mov [edx+11], byte 0
    

    mov eax, 3
    mov ecx, edx
    int 80h

    mov eax, 1
    int 80h