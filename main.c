#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"

#define MaxMem 16384

typedef struct
{
    short int base, tam;
} TypeTDS;

struct
{
    TypeTDS TDS[5];
    int Regs[16];
    unsigned char RAM[MaxMem];
} MV;
struct
{
    char *ident;
    char verc;
} header;

typedef struct
{
    int valor;
    char *descripcion;
} TError;

TError VecError[];

void (*funcion)[24](int *a, int *b, struct MV) = {};

int Ejecuta(int TamC) // hacerlo int para manejo de errores
{
    unsigned char Cod, OP1, OP2, inst = 0;
    int Valor1, Valor2, Iptemp;

    inst = MV.RAM[MV.Regs[5]];
    while (MV.Regs[5] < TamC - 1 && inst != 0xFFFF)
    {
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x3;
        OP2 = (inst >> 6) & 0x3;                             // Busca Operandos
        MV.Regs[5] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03); // Incrementar IP  mascaras en los op para quedarme con los ultimos dos bits
        if ((Cod >> 4) == 0)
        {
            switch (Cod & 0b01111)
            {
            case 3:
                if (OP1 == 0b01 && OP2 == 0b01)
                    return 1;
                break;
            case 6: // Consulta
                break;
            default: // op1 no inmediato
                if (OP1 == 0b01)
                    return 1;
                break;
            }
            Iptemp = 0;
            Valor2 = getOperando(MV, OP2, Iptemp); // agregar parametro vector errores
            Iptemp = (~OP2) & 0x03;
            Valor1 = getOperando(MV, OP1, Iptemp); // agregar parametro vector errores"  modificar parametros getoperando
            if (Cod == 5 && Valor2 == 0)
            {
                // modifcar vec errores
                return 1;
            }
        }
        else
        {
            switch (Cod & 0b01111)
            {
            case 26:
                if (OP2 == 0b01)
                    return 1;
                break;
            }
            Valor2 = getOperando(MV, OP2); // agregar parametro vector errores
            Valor1 = getOperando(MV, OP1); // "
        }

        funcion[Cod](&Valor1, &Valor2, MV);
        // Llamar funcion
        // Guardar datos
        // Manejo de Errores
        inst = MV.RAM[MV.Regs[5]];
    }
    if (MV.Regs[5] >= TamC)
    {
        // modificar vec errores
        return 1;
    }
    else
        return 0; // encontre stop
}
int main(int argc, char *argv[])
{

    char *aux = "";
    int TamC, i;
    FILE *arch;
    // inicializar vec de errores;
    arch = fopen(argv[1], "rb");
    if (arch != NULL)
    {
        fgets(header.ident, 5, arch);
        header.verc = fgetc(arch);
        if (strcmp(header.ident, "VMX24") == 0)
            if (header.verc == '1')
            {
                fgets(aux, 2, arch);
                TamC = atoi(aux);
                if (TamC <= MaxMem)
                {
                    MV.TDS[0].base = 0;
                    MV.TDS[0].tam = TamC;
                    MV.TDS[1].base = TamC;
                    MV.TDS[1].tam = MaxMem - TamC;
                    MV.Regs[0] = MV.Regs[5] = 0;
                    MV.Regs[1] = 0x00010000;
                    for (i = 0; i < TamC; i++)
                        MV.RAM[i] = fgetc(arch);
                    Ejecuta(TamC);
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
