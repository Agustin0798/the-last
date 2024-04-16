
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"
#include "MV.h"
#include "operandos.h"

struct
{
    char ident[6];
    char verc;
} header;
typedef char CodOpe[5];
void (*funcion[32])(int *a, int *b, MV mv) = {MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, RND, VACIO, VACIO, VACIO, SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, LDL, LDH, NOT, VACIO, VACIO, VACIO, VACIO, STOP};

int Errores(MV mv)
{
    int i = 0, e = 0;

    while (i < 4 && !e)
        if (mv.VecError[i].valor == 1)
            e = 1;
        else
            i++;
    return e;
}

int Ejecuta(int TamC, MV mv, CodOpe codigosOperacion[32]) // hacerlo int para manejo de errores
{
    char Cod, OP1, OP2, inst = 0;
    int Valor1, Valor2, Iptemp;

    inst = mv.RAM[mv.Regs[IP]];
    printf("\nEjecutando...");
    while (mv.Regs[IP] < TamC - 1 && inst != 0xFFFF) // Simplificar codOp no lo permite
    {
        Cod = inst & 0b00011111;

        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;
        printf("\n%x  %x", OP2, OP1);
        // Busca Operandos
        if ((Cod == 0x03 || Cod == 0x1A) && OP2 == 0b01) // Si cod=0x03 (swap) o cod=0x1A(not) el op1 no puede ser inme
            return 1;                                    // por superposicion de bits => solo reviso op2
        Iptemp = 0;
        Valor2 = getOperando(mv, OP2, Iptemp);
        Iptemp = (~OP2) & 0x03;
        Valor1 = getOperando(mv, OP1, Iptemp);
        printf("\n%s | %d  |%d", codigosOperacion[Cod], Valor1, Valor2);

        if (Errores(mv))
            return 1;

        funcion[Cod](&Valor1, &Valor2, mv);
        mv.Regs[IP] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03); // Incrementar IP  mascaras en los op para quedarme con los ultimos dos bits
        if (Errores(mv))
            return 1;
        if ((Cod & 0b10000) == 0b00000 && Cod != 0x06) // dos operandos y distinto de cmp
        {
            setOperando(mv, OP1, Valor1);
            if (Cod == 0x03)
                setOperando(mv, OP2, Valor2);
            if (Errores(mv))
                return 1;
        }
        else if (Cod == 0x1A) // operacion not
        {
            setOperando(mv, OP2, Valor2);
            if (Errores(mv))
                return 1;
        }
        inst = mv.RAM[mv.Regs[IP]];
    }
    if (mv.Regs[IP] >= TamC)
    {
        mv.VecError[2].valor = 1; // error fallo de segmento
        return 1;
    }
    else
        return 0; // encontre stop
}
int main(int argc, char *argv[])
{
    CodOpe codigosOperacion[32] =
        {"MOV",
         "ADD",
         "SUB",
         "SWAP",
         "MUL",
         "DIV",
         "CMP",
         "SHL",
         "SHR",
         "AND",
         "OR",
         "XOR",
         "RND",
         "SYS",
         "JMP",
         "JZ",
         "JP",
         "JN",
         "JNZ",
         "JNP",
         "JNN",
         "LDL",
         "LDH",
         "NOT",
         "STOP"};
    char aux[3];
    int TamC, i;
    FILE *arch;
    MV mv;
    strcpy(codigosOperacion[31], codigosOperacion[24]);
    for (i = 23; i >= 13; i--)
    {
        strcpy(codigosOperacion[i + 3], codigosOperacion[i]);
    }
    printf("Iniciando...");
    for (i = 0; i < 4; i++)
    {
        mv.VecError[i].valor = 0;
    }
    mv.VecError[0].descripcion = "Instruccion invalida";
    mv.VecError[1].descripcion = "Divicion por cero";
    mv.VecError[2].descripcion = "Fallo de segmento";
    mv.VecError[3].descripcion = "Direccion logica invalida";

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
            fread(header.ident, 1, 5, arch);
            printf("\n%s", header.ident);
            fread(&header.verc, 1, 1, arch);
            printf("\n%x", header.verc);
            fread(&aux, 1, 2, arch);
            printf("\nLeyendo Archivo...");
            sscanf(aux, "%x", &TamC);
            if (strcmp(header.ident, "VMX24") == 0)
                if (header.verc == 0x01)
                {
                    // TamC = atoi(aux);
                    printf("\n%d", TamC);
                    if (TamC <= MaxMem)
                    {
                        mv.TDS[0].base = 0;
                        mv.TDS[0].tam = TamC;
                        mv.TDS[1].base = TamC;
                        mv.TDS[1].tam = MaxMem - TamC;
                        mv.Regs[CS] = mv.Regs[5] = 0;
                        mv.Regs[DS] = 0x00010000;
                        for (i = 0; i < TamC; i++)
                        {
                            fread(&mv.RAM[i], 1, 1, arch);
                        }

                        Ejecuta(TamC, mv, codigosOperacion);
                        for (i = 0; i < 4; i++)
                            mv.VecError[i].valor == 0 ? printf("") : printf("%s \n", mv.VecError[i].descripcion);
                    }
                    else
                        printf("\nNo hay memoria suficiente para almacenar el codigo. ");
                }
                else
                    printf("\nVercion no valida. ");
            else
                printf("\nIdentificador invalido.");
        }
        return 0;
    }
}