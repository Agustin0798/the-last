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

    while (i < 7 && !e)
        if (mv.VecError[i].valor == 1)
            e = 1;
        else
            i++;
    return e;
}

int Ejecuta(MV *mv, CodOpe codigosOperacion[32])
{
    char Cod, OP1, OP2, inst = 0;
    int Valor1, Valor2, ipAct, Iptemp;
    short int segIP, dirFisIP, offsetIP;
    segIP = mv->Regs[IP] >> 16;
    offsetIP = mv->Regs[IP] & 0x0000FFFF;
    dirFisIP = mv->TDS[segIP].base + offsetIP;
    inst = mv->RAM[dirFisIP] & 0xff;
    while ((dirFisIP >= mv->TDS[segIP].base) && (dirFisIP < (mv->TDS[segIP].base + mv->TDS[segIP].tam)) && strcmp(codigosOperacion[inst & 0b00011111], "STOP"))
    {
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;

        if ((Cod == 0x03 || Cod == 0x1A) && OP2 == 0b01)
            return 1;
        Iptemp = 0;
        Valor2 = getOperando(mv, OP2, mv->Regs[IP], Iptemp);
        Iptemp = ((~OP2) & 0x03);

        Valor1 = getOperando(mv, OP1, mv->Regs[IP], Iptemp);

        if (Errores(*mv))
        {
            return 1;
        }
        ipAct = mv->Regs[IP];
        // printf("\nIP ANTES%x    IpAct:%x", mv->Regs[IP], ipAct);
        mv->Regs[IP] += 1 + ((~OP1) & 0x03) + ((~OP2) & 0x03);

        // printf("\n%s %x   %x", codigosOperacion[Cod], Valor1, Valor2);
        funcion[Cod](&Valor1, &Valor2, mv);
        // printf("\n%x", mv->Regs[DS]);

        // printf("\n%s %x   %x", codigosOperacion[Cod], Valor1, Valor2);
        // printf("\n%x", mv->Regs[EDX]);
        if (Errores(*mv))
        {
            return 1;
        }
        if ((Cod & 0b10000) == 0b00000 && (Cod != 0x06))
        {
            // printf("\nCod:%x  llamo a set operando ipTemp:%d ipAct:%x", Cod, (~OP2) & 0x03, ipAct);
            setOperando(mv, OP1, Valor1, ipAct, ((~OP2) & 0x03));
            if (Cod == 0x03)
                setOperando(mv, OP2, Valor2, ipAct, 0);
            if (Errores(*mv))
            {
                return 1;
            }
        }
        else if ((Cod == 0x1A) || (Cod == 0x1C))
        {
            setOperando(mv, OP2, Valor2, ipAct, 0);
            if (Errores(*mv))
                return 1;
        }
        offsetIP = mv->Regs[IP] & 0x0000FFFF;
        dirFisIP = mv->TDS[segIP].base + offsetIP;
        inst = mv->RAM[dirFisIP];
        if (mv->enter == 1)
        {
            mv->enter = 0;
            Valor1 = 0;
            Valor2 = 0xF;
            funcion[0x10](&Valor1, &Valor2, mv);
        }
    }
    if ((dirFisIP >= (mv->TDS[segIP].base + mv->TDS[segIP].tam))) // me cai del cs?

    {
        mv->VecError[2].valor = 1; // fallo de segmento
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

    int IPaux, i, j, seg, dirFis,tam;
    unsigned char inst, OP1, OP2, Cod;
    char muestra1[3], muestra2[3];
    char *string;
    char *NomReg[16][4] = {
        {"CS", "", "", ""},
        {"DS", "", "", ""},
        {"ES", "", "", ""},
        {"SS", "", "", ""},
        {"KS", "", "", ""},
        {"IP", "", "", ""},
        {"SP", "", "", ""},
        {"BP", "", "", ""},
        {"CC", "", "", ""},
        {"AC", "", "", ""},
        {"EAX", "AL", "AH", "AX"},
        {"EBX", "BL", "BH", "BX"},
        {"ECX", "CL", "CH", "CX"},
        {"EDX", "DL", "DH", "DLX"},
        {"EEX", "EL", "EH", "EX"},
        {"EFX", "FL", "FH", "FX"},
    };
    seg= mv.Regs[KS] >> 16;
    dirFis= mv.TDS[seg].base;
    while (dirFis < (mv.TDS[seg].base + mv.TDS[seg].tam))// muestro el ks
    {
        inst= mv.RAM[dirFis];
        printf(" [%04X] %02X ",dirFis, (unsigned) inst);
        string=NULL;
        i=1;
        while (mv.RAM[dirFis+i] != 0x0) // armo el string
        {
            tam=strlen(string);
            string[tam]=mv.RAM[dirFis+i];
            string[tam+1]=0x00;
            i++;
        }
        tam=strlen(string) + 1;
        if (tam <= 6) // tengo espacio para mostrar todo el string??
        {
            for (i=0; i<tam; i++) //muestro el string completo
                printf("%02X ",string[i]);
            if (tam < 6)  //el string ocupo todo el espacio disponible??
                for (i=tam; i<6; i++) // lleno los espacios no ocupados
                    printf("   ");
        }
        else
        {
            for (i=0; i<5; i++)
                printf("%02X ",string[i]);
            printf(".. ");
        }
        printf("| \"%s\"\n",string);
    }

    seg = mv.Regs[CS] >> 16;
    IPaux = mv.TDS[seg].base;
    while (IPaux < (mv.TDS[seg].base + mv.TDS[seg].tam)) //muestro el cs
    {
        inst = mv.RAM[IPaux];
        Cod = inst & 0b00011111;
        OP1 = (inst >> 4) & 0x03;
        OP2 = (inst >> 6) & 0x03;

        if (IPaux == (mv.TDS[seg].base + mv.header.offsetEP))
            printf(">[%04X] %02X ", IPaux, (unsigned)inst);
        else
            printf(" [%04X] %02X ", IPaux, (unsigned)inst);

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

void iniciaEjecucion(int argc, char *argv[], MV *mv, CodOpe codigosOperacion[32])
{
    int i = 0, bandera = 0;
    while (i < argc && bandera != 1)
    {
        if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "-D") == 0))
        {
            bandera = 1;
        }
        else
            i++;
    }
    if (bandera == 1)
    {

        Disassembler(*mv, codigosOperacion);
    }
    Ejecuta(mv, codigosOperacion);
    i = 0;
    bandera = 0;
    for (i = 0; i <= 6; i++)
        if (mv->VecError[i].valor == 1)
            printf("\n%s\n", mv->VecError[i].descripcion);
}

