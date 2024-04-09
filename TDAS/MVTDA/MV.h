#define MaxMem 16384

typedef struct Segmentos
{
    short int base, size;
} Segmentos;
typedef struct MV
{
    char RAM[MaxMem];
    int Regs[16];
    Segmentos TDS[8];
} MV;