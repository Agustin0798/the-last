#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "Funciones.h"
#include "MV.h"

void modificaCC(int *a, MV *mv)
{
    if ((*a) < 0)
        mv->Regs[CC] = 0x80000000 + (mv->Regs[CC] & 0x3FFFFFFF);
    else if (*a == 0)
        mv->Regs[CC] = 0x40000000 + (mv->Regs[CC] & 0x3FFFFFFF);
    else
        mv->Regs[CC] = 0x00000000 + (mv->Regs[CC] & 0x3FFFFFFF);
}

void VACIO(int *a, int *b, MV *mv)
{
    mv->VecError[0].valor = 1;
}

void MOV(int *a, int *b, MV *mv)
{
    *a = *b;
    modificaCC(a, mv);
}

void ADD(int *a, int *b, MV *mv)
{

    *a += *b;
    // printf("\n%x %x", *a, *b);
    modificaCC(a, mv);
}

void SUB(int *a, int *b, MV *mv)
{
    *a -= *b;
    modificaCC(a, mv);
}

void MUL(int *a, int *b, MV *mv)
{
    *a *= *b;
    modificaCC(a, mv);
}

void DIV(int *a, int *b, MV *mv)
{
    if (*b != 0)
    {
        *a /= *b;
        modificaCC(a, mv);
    }
    else
        mv->VecError[0].valor = 1; // solucion preliminar para manejo de error
}

void SWAP(int *a, int *b, MV *mv) // no debe aceptar operandos inmediatos
{
    int aux;

    aux = *a;
    *a = *b;
    *b = aux;
}

void CMP(int *a, int *b, MV *mv)
{
    int aux = *a - *b;
    modificaCC(&aux, mv);
}

void AND(int *a, int *b, MV *mv)
{
    *a &= *b;
    modificaCC(a, mv);
}

void OR(int *a, int *b, MV *mv)
{
    *a |= *b;
    modificaCC(a, mv);
}

void XOR(int *a, int *b, MV *mv)
{
    *a ^= *b;
    modificaCC(a, mv);
}

void SHL(int *a, int *b, MV *mv)
{
    *a <<= *b;
}

void SHR(int *a, int *b, MV *mv)
{
    *a >>= *b;
}

void RND(int *a, int *b, MV *mv)
{
    srand(time(NULL));
    *a = rand() % (*b + 1);
}
void JMP(int *a, int *b, MV *mv)
{
    mv->Regs[IP] = *b;
}
void JZ(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0x40000000) == 0x40000000)
    {
        mv->Regs[IP] = *b;
    }
}

void JNN(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0x80000000) != 0X80000000)
    {
        mv->Regs[IP] = *b;
    }
}

void JNP(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0xc0000000) != 0)
    {
        mv->Regs[IP] = *b;
    }
}
void JP(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0xc0000000) == 0)
    {
        mv->Regs[IP] = *b;
    }
}
void JN(int *a, int *b, MV *mv)
{

    if ((mv->Regs[CC] & 0x80000000) != 0)
    {
        mv->Regs[IP] = *b;
    }
}

void JNZ(int *a, int *b, MV *mv)
{

    if ((mv->Regs[CC] & 0x40000000) != 0)
    {
        mv->Regs[IP] = *b;
    }
}
void LDL(int *a, int *b, MV *mv)
{
    mv->Regs[AC] = (mv->Regs[AC] | 0x0000ffff) & (short int)(0x8000 | ((*b) & 0xffff));
}
void LDH(int *a, int *b, MV *mv)
{
    mv->Regs[AC] = (mv->Regs[AC] & 0x0000ffff) + ((0x8000 | ((*b) & 0xffff)) << 16);
}

void NOT(int *a, int *b, MV *mv)
{
    *b = ~(*b);
}

void STOP(int *a, int *b, MV *mv)
{
}

