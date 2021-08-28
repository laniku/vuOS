#include "io.h"
#include "multiboot.h" // https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot_002eh.html
#include "kernel.h"
#include "ramdisk.h"

char* fb = (char*) 0xB8000;
DNode* root;
char* filedata;

multiboot_module_t* modules;

void DrawChar(int x, int y, char c, int color) {
    int start = 2 * x + 160 * y;
    fb[start] = c;
    fb[start + 1] = color;
}

void DrawCharRaw(int start, char c, int color) {
    fb[start] = c;
    fb[start + 1] = color;
}

segment Make_Segment(unsigned int address, unsigned int size, unsigned char access) {
    segment seg;

    seg.limit = size;
    seg.base0_15 = (address >> 8 * 2) & 0xFFFF;
    seg.base16_23 = (address >> 8) & 0xFF;
    seg.base24_31 = address & 0xFF;
    seg.access = access;
    seg.flags = 0xCF;

    return seg;
}

unsigned short CursorX = 0;
unsigned short CursorY = 0;

// If len is negative write until null terminator
void Write(char* buf, int len, int color) {
    

    if (len < 0) {
        int i = 0;

        while (buf[i] != '\0') {
            if (buf[i] == '\n') {
                CursorX = 0;
                CursorY++;
                i++;
                continue;
            }

            DrawChar(CursorX, CursorY, buf[i], color);
            CursorX++;
            i++;
        }

        return;
    }

    for (int i = 0; i < len; i++) {
        if (buf[i] == '\n') {
            CursorX = 0;
            CursorY++;
            i++;
            continue;
        }

        DrawChar(CursorX, CursorY, buf[i], color);
        CursorX++;
    }
}

void Clear() {
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 25; y++) {
            DrawChar(x, y, ' ', 0x0);
        }
    }
}

void ClearColor(unsigned short color) {
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 25; y++) {
            DrawChar(x, y, ' ', color);
        }
    }
}

#define KB_PORT 0x60
// Mapping keyboard scancodes to ascii characters
char scans[84];

// Definition of special keys
#define ESC 0xF1
#define BACKSPACE 0xF2
#define TAB 0xF3
#define ENTER 0x1C
#define CTRL 0xF5
#define LSHIFT 0xF6
#define RSHIFT 0xF7
#define PRNTSCN 0xF8
#define ALT 0xF9
#define CAPS 0xFA

// Load keymap
void loadkeymap() {
    int count = 0;

    scans[0] = ESC;

    for (int i = 1; i <= 11; i++) {
        scans[i] = '0' + i - 1;

        if (i == 11)
            scans[i] = '0';
    }

    scans[12] = '-';
    scans[13] = '=';

    scans[14] = BACKSPACE;

    scans[15] = TAB;

    scans[16] = 'q';
    scans[17] = 'w';
    scans[18] = 'e';
    scans[19] = 'r';
    scans[20] = 't';
    scans[21] = 'y';
    scans[22] = 'u';
    scans[23] = 'i';
    scans[24] = 'o';
    scans[25] = 'p';
    scans[26] = '[';
    scans[27] = ']';

    scans[28] = ENTER;
    scans[29] = CTRL;

    scans[30] = 'a';
    scans[31] = 's';
    scans[32] = 'd';
    scans[33] = 'f';
    scans[34] = 'g';
    scans[35] = 'h';
    scans[36] = 'j';
    scans[37] = 'k';
    scans[38] = 'l';
    scans[39] = ';';
    scans[40] = '\'';
    scans[41] = '`';

    scans[42] = LSHIFT;

    scans[43] = '\\';

    scans[44] = 'z';
    scans[45] = 'x';
    scans[46] = 'c';
    scans[47] = 'v';
    scans[48] = 'b';
    scans[49] = 'n';
    scans[50] = 'm';
    scans[51] = ',';
    scans[52] = '.';
    scans[53] = '/';

    scans[54] = RSHIFT;

    scans[55] = PRNTSCN;
    scans[56] = ALT;

    scans[57] = ' ';
    scans[58] = CAPS;
}

char keybuffer[500];
short size = 0;

extern void* ramdisk;

char ReadKey() {
    while (size == 0);

    char c = keybuffer[size - 1];
    size--;

    Write(&c, 1, 0x000F);

    return c;
}

char* ReadLine() {
    char* line = "";

    int cur = 0;

    char c = ReadKey();

    while (c != '\n') {
        line[cur] = c;

        cur++;
        c = ReadKey();
    }

    line[cur] = '\0';
    return line;
}

int Len(char* ch) {
    int len = 0;
    
    while (ch[len] != '\0') {
        len++;
    }
    
    return len;
}

short Cmp(char* one, char* two) {
    short eq = 1;
    
    for (int i = 0; i < Len(two); i++) {
        if (one[i] != two[i]) {
            eq = 0;
        }
    }
    
    return eq;
}

