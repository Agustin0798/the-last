
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

void (*funcion[32])(int *a, int *b, MV *mv) = {MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, RND, VACIO, VACIO, VACIO, SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, LDL, LDH, NOT, VACIO, VACIO, VACIO, VACIO, STOP};

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

int Ejecuta(MV *mv, CodOpe codigosOperacion[32]) // hacerlo int para manejo de errores
{
    char Cod, OP1, OP2, inst = 0;
    int Valor1, Valor2, Iptemp, ipAct;

    inst = mv->RAM[mv->Regs[IP]];
    // printf("\nEjecutando...");
    while ((mv->Regs[IP] < (mv->TDS[CS].base + mv->TDS[CS].tam)) && strcmp(codigosOperacion[inst & 0b00011111], "STOP")) // Simplificar codOp no lo permite
    {
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;
        // printf("\n %x %x  %x", Cod, OP2, OP1);
        // Busca Operandos
        if ((Cod == 0x03 || Cod == 0x1A) && OP2 == 0b01) // Si cod=0x03 (swap) o cod=0x1A(not) el op1 no puede ser inme
            return 1;                                    // por superposicion de bits => solo reviso op2
        Iptemp = 0;
        Valor2 = getOperando(mv, OP2, mv->Regs[IP], Iptemp);
        Iptemp = (~OP2) & 0x03;
        Valor1 = getOperando(mv, OP1, mv->Regs[IP], Iptemp);
        printf("\n%s | %d |%d\n", codigosOperacion[Cod], Valor1, Valor2);

        if (Errores(*mv))
            return 1;
        ipAct = mv->Regs[IP];
        mv->Regs[IP] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03); // Incrementar IP  mascaras en los op para quedarme con los ultimos dos bits
        funcion[Cod](&Valor1, &Valor2, mv);
        if (Errores(*mv))
        {
            return 1;
        }
        if ((Cod & 0b10000) == 0b00000 && Cod != 0x06 && (Cod != 0x10)) // dos operandos y distinto de cmp
        {
            setOperando(mv, OP1, Valor1, ipAct, (~OP2) & 0x03);
            if (Cod == 0x03)
                setOperando(mv, OP2, Valor2, ipAct, 0);
            if (Errores(*mv))
                return 1;
        }
        else if (Cod == 0x1A) // operacion not
        {
            setOperando(mv, OP2, Valor2, ipAct, 0);
            if (Errores(*mv))
                return 1;
        }
        inst = mv->RAM[mv->Regs[IP]];
    }
    if (mv->Regs[IP] > mv->TDS[CS].tam)
    {

        mv->VecError[2].valor = 1;
        // error fallo de segmento
        return 1;
    }
    else
        return 0; // encontre stop
}

void muestRegi(char muest[], char *NomReg[][4])
{
    char seg = (muest[0] >> 4) & 0x03;
    char reg = muest[0] & 0x0F;

    printf("%s", NomReg[reg][seg]);
}
void muestMem(char muest[], char *NomReg[][4])
{
    char reg = muest[0] & 0x0F;
    short int offset = (muest[1] << 8) | muest[2];

    printf("[%s", NomReg[reg][0]);
    if (offset != 0)
    {
        if (offset < 0)
            printf("-%d", abs(offset));
        else
            printf("+%d", offset);
    }
    printf("]");
}

void muestInme(char muest[])
{
    short int num = (muest[0] << 8) | muest[1];

    printf("%d", num);
}

