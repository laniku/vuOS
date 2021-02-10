#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH 14
#define FB_LOW 15

typedef struct {
        unsigned short size;
        unsigned int address;
} __attribute__((packed)) gdt;

typedef struct {
    unsigned short limit;
    unsigned short base0_15;
    unsigned char base16_23;
    unsigned char access;
    unsigned char flags;
    unsigned char base24_31;
} segment;

typedef struct {
    unsigned short offset_1;
    unsigned char type;
    unsigned char reserved;
    unsigned short segment;
    unsigned short offset_2;
} __attribute__((packed)) idtentry;

unsigned char inb();
char ink();
void outb(unsigned short port, unsigned char data);
void* gdtstart;
segment gdttbl[];
void loadgdt(gdt table);
void load_idt(void* idt);
segment Make_Segment(unsigned int address, unsigned int size, unsigned char access);;