// This function runs with segmentation enabled
void Protected() {
    Clear();

    unsigned char mask = inb (0x21);

    ClearColor(0x0010);
    Write("vuOS kernel (SEATRAY) init 1\n", -1, 0x000F); // 0x000F = White on Black
    
    char first = 1; // Weird character that has to be ignored
    
    while (1) {
        Write("vuos-%", 6, 0x000F);
        if (! first)
            ReadKey();
        first = 0;
        char* ch = ReadLine();
        
        if (Cmp(ch, "about") == 1) {
            Write("vuOS alpha, (c) 2021 Lucas Puntillo\n", -1, 0x000F);
            continue;
        }
        
        if (Cmp(ch, "clear") == 1) {
            ClearColor(0x0010);
            CursorX = 0;
            CursorY = 0;
            continue;
        }
	    
	if (Cmp(ch, "modload") == 1) {
	    Jump((void*) modules[1].mod_start);
            continue;
        }

        if (Cmp(ch, "you") == 1) {
            Write("you mean 7880\n", -1, 0x000F);
            continue;
        }
	    
        if (Cmp(ch, "osinfo") == 1) {
            Write("$$*    $$*             vuOS rolling\n $$ |   $$ |             --------------------------\n $$ |   $$ |             Version: Nightly\n *$$*  $$  |            Rev: 20212808\n  *$$*$$  /            Display: 640x480 @ 16bpp\n   \$$$  *              Arch: x86\n", -1, 0x000F);
            continue;
        }
        
        if (Cmp(ch, "exit") == 1) {
            Clear();
            break;
        }
    }

    // When ran under GRUB(Which has modules I use to load a program) this would jump to it
    Jump((void*) modules[1].mod_start);
}

#pragma region COPY PASTE
void irq0_handler(void) {
    outb(0x20, 0x20); //EOI
}

// Keyboard Input
void irq1_handler(void) {
    char c = inb(0x60);
    
    if (c >= 0 && c <= 58) {
        keybuffer[size] = scans[c];
        size++;
    }

    if (c == ENTER) {
        keybuffer[size] = '\n';
        size++;
    }

	outb(0x20, 0x20); //EOI
}
 
void irq2_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq3_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq4_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq5_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq6_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq7_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq8_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI          
}
 
void irq9_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq10_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq11_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq12_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq13_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}

char* sysargs = (char*) 0x060000;

void syscall_handler(unsigned int eax, unsigned int ecx) {
    if (eax == 1) {
        Clear();
        Write("Program exited...", -1, 0x0004);

        while (1);
    }

    if (eax == 2) {
        Clear();
    }

    if (eax == 3) {
        char* ptr = (char*) ecx;
        Write(ptr, -1, 0x0015);
    }
}

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};


