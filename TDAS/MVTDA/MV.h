#define MaxMem 16384

typedef struct Segme
{
    short int base, size;
} Segme;
typedef Segme Segmentos;
typedef struct MV
{
    char RAM[MaxMem];
    int Regs[16];
    Segmentos TDS[8];
} MV;