#ifndef MV_H
#define MV_H
#define MaxMem 16384
#define CS 0
#define DS 1
#define IP 5
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
    short int base, size;
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
#endif