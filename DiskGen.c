#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char Name[16];
    int Offset;
    int Size;
} FNode;

typedef struct {
    char Name[16];
    FNode Files[254];
    int FileCount;
} DNode;

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
    file.Size = size;

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

int main(int argc, char** argv) {
  char* data = (char*) malloc(sizeof(char) * 1024 * 1024);
  DNode root = MakeFS();

  FILE* output = fopen("./iso/modules/ram.rd", "w");

  unsigned char count = 1;
  
  int offset = 0;

  fwrite(&count, 1, 1, output);
  // Register files.
  for (int i = 1, o = 0; i < argc; i++, o++) {
    FILE* input = fopen(argv[i + 1], "r"); // Inefficient, have to open and close files twice :(. Doesn't really matter though!

    // Measure file.
    fseek(input, 0, SEEK_END);
    root.Files[o] = MakeFile(argv[i], offset, ftell(input));
    fseek(input, 0, SEEK_SET);

    printf("%i\n", root.Files[o].Size);

    root.FileCount++;
    offset += root.Files[o].Size;
    i++;
    fclose(input);
  }
  
  fwrite(&root, sizeof(DNode), 1, output);
  
  for (int i = 1, o = 0; i < argc; i++, o++) {
    FILE* input = fopen(argv[i + 1], "r");
    char buf[50];

    int read = 1;
    int total = 0;

    while (read) {
        read = fread(buf, 1, 50, input);

        fwrite(buf, 1, read, output);
        total += read;
    }

    printf("Bytes read: %i\n", total);

    fclose(input);
    i++;
  }
  
  fclose(output);

  return 0;
}