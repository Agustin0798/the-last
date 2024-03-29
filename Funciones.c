#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Funciones.h"

void MOV(int *a, int *b)
{
    *a=*b;  // agregar modificacion de cc
}

void ADD(int *a, int *b)
{
    *a+=*b; // agregar modificacion de cc
}

void SUB(int *a, int *b)
{
    *a-=*b;// agregar modificacion de cc
}

void MUL(int *a, int *b)
{
    *a*=*b;// agregar modificacion de cc
}

void DIV(int *a, int *b)
{
    if (*b != 0)
        *a/=*b;// agregar modificacion de cc
    else
        printf("Error divicion por 0");  //solucion preliminar para manejo de error
}

void SWAP(int *a, int *b)  // no debe aceptar operandos inmediatos
{
    int aux;

    aux=*a;
    *a=*b;
    *b=aux;
}

void CMP(int *a, int *b)
{
    int aux=*a-*b; // agregar modificacion de cc
}

void AND(int *a, int *b)
{
    *a&=*b; // agregar modificacion de cc
}

void OR(int *a, int *b)
{
    *a|=*b; // agregar modificacion de cc
}

void XOR(int *a, int *b)
{
    *a^=*b; // agregar modificacion de cc
}

void SHL(int *a, int *b)
{
    *a<<=*b;
}

void SHR(int *a, int *b)
{
    *a>>=*b;
}

void RND(int *a, int *b)
{
    srand(time(NULL));
    *a=rand() % (*b + 1);
}