void SYS(int *a, int *b, MV *mv)
{
    unsigned int dirFis, dirLog = mv->Regs[EDX], seg, offset;
    unsigned int tamCel = (mv->Regs[ECX] >> 8) & 0x000000FF;
    unsigned int cantCel = mv->Regs[ECX] & 0x000000FF;
    unsigned int modSys = mv->Regs[EAX] & 0x000000FF;
    int i, j, numero;
    char aux;
    int dato;

    switch (*b)
    {
    case 1: // read
        seg = dirLog >> 16;
        offset = dirLog & 0x0000FFFF;
        if (seg < 5) // puedo referirme a algun elemento de la TDS
        {
            dirFis = mv->TDS[seg].base + offset;
            i = 0;
            if (dirFis >= mv->TDS[seg].base)

                while ((dirFis < (mv->TDS[seg].base + mv->TDS[seg].tam)) && i < cantCel)
                {
                    switch (modSys)
                    {
                    case 1:
                        scanf(" %d", &dato);
                        break;
                    case 2:
                        scanf(" %c", &dato);
                        break;
                    case 4:
                        scanf(" %o", &dato);
                        break;
                    case 8:
                        scanf(" %x", &dato);
                    }
                    if ((dirFis + tamCel) < (mv->TDS[seg].base + mv->TDS[seg].tam))
                    {
                        for (j = tamCel - 1; j >= 0; j--)
                        {
                            mv->RAM[dirFis] = (dato >> (8 * j));
                            dirFis++;
                        }
                    }
                    i++;
                }
            if (i < cantCel)
                mv->VecError[2].valor = 1;
        }
        break;
    case 2:
        seg = dirLog >> 16;
        offset = dirLog & 0x0000FFFF;
        if (seg < 5)
        {
            dirFis = mv->TDS[seg].base + offset;
            // printf("\n%d Direccion Fisica SYS", dirFis);
            i = 0;
            if (dirFis >= mv->TDS[seg].base) // ACA esta el error
            {

                while ((dirFis < (mv->TDS[seg].base + mv->TDS[seg].tam)) && i < cantCel)
                {
                    dato = 0;
                    if ((dirFis + tamCel) < (mv->TDS[seg].base + mv->TDS[seg].tam))
                    {
                        if (tamCel == 4)
                        {
                            dato = (dato & 0x00FFFFFF) | (mv->RAM[dirFis] << 24);
                            dato = (dato & 0xFF00FFFF) | (mv->RAM[dirFis + 1] << 16);
                            dato = (dato & 0xFFFF00FF) | (mv->RAM[dirFis + 2] << 8);
                            dato = (dato & 0xFFFFFF00) | mv->RAM[dirFis + 3];
                            printf("\n%d %x ", dato);
                            printf("%2x\n", (mv->RAM[dirFis] << 24));
                            printf("%2x\n", (mv->RAM[dirFis + 1] << 16));
                            printf("%2x\n", (mv->RAM[dirFis + 2] << 8));
                            printf("%2x\n", (mv->RAM[dirFis + 3]));
                        }
                        else if (tamCel == 3)
                        {
                            dato = (dato & 0xFF00FFFF) | (mv->RAM[dirFis] << 16);
                            dato = (dato & 0xFFFF00FF) | (mv->RAM[dirFis + 1] << 8);
                            dato = (dato & 0xFFFFFF00) | mv->RAM[dirFis + 2];
                        }
                        else if (tamCel == 2)
                        {
                            dato = (dato & 0xFFFF00FF) | (mv->RAM[dirFis] << 8);
                            dato = (dato & 0xFFFFFF00) | mv->RAM[dirFis + 1];
                        }
                        else
                            dato = (dato & 0xFFFFFF00) | mv->RAM[dirFis];
                    }

                    printf("\nMEmoria: %x", mv->RAM[dirFis]);
                    i++;
                }
                switch (modSys)
                {
                case 1:
                    printf("%x\n", dato);
                    break;
                case 2:
                    if (isprint(dato))
                        printf("'%c\n", dato);

                    else
                        printf(".\n");
                    break;
                case 4:
                    printf("%o\n", dato);
                    break;
                case 8:
                    printf("%x\n", dato);
                }
            }
            if (i < cantCel)
            {
                mv->VecError[2].valor = 1;
            }
        }
        break;
    }
}
