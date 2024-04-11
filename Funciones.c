#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Funciones.h"
#include "MV.h"

void modificaCC(int *a, MV mv)
{
    if (*a < 0)
        mv.Regs[8] = 0x80000000;
    else if (*a == 0)
        mv.Regs[8] = 0x40000000;
    else
        mv.Regs[8] = 0x00000000;
}

void VACIO(int *a, int *b, MV mv)
{
    mv.VecError[0].valor = 1;
}

void MOV(int *a, int *b, MV mv)
{
    *a = *b;
    modificaCC(a, mv);
}

void ADD(int *a, int *b, MV mv)
{
    *a += *b;
    modificaCC(a, mv);
}

void SUB(int *a, int *b, MV mv)
{
    *a -= *b;
    modificaCC(a, mv);
}

void MUL(int *a, int *b, MV mv)
{
    *a *= *b;
    modificaCC(a, mv);
}

void DIV(int *a, int *b, MV mv)
{
    if (*b != 0)
    {
        *a /= *b;
        modificaCC(a, mv);
    }
    else
        printf("Error divicion por 0"); // solucion preliminar para manejo de error
}

void SWAP(int *a, int *b, MV mv) // no debe aceptar operandos inmediatos
{
    int aux;

    aux = *a;
    *a = *b;
    *b = aux;
}

void CMP(int *a, int *b, MV mv)
{
    int aux = *a - *b;
    modificaCC(&aux, mv);
}

void AND(int *a, int *b, MV mv)
{
    *a &= *b;
    modificaCC(a, mv);
}

void OR(int *a, int *b, MV mv)
{
    *a |= *b;
    modificaCC(a, mv);
}

void XOR(int *a, int *b, MV mv)
{
    *a ^= *b;
    modificaCC(a, mv);
}

void SHL(int *a, int *b, MV mv)
{
    *a <<= *b;
}

void SHR(int *a, int *b, MV mv)
{
    *a >>= *b;
}

void RND(int *a, int *b, MV mv)
{
    srand(time(NULL));
    *a = rand() % (*b + 1);
}
void JMP(int *a, int *b, MV mv)
{
    mv.Regs[5] = *b;
}
void JZ(int *a, int *b, MV mv)
{
    if (mv.Regs[8] & 0x40000000 == 0x40000000)
    {
        mv.Regs[5] = *b;
    }
}

void JNN(int *a, int *b, MV mv)
{
    if (mv.Regs[8] & 0x80000000 != 0X80000000)
    {
        mv.Regs[5] = *b;
    }
}

void JNP(int *a, int *b, MV mv)
{
    if (mv.Regs[8] & 0xc0000000 != 0)
    {
        mv.Regs[5] = *b;
    }
}

void SYS(int *a, int *b, MV mv)
{
    unsigned int dir = mv.Regs[13];                        // EDX
    unsigned int tamCel = (mv.Regs[12] >> 8) & 0x000000FF; // CH
    unsigned int cantCel = mv.Regs[12] & 0x000000FF;       // CL
    unsigned int modSys = mv.Regs[10] & 0x000000FF;

    switch (*b)
    {
    case 1:

        break;
    }
}
