#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"

#define MaxMem 16384

typedef struct
{
    int base,tam;
}TypeTDS;
struct
{
    TypeTDS TDS[5];
    char RAM[MaxMem];
}MV;
struct
{
    char *ident;
    char verc;
}header;
int main(int argc, char *argv[])
{

    char *aux="";
    int TamC;
    FILE *arch;
    arch=fopen(argv[1],"rb");
    if (arch != NULL)
    {
        fgets(header.ident,5,arch);
        header.verc=fgetc(arch);
        if (strcmp(header.ident,"VMX24")==0)
            if (header.verc == '1')
            {
                fgets(aux,2,arch);
                TamC=atoi(aux);
                if (TamC <= MaxMem)
                {
                    MV.TDS[0].base=0; MV.TDS[0].tam=TamC;
                    MV.TDS[1].base=TamC; MV.TDS[1].tam=MaxMem-TamC;
                }
                else
                    printf("No hay memoria suficiente para almacenar el codigo. ");
            }
            else
                printf("Vercion no valida. ");
        else
            printf("Identificador invalido.");
    }
    return 0;
}
