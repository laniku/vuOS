#include "ramdisk.h"

extern void* heap;

FNode MakeFile(char* name, int offset, int size) {
    FNode file;

    char passednull = 0;
    
    for (int i = 0; i < 16; i++) {
        if (! passednull)
            file.Name[i] = name[i];
        else
            file.Name[i] = '\0';

        if (name[i] == '\0')
            passednull = 1;
    }

    file.Offset = offset;

    return file;
}

DNode MakeFS() {
    DNode root;

    root.Name[0] = 'r';
    root.Name[1] = 'o';
    root.Name[2] = 'o';
    root.Name[3] = 't';

    for (int i = 4; i < 16; i++) {
        root.Name[i] = '\0';
    }

    return root;
}

