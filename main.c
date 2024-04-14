#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"
#include "MV.h"
#include "operandos.h"

struct
{
    char *ident;
    char verc;
} header;

void (*funcion[32])(int *a, int *b, MV mv) = {MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, RND, VACIO, VACIO, VACIO, SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, LDL, LDH, NOT, VACIO, VACIO, VACIO, VACIO, STOP};

int Ejecuta(int TamC, MV mv) // hacerlo int para manejo de errores
{
    unsigned char Cod, OP1, OP2, inst = 0;

    int Valor1, Valor2, Iptemp;

    inst = mv.RAM[mv.Regs[IP]];
    printf("Entra en ejecuta");
    while (mv.Regs[IP] < TamC - 1 && inst != 0xFFFF) // Simplificar codOp no lo permite
    {
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x3;
        OP2 = (inst >> 6) & 0x3;                              // Busca Operandos
        mv.Regs[IP] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03); // Incrementar IP  mascaras en los op para quedarme con los ultimos dos bits
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
            Valor2 = getOperando(mv, OP2, Iptemp); // agregar parametro vector errores
            Iptemp = (~OP2) & 0x03;
            Valor1 = getOperando(mv, OP1, Iptemp); // agregar parametro vector errores"  modificar parametros getoperando
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
            Valor2 = getOperando(mv, OP2, Iptemp); // agregar parametro vector errores
            Valor1 = getOperando(mv, OP1, Iptemp); // "
        }

        funcion[Cod](&Valor1, &Valor2, mv);
        printf("%d", Valor1);
        // Llamar funcion
        // Guardar datos
        // Manejo de Errores
        inst = mv.RAM[mv.Regs[IP]];
    }
    if (mv.Regs[IP] >= TamC)
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
    MV mv;
    printf("Iniciando...");
    // inicializar vec de errores;
    if (argc <= 1)
    {
        printf("No hay archivo");
    }
    else
    {

        arch = fopen(argv[1], "rb");
        if (arch != NULL)
        {
            printf("\nAbriendo Archivo...");
            fgets(header.ident, 5, arch);
            printf("\n%s", header.ident);
            header.verc = fgetc(arch);
            if (strcmp(header.ident, "VMX24") == 0)
                if (header.verc == '1')
                {

                    printf("\nLeyendo Archivo...");
                    fgets(aux, 2, arch);
                    TamC = atoi(aux);
                    if (TamC <= MaxMem)
                    {
                        mv.TDS[0].base = 0;
                        mv.TDS[0].tam = TamC;
                        mv.TDS[1].base = TamC;
                        mv.TDS[1].tam = MaxMem - TamC;
                        mv.Regs[CS] = mv.Regs[5] = 0;
                        mv.Regs[DS] = 0x00010000;
                        for (i = 0; i < TamC; i++)
                            mv.RAM[i] = fgetc(arch);
                        Ejecuta(TamC, mv);
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
}
