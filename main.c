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

int Errores(MV mv)
{
    int i=0,e=0;

    while (i<4 && !e)
        if (mv.VecError[i].valor == 1)
            e=1;
        else
            i++;
    return e;
}

int Ejecuta(int TamC, MV mv) // hacerlo int para manejo de errores
{
    unsigned char Cod, OP1, OP2, inst = 0;
    int Valor1, Valor2, Iptemp;

    inst = mv.RAM[mv.Regs[IP]];
    while (mv.Regs[IP] < TamC - 1 && inst != 0xFFFF) // Simplificar codOp no lo permite
    {
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x3;
        OP2 = (inst >> 6) & 0x3;                             // Busca Operandos
        if ((Cod == 0x03 || Cod == 0x1A) && OP2 == 0b01)  // Si cod=0x03 (swap) o cod=0x1A(not) el op1 no puede ser inme
            return 1;                                      //por superposicion de bits => solo reviso op2
        Iptemp = 0;
        Valor2 = getOperando(mv, OP2, Iptemp);
        Iptemp = (~OP2) & 0x03;
        Valor1 = getOperando(mv, OP1, Iptemp);
        if (Errores(mv))
            return 1;
        mv.Regs[IP] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03); // Incrementar IP  mascaras en los op para quedarme con los ultimos dos bits
        funcion[Cod](&Valor1, &Valor2, mv); // Llamar funcion
        if (Errores(mv))
            return 1;
        if ((Cod & 0b10000) == 0b00000 && Cod != 0x06)  // dos operandos y distinto de cmp
        {
            setOperando(mv,OP1,Valor1);
            if (Cod == 0x03)
                setOperando(mv,OP2,Valor2);
            if (Errores(mv))
                return 1;
        }
        else
            if (Cod == 0x1A)  // operacion not
            {
                setOperando(mv,OP2,Valor2);
                if (Errores(mv))
                    return 1;
            }
        inst = mv.RAM[mv.Regs[IP]];
    }
    if (mv.Regs[IP] >= TamC)
    {
        mv.VecError[2].valor=1; // error fallo de segmento
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
    for (i=0; i<4; i++)
    {
        mv.VecError[i].valor=0;
    }
    mv.VecError[0].descripcion="Instruccion invalida";
    mv.VecError[1].descripcion="Divicion por cero";
    mv.VecError[2].descripcion="Fallo de segmento";
    mv.VecError[3].descripcion="Direccion logica invalida";

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
            fgets(header.ident, 6, arch);
            printf("\n%s", header.ident);
            header.verc = fgetc(arch);
            printf("\n%x", header.verc);
            if (strcmp(header.ident, "VMX24") == 0)
                if (header.verc == 0x01)
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
                        for (i=0; i<4; i++)
                            printf("%s \n",mv.VecError[i].descripcion);
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