int main(int argc, char *argv[])
{
    CodOpe codigosOperacion[32] =
        {"MOV", "ADD", "SUB", "SWAP", "MUL", "DIV", "CMP", "SHL", "SHR", "AND", "OR", "XOR", "RND", "", "", "", "SYS", "JMP", "JZ", "JP", "JN", "JNZ", "JNP", "JNN", "LDL", "LDH", "NOT", "PUSH", "POP", "CALL", "RET", "STOP"};
    char aux[3];
    int TamC, i, bandera, ultimoIndice, seg;
    short int tamSegAux;
    FILE *arch = NULL;
    MV mv;
    char *encontradoVMX = NULL, *encontradoVMI = NULL;
    for (i = 0; i <= 4; i++)
    {
        mv.Regs[i] = -1 << 16;
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
    mv.tamMem = MaxMem;
    mv.imagen = NULL;
    while (i < argc)
    {
        encontradoVMX = strstr(argv[i], ".vmx");
        encontradoVMI = strstr(argv[i], ".vmi");
        if (encontradoVMI != NULL)
        {

            strcpy(mv.imagen, argv[i]);
        }
        if (strstr(argv[i], "m="))
        {
            argv[i][0] = '0';
            argv[i][1] = '0';
            mv.tamMem = (int)strtol(argv[i], NULL, 10);
        }
        if (encontradoVMX != NULL)
        {
            arch = fopen(argv[i], "rb");
            if (!arch)
            {
                printf("Archivo invalido");
            }
        }

        i++;
    }
    if (arch != NULL)
    {
        for (i = 0; i <= 4; i++)
        {
            mv.header.tamanios[i] = 0;
        }
        fread(mv.header.ident, 1, 5, arch);
        fread(&mv.header.v, 1, 1, arch);
        if (strcmp(mv.header.ident, "VMX24") == 0)
            if (mv.header.v == 0x01 || mv.header.v == 0x02)
            {
                if (mv.header.v == 0x01)
                {

                    fread(aux, 1, 2, arch);
                    TamC = (aux[0] << 8) | aux[1];
                    mv.header.tamanios[CS] = TamC;
                    mv.TDS[0].base = 0;
                    mv.TDS[0].tam = TamC;
                    mv.TDS[1].base = TamC;
                    mv.header.tamanios[DS] = mv.TDS[1].tam = mv.tamMem - TamC;

                    mv.Regs[CS] = mv.Regs[IP] = 0;
                    mv.Regs[DS] = 0x00010000;
                }
                else // v2
                {
                    fread(aux, 1, 2, arch);
                    TamC = (aux[0] << 8) | aux[1];
                    mv.header.tamanios[CS] = TamC;
                    for (i = 1; i <= 4; i++)
                    {

                        fread(&aux, 1, 2, arch);
                        mv.header.tamanios[i] = (aux[0] << 8) | aux[1];
                    }
                    // printf("\n%x", mv.header.tamanios[CS]);
                    // printf("\n%x", mv.header.tamanios[KS]);
                    // printf("\n%x", mv.header.tamanios[ES]);
                    // printf("\n%x", mv.header.tamanios[SS]);
                    // printf("\n%x", mv.header.tamanios[DS]);

                    fread(&aux, 1, 2, arch);
                    mv.header.offsetEP = (aux[0] << 8) | aux[1];
                    if (mv.header.tamanios[KS] != 0)
                    {
                        mv.Regs[KS] = 0;
                        mv.TDS[0].base = 0;
                        mv.TDS[0].tam = mv.header.tamanios[KS];
                        ultimoIndice = 0;
                    }
                    else
                    {
                        ultimoIndice = -1;
                    }
                    for (i = 0; i <= 3; i++)
                    {
                        if (mv.header.tamanios[i] != 0)
                        {
                            ultimoIndice++;
                            if (ultimoIndice - 1 == -1)
                            {
                                mv.TDS[ultimoIndice].base = 0;
                            }
                            else
                            {

                                mv.TDS[ultimoIndice].base = mv.TDS[ultimoIndice - 1].base + mv.TDS[ultimoIndice - 1].tam;
                            }
                            mv.TDS[ultimoIndice].tam = mv.header.tamanios[i];

                            mv.Regs[i] = ultimoIndice << 16;
                        }
                    }
                    i = (mv.Regs[SS] >> 16) & 0xffff;
                    mv.Regs[SP] = (i << 16 | (mv.TDS[i].base + mv.TDS[i].tam));
                    mv.Regs[IP] = mv.Regs[CS] + (mv.header.offsetEP & 0xffff);
                }

                if ((mv.TDS[ultimoIndice].base + mv.TDS[ultimoIndice].tam) <= mv.tamMem)
                {
                    seg = mv.Regs[CS] >> 16;
                    for (i = mv.TDS[seg].base; i < (mv.TDS[seg].base + mv.TDS[seg].tam); i++)
                    {
                        fread(&mv.RAM[i], 1, 1, arch);
                    }
                    if (mv.Regs[KS] != -1)
                    {
                        seg = mv.Regs[KS] >> 16;
                        for (i = mv.TDS[seg].base; i < (mv.TDS[seg].base + mv.TDS[seg].tam); i++)
                        {
                            fread(&mv.RAM[i], 1, 1, arch);
                        }
                    }
                    iniciaEjecucion(argc, argv, &mv, codigosOperacion);
                }
                else
                {
                    puts(mv.VecError[4].descripcion);
                }
            }
            else
                printf("\nVersion no valida. ");
        else
            printf("\nIdentificador invalido.");
        return 0;
    }
    else
    {
        if (mv.imagen != NULL)
        {
            arch = fopen(mv.imagen, "rb");
            if (arch != NULL)
            {
                fread(mv.header.ident, 1, 5, arch);
                fread(&mv.header.v, 1, 1, arch);
                fread(aux, 1, 2, arch);
                mv.tamMem = (aux[0] << 8) | aux[1];
                if (strcmp(mv.header.ident, "VMI24") == 0)
                    if (mv.header.v == 0x01)
                    {
                        for (i = 0; i <= 15; i++)
                        {
                            fread(&mv.Regs[i], 1, 4, arch);
                        }
                        for (i = 0; i <= 4; i++)
                        {
                            fread(&mv.TDS[i].base, 1, 2, arch);
                            fread(&mv.TDS[i].tam, 1, 2, arch);
                        }
                        for (i = 0; i < mv.tamMem; i++)
                        {
                            fread(&mv.RAM[i], 1, 1, arch);
                        }
                        iniciaEjecucion(argc, argv, &mv, codigosOperacion);
                    }
            }
        }
    }
    fclose(arch);
}