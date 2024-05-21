#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"
#include "MV.h"
#include "operandos.h"

typedef char CodOpe[5];

void (*funcion[32])(int *a, int *b, MV *mv) = {MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, RND, VACIO, VACIO, VACIO, SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, LDL, LDH, NOT, PUSH, POP, CALL, RET, STOP};

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

int Ejecuta(MV *mv, CodOpe codigosOperacion[32])
{
    char Cod, OP1, OP2, inst = 0;
    int Valor1, Valor2, Iptemp, ipAct;

    inst = mv->RAM[mv->Regs[IP]];

    while ((mv->Regs[IP] < (mv->TDS[CS].base + mv->TDS[CS].tam)) && strcmp(codigosOperacion[inst & 0b00011111], "STOP"))
    {
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;

        if ((Cod == 0x03 || Cod == 0x1A) && OP2 == 0b01)
            return 1;
        Iptemp = 0;
        Valor2 = getOperando(mv, OP2, mv->Regs[IP], Iptemp);
        Iptemp = (~OP2) & 0x03;

        Valor1 = getOperando(mv, OP1, mv->Regs[IP], Iptemp);

        if (Errores(*mv))
        {
            return 1;
        }
        ipAct = mv->Regs[IP];
        mv->Regs[IP] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03);
        funcion[Cod](&Valor1, &Valor2, mv);
        if (Errores(*mv))
        {
            return 1;
        }
        if ((Cod & 0b10000) == 0b00000 && Cod != 0x06 && (Cod != 0x10))
        {
            setOperando(mv, OP1, Valor1, ipAct, (~OP2) & 0x03);
            if (Cod == 0x03)
                setOperando(mv, OP2, Valor2, ipAct, 0);
            if (Errores(*mv))
                return 1;
        }
        else if (Cod == 0x1A)
        {
            setOperando(mv, OP2, Valor2, ipAct, 0);
            if (Errores(*mv))
                return 1;
        }
        inst = mv->RAM[mv->Regs[IP]];
        if (mv->enter == 1)
        {
            mv->enter = 0;
            Valor1 = 0;
            Valor2 = 0xF;
            funcion[0x10](&Valor1, &Valor2, mv);
        }
    }
    if (mv->Regs[IP] > mv->TDS[CS].tam)
    {

        mv->VecError[2].valor = 1;
        return 1;
    }
    else
        return 0;
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
    char tam = (muest[0] >> 6) & 0x3;
    short int offset = (muest[1] << 8) | muest[2];
    if (tam == 0x3)
        printf("b");
    else if (tam == 0x1)
        printf("w");
    else
        printf("l");
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
    int num = ((muest[0] << 8) | (muest[1] & 0xff));

    printf("%d", num);
}

void Disassembler(MV mv, CodOpe codigosOperacion[])
{
    int IPaux = mv.TDS[CS].base, i, j;
    unsigned char inst, OP1, OP2, Cod;
    char muestra1[3], muestra2[3];
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

    while (IPaux < (mv.TDS[CS].base + mv.TDS[CS].tam))
    {
        inst = mv.RAM[IPaux];
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;

        printf("[%04X] %02X ", IPaux, (unsigned)inst);

        for (i = 0; i < ((~OP2) & 0x03); i++)
        {
            printf("%02X ", mv.RAM[IPaux + 1 + i]);
            muestra2[i] = mv.RAM[IPaux + 1 + i];
        }

        j = 0;
        for (i = ((~OP2) & 0x03); i < (((~OP1) & 0x03) + ((~OP2) & 0x03)); i++)
        {
            printf("%02X ", mv.RAM[IPaux + 1 + i]);
            muestra1[j] = mv.RAM[IPaux + 1 + i];
            j++;
        }

        for (i = (((~OP1) & 0x03) + ((~OP2) & 0x03)); i < 6; i++)
            printf("   ");
        printf("| %s ", codigosOperacion[Cod]);

        if (OP1 != 3)
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
            if (OP2 != 3)
            {
                printf(",");
                switch (OP2)
                {
                case 0:
                    muestMem(muestra2, NomReg);
                    break;
                case 1:
                    muestInme(muestra2);
                    break;
                case 2:
                    muestRegi(muestra2, NomReg);
                    break;
                }
            }
        }
        else
            switch (OP2)
            {
            case 0:
                muestMem(muestra2, NomReg);
                break;
            case 1:
                muestInme(muestra2);
                break;
            case 2:
                muestRegi(muestra2, NomReg);
                break;
            }
        printf("\n");
        IPaux += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03);
    }
}

int main(int argc, char *argv[])
{
    CodOpe codigosOperacion[32] =
        {"MOV", "ADD", "SUB", "SWAP", "MUL", "DIV", "CMP", "SHL", "SHR", "AND", "OR", "XOR", "RND", "", "", "", "SYS", "JMP", "JZ", "JP", "JN", "JNZ", "JNP", "JNN", "LDL", "LDH", "NOT", "PUSH", "POP", "CALL", "RET", "STOP"};
    char aux[3];
    int TamC, i, bandera;
    FILE *arch = NULL;
    MV mv;
    char *encontradoVMX = NULL, *encontradoVMI = NULL;
    strcpy(codigosOperacion[31], codigosOperacion[24]);
    for (i = 23; i >= 13; i--)
    {
        strcpy(codigosOperacion[i + 3], codigosOperacion[i]);
    }
    for (i = 0; i < 7; i++)
    {
        mv.VecError[i].valor = 0;
    }
    mv.VecError[0].descripcion = "Instruccion invalida";
    mv.VecError[1].descripcion = "Divicion por cero";
    mv.VecError[2].descripcion = "Fallo de segmento";
    mv.VecError[3].descripcion = "Direccion logica invalida";
    mv.VecError[4].descripcion = "Memoria Insuficiente";
    mv.VecError[5].descripcion = "Stack Overflow";
    mv.VecError[6].descripcion = "Stack Underflow";

    i = 0;
    while (i < argc && encontradoVMX == NULL)
    {
        encontradoVMX = strstr(argv[i], ".vmx");
        encontradoVMI = strstr(argv[i], ".vmi");
        i++;
    }
    if (encontradoVMX != NULL)
    {

        arch = fopen(argv[i - 1], "rb");
        if (!arch)
        {
            printf("Archivo invalido");
        }
    }
    else
        printf("No hay archivo");
    if (arch != NULL)
    {
        fread(mv.header.ident, 1, 5, arch);
        fread(&mv.header.v, 1, 1, arch);
        fread(aux, 1, 2, arch);
        TamC = (aux[0] << 8) | aux[1];
        if (strcmp(mv.header.ident, "VMX24") == 0)
            if (mv.header.v == 0x01 || 0x02)
            {
                mv.TDS[0].base = 0;
                mv.TDS[0].tam = TamC;
                if (mv.header.v == 0x01)
                {

                    mv.TDS[1].base = TamC;
                    mv.TDS[1].tam = TamC;
                    mv.Regs[CS] = mv.Regs[IP] = 0;
                    mv.Regs[DS] = 0x00010000;
                }
                else
                {
                    for (i = 2; i <= 4; i++)
                    {

                        mv.TDS[i].base = (mv.TDS[i - 1].tam + mv.TDS[i - 1].base);
                        fread(&mv.TDS[2].tam, 1, 2, arch);
                    }
                }
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
                printf("\nVersion no valida. ");
        else
            printf("\nIdentificador invalido.");
        return 0;
    }
}