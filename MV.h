#ifndef MV_H
#define MV_H
#define MaxMem 16384
#define CS 0
#define DS 1
#define ES 2
#define SS 3
#define KS 4
#define IP 5
#define SP 6
#define BP 7
#define CC 8
#define AC 9
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15

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
    struct
    {
        char ident[6];
        char v;
        short int sizeCS, sizeDS, sizeES, sizeSS, sizeKS, offsetEP;

    } header;
    short int tamMem;
    char enter;
    char *imagen;
    unsigned char RAM[MaxMem];
    int Regs[16];
    Segmentos TDS[5];
    TError VecError[7]; 
} MV;
#endif
