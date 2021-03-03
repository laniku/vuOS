
ENTRY(loader)

SECTIONS
{
    . = 4M;

    .text : 
    {
        *(.multiboot_header)
        *(.text)
    }
    .rodata ALIGN (4K) :
    {
        *(.rodata)
    }
     
     
    .bss ALIGN(4K) :
    {
        *(.COMMON)
        *(.bss)
        *(.initial_stack)
    }
    .data ALIGN(4K) :
    {
        *(.data)
    }

    
    end = .;
}
