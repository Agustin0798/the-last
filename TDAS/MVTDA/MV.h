#define MaxMem 16384

typedef struct Segmentos
{
    short int base, tam;
} Segmentos;

typedef struct
{
    int valor;
    char *descripcion;
} TError;

typedef struct MV
{
    char RAM[MaxMem];
    int Regs[16];
    Segmentos TDS[8];
    TError VecError[3];
} MV;