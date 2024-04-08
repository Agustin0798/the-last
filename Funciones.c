#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Funciones.h"
#include "/../MV.h"

void MOV(int *a, int *b, MV mv)
{
    *a = *b; // agregar modificacion de cc
}

void ADD(int *a, int *b, MV mv)
{
    *a += *b; // agregar modificacion de cc
}

void SUB(int *a, int *b, MV mv)
{
    *a -= *b; // agregar modificacion de cc
}

void MUL(int *a, int *b, MV mv)
{
    *a *= *b; // agregar modificacion de cc
}

void DIV(int *a, int *b, MV mv)
{
    if (*b != 0)
        *a /= *b; // agregar modificacion de cc
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
    int aux = *a - *b; // agregar modificacion de cc
}

void AND(int *a, int *b, MV mv)
{
    *a &= *b; // agregar modificacion de cc
}

void OR(int *a, int *b, MV mv)
{
    *a |= *b; // agregar modificacion de cc
}

void XOR(int *a, int *b, MV mv)
{
    *a ^= *b; // agregar modificacion de cc
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
    mv.registros[5] = *b;
}
void JZ(int *a, int *b, MV mv)
{
    if (mv.registros[8] & 0x40000000 == 0x40000000)
    {
        mv.registros[5] = *b;
    }
}

void JNN(int *a, int *b, MV mv)
{
    if (mv.registros[8] & 0x80000000 != 0X80000000)
    {
        mv.registros[5] = *b;
    }
}

void JNP(int *a, int *b, MV mv)
{
    if (mv.registros[8] & 0xc0000000 != 0)
    {
        mv.registros[5] = *b;
    }
}