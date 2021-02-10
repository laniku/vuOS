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

DNode MakeFS();