void idt_init(void) {
        struct IDT_entry IDT[256];

        extern void load_idt();
        extern int irq0();
        extern int irq1();
        extern int irq2();
        extern int irq3();
        extern int irq4();
        extern int irq5();
        extern int irq6();
        extern int irq7();
        extern int irq8();
        extern int irq9();
        extern int irq10();
        extern int irq11();
        extern int irq12();
        extern int irq13();
        extern int irq14();
        extern int irq15();
        extern int syscall();
 
	    unsigned long irq0_address;
        unsigned long irq1_address;
        unsigned long irq2_address;
        unsigned long irq3_address;          
        unsigned long irq4_address; 
        unsigned long irq5_address;
        unsigned long irq6_address;
        unsigned long irq7_address;
        unsigned long irq8_address;
        unsigned long irq9_address;          
        unsigned long irq10_address;
        unsigned long irq11_address;
        unsigned long irq12_address;
        unsigned long irq13_address;
        unsigned long irq14_address;          
        unsigned long irq15_address;
        unsigned long syscall_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];
 
        /* remapping the PIC */
	    outb(0x20, 0x11);
        outb(0xA0, 0x11);
        
        
        outb(0x21, 0x20);
        outb(0x21, 0x04);
        outb(0x21, 0x01);
        outb(0x21, 0x0); // NULL
        outb(0xA1, 0x28);
        outb(0xA1, 0x02);
        outb(0xA1, 0x01);
        outb(0xA1, 0x0); // NULL
        
 
	irq0_address = (unsigned long)irq0; 
	IDT[32].offset_lowerbits = irq0_address & 0xffff;
	IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[32].zero = 0;
	IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;
 
	irq1_address = (unsigned long)irq1; 
	IDT[33].offset_lowerbits = irq1_address & 0xffff;
	IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[33].zero = 0;
	IDT[33].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;
 
	irq2_address = (unsigned long)irq2; 
	IDT[34].offset_lowerbits = irq2_address & 0xffff;
	IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[34].zero = 0;
	IDT[34].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[34].offset_higherbits = (irq2_address & 0xffff0000) >> 16;
 
	irq3_address = (unsigned long)irq3; 
	IDT[35].offset_lowerbits = irq3_address & 0xffff;
	IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[35].zero = 0;
	IDT[35].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[35].offset_higherbits = (irq3_address & 0xffff0000) >> 16;
 
	irq4_address = (unsigned long)irq4; 
	IDT[36].offset_lowerbits = irq4_address & 0xffff;
	IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[36].zero = 0;
	IDT[36].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[36].offset_higherbits = (irq4_address & 0xffff0000) >> 16;
 
	irq5_address = (unsigned long)irq5; 
	IDT[37].offset_lowerbits = irq5_address & 0xffff;
	IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[37].zero = 0;
	IDT[37].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[37].offset_higherbits = (irq5_address & 0xffff0000) >> 16;
 
	irq6_address = (unsigned long)irq6; 
	IDT[38].offset_lowerbits = irq6_address & 0xffff;
	IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[38].zero = 0;
	IDT[38].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[38].offset_higherbits = (irq6_address & 0xffff0000) >> 16;
 
	irq7_address = (unsigned long)irq7; 
	IDT[39].offset_lowerbits = irq7_address & 0xffff;
	IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[39].zero = 0;
	IDT[39].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[39].offset_higherbits = (irq7_address & 0xffff0000) >> 16;
 
	irq8_address = (unsigned long)irq8; 
	IDT[40].offset_lowerbits = irq8_address & 0xffff;
	IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[40].zero = 0;
	IDT[40].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[40].offset_higherbits = (irq8_address & 0xffff0000) >> 16;
 
	irq9_address = (unsigned long)irq9; 
	IDT[41].offset_lowerbits = irq9_address & 0xffff;
	IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[41].zero = 0;
	IDT[41].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[41].offset_higherbits = (irq9_address & 0xffff0000) >> 16;
 
	irq10_address = (unsigned long)irq10; 
	IDT[42].offset_lowerbits = irq10_address & 0xffff;
	IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[42].zero = 0;
	IDT[42].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[42].offset_higherbits = (irq10_address & 0xffff0000) >> 16;
 
	irq11_address = (unsigned long)irq11; 
	IDT[43].offset_lowerbits = irq11_address & 0xffff;
	IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[43].zero = 0;
	IDT[43].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[43].offset_higherbits = (irq11_address & 0xffff0000) >> 16;
 
	irq12_address = (unsigned long)irq12; 
	IDT[44].offset_lowerbits = irq12_address & 0xffff;
	IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[44].zero = 0;
	IDT[44].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[44].offset_higherbits = (irq12_address & 0xffff0000) >> 16;
 
	irq13_address = (unsigned long)irq13; 
	IDT[45].offset_lowerbits = irq13_address & 0xffff;
	IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[45].zero = 0;
	IDT[45].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[45].offset_higherbits = (irq13_address & 0xffff0000) >> 16;
 
	irq14_address = (unsigned long)irq14; 
	IDT[46].offset_lowerbits = irq14_address & 0xffff;
	IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[46].zero = 0;
	IDT[46].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[46].offset_higherbits = (irq14_address & 0xffff0000) >> 16;
 
    irq15_address = (unsigned long)irq15; 
	IDT[47].offset_lowerbits = irq15_address & 0xffff;
	IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[47].zero = 0;
	IDT[47].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[47].offset_higherbits = (irq15_address & 0xffff0000) >> 16;

    syscall_address = (unsigned long)syscall; 
	IDT[128].offset_lowerbits = syscall_address & 0xffff;
	IDT[128].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[128].zero = 0;
	IDT[128].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[128].offset_higherbits = (syscall_address & 0xffff0000) >> 16;
 
	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;
 
 
 
	load_idt(idt_ptr);
 
}
#pragma endregion COPY PASTE

segment gdttbl[6];

void Grub_LoadRD(multiboot_module_t* info) {
    root = (DNode*) (info->mod_start + 1);
    filedata = (char*) (info->mod_start + sizeof(DNode) + 1);
}

void LoadRD(char* data) {
    root = (DNode*) (data);
    filedata = (char*) (data + sizeof(DNode) + 1);
}

int Main(unsigned int ebx) {
    loadkeymap();
    
    // Can't use modules if we aren't loaded from grub :(
    //multiboot_info_t* bootinfo = (multiboot_info_t*) ebx;

    //multiboot_module_t* mods = (multiboot_module_t*) bootinfo->mods_addr;

    //Grub_LoadRD(&mods[0]);

    idt_init();
    
    gdt g;
    g.address = (unsigned int) &gdttbl;
    g.size = 8 * 2;
    gdttbl[1] = Make_Segment(0, 0xFFFFFFFF, 0x9A);
    gdttbl[2] = Make_Segment(0, 0xFFFFFFFF, 0x92);

    // modules = mods;

    loadgdt(g);
    
    return 0;
}