void Disassembler(MV mv, CodOpe codigosOperacion[])
{
    int i = mv.TDS[CS].base, j, k;
    char inst, OP1, OP2, Cod, muestra1[3], muestra2[3];
    char *NomReg[16][4] = {
        {"CS", "", "", ""},
        {"DS", "", "", ""},
        {"", "", "", ""},
        {"", "", "", ""},
        {"", "", "", ""},
        {"IP", "", "", ""},
        {"", "", "", ""},
        {"", "", "", ""},
        {"CC", "", "", ""},
        {"AC", "", "", ""},
        {"EAX", "AL", "AH", "AX"},
        {"EBX", "BL", "BH", "BX"},
        {"ECX", "CL", "CH", "CX"},
        {"EDX", "DL", "DH", "DLX"},
        {"EEX", "EL", "EH", "EX"},
        {"EFX", "FL", "FH", "FX"},
    };
    while (i < (mv.TDS[CS].base + mv.TDS[CS].tam))
    {
        inst = mv.RAM[i];
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;
        printf("[%4x] %2x", i, inst); // muestro posicion de memoria e instruccion
        j = i + 1;
        if (OP2 != 0b11)
        {
            k = 0;
            for (; j < i + ((~OP2) & 0x03); j++) // muestro OP2
            {
                printf(" %2x ", mv.RAM[j]);
                muestra2[k] = mv.RAM[j];
                k++;
            }

            if (OP1 != 0b11)
            {
                k = 0;
                for (; j < i + ((~OP2) & 0x03) + ((~OP1) & 0x03); j++) // muestro OP2
                {
                    printf(" %2x ", mv.RAM[j]);
                    muestra1[k] = mv.RAM[j];
                    k++;
                }
            }
        }
        j = ((~OP2) & 0x03) + ((~OP1) & 0x03);
        for (; j < 7; j++)
            printf("  ");
        printf("|");
        printf(" %s ", codigosOperacion[Cod]); // muestro mnemonico
        if (OP1 != 0b11)
        {
            switch (OP1)
            {
            case 0:
                muestMem(muestra1, NomReg);
                break;
            case 1:
                muestInme(muestra1);
                break;
            case 2:
                muestRegi(muestra1, NomReg);
                break;
            }
            printf(",");
            if (OP2 != 0b11)
            {
                switch (OP2)
                {
                case 0:
                    muestMem(muestra1, NomReg);
                    break;
                case 1:
                    muestInme(muestra1);
                    break;
                case 2:
                    muestRegi(muestra1, NomReg);
                    break;
                }
            }
        }
        else if (OP2 != 0b11)
        {
            switch (OP2)
            {
            case 0:
                muestMem(muestra1, NomReg);
                break;
            case 1:
                muestInme(muestra1);
                break;
            case 2:
                muestRegi(muestra1, NomReg);
                break;
            }
        }
        printf("\n");
        i += 1 + ((~OP2) & 0x03) + ((~OP1) & 0x03);
    }
}

int main(int argc, char *argv[])
{
    CodOpe codigosOperacion[32] =
        {"MOV", "ADD", "SUB", "SWAP", "MUL", "DIV", "CMP", "SHL", "SHR", "AND", "OR", "XOR", "RND", "SYS", "JMP", "JZ", "JP", "JN", "JNZ", "JNP", "JNN", "LDL", "LDH", "NOT", "STOP"};
    char aux[3];
    int TamC, i, bandera;
    FILE *arch = NULL;
    MV mv;
    char *encontrado = NULL;
    strcpy(codigosOperacion[31], codigosOperacion[24]);
    for (i = 23; i >= 13; i--)
    {
        strcpy(codigosOperacion[i + 3], codigosOperacion[i]);
    }
    // printf("Iniciando...");
    for (i = 0; i < 4; i++)
    {
        mv.VecError[i].valor = 0;
    }
    mv.VecError[0].descripcion = "Instruccion invalida";
    mv.VecError[1].descripcion = "Divicion por cero";
    mv.VecError[2].descripcion = "Fallo de segmento";
    mv.VecError[3].descripcion = "Direccion logica invalida";

    i = 0;
    while (i < argc && encontrado == NULL)
    {
        encontrado = strstr(argv[i], ".vmx");
        i++;
    }
    if (encontrado != NULL)
        arch = fopen(argv[i - 1], "rb");
    else
        printf("No hay archivo");
    if (arch != NULL)
    {
        // printf("\nAbriendo Archivo...");
        fread(header.ident, 1, 5, arch);
        // printf("\n%s", header.ident);
        fread(&header.verc, 1, 1, arch);
        // printf("\n%x", header.verc);
        fread(aux, 1, 2, arch);
        // printf("\nLeyendo Archivo...");
        TamC = (aux[0] << 8) | aux[1];
        if (strcmp(header.ident, "VMX24") == 0)
            if (header.verc == 0x01)
            {
                // TamC = atoi(aux);
                // printf("\n%d tamC", TamC);
                if (TamC <= MaxMem)
                {
                    mv.TDS[0].base = 0;
                    mv.TDS[0].tam = TamC;
                    mv.TDS[1].base = TamC;
                    mv.TDS[1].tam = MaxMem - TamC;
                    mv.Regs[CS] = mv.Regs[IP] = 0;
                    mv.Regs[DS] = 0x00010000;
                    for (i = 0; i < TamC; i++)
                    {
                        fread(&mv.RAM[i], 1, 1, arch);
                    }
                    Ejecuta(&mv, codigosOperacion);
                    for (i = 0; i < 4; i++)
                        if (mv.VecError[i].valor == 0)
                            printf("");
                        else
                            printf(" \n%s", mv.VecError[i].descripcion);
                    i = 0;
                    bandera = 0;
                    while (i < argc && bandera != 1)
                        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-D") == 0)
                            bandera = 1;
                        else
                            i++;
                    if (bandera == 1)
                        Disassembler(mv, codigosOperacion);
                }
                else
                    printf("\nNo hay memoria suficiente para almacenar el codigo. ");
            }
            else
                printf("\nVersion no valida. ");
        else
            printf("\nIdentificador invalido.");
        return 0;
    }
}
