
typedef struct Segmentos
{
    short int base, size;
} Segmentos;
typedef struct MV
{
    char memoria[16384];
    int registros[16];
    Segmentos tablaSegmentos[8];
} MV;