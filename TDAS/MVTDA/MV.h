
typedef struct Segmentos
{
    short base, size;
} Segmentos;
typedef struct MV
{
    char memoria[16000];
    int registros[16];
    Segmentos tablaSegmentos[8];
} MV;