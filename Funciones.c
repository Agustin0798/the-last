#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Funciones.h"
#include "MV.h"

void modificaCC(int *a, MV mv)
{
    if ((*a) < 0)
        mv.Regs[IP] = 0x80000000 + (mv.Regs[IP] & 0x3FFFFFFF);
    else if (*a == 0)
        mv.Regs[IP] = 0x40000000 + (mv.Regs[IP] & 0x3FFFFFFF);
    else
        mv.Regs[IP] = 0x00000000 + (mv.Regs[IP] & 0x3FFFFFFF);
}

void VACIO(int *a, int *b, MV mv)
{
    mv.VecError[0].valor = 1;
}

void MOV(int *a, int *b, MV mv)
{
    printf("\n operandoA:%d  operandoB:%d", *a, *b);
    *a = *b;
    printf("\n operandoA:%d  operandoB:%d", *a, *b);
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
        mv.VecError[0].valor = 1; // solucion preliminar para manejo de error
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
    mv.Regs[IP] = *b;
}
void JZ(int *a, int *b, MV mv)
{
    if ((mv.Regs[CC] & 0x40000000) == 0x40000000)
    {
        mv.Regs[IP] = *b;
    }
}

void JNN(int *a, int *b, MV mv)
{
    if ((mv.Regs[CC] & 0x80000000) != 0X80000000)
    {
        mv.Regs[IP] = *b;
    }
}

void JNP(int *a, int *b, MV mv)
{
    if ((mv.Regs[CC] & 0xc0000000) != 0)
    {
        mv.Regs[IP] = *b;
    }
}
void JP(int *a, int *b, MV mv)
{
    if ((mv.Regs[CC] & 0xc0000000) == 0)
    {
        mv.Regs[IP] = *b;
    }
}
void JN(int *a, int *b, MV mv)
{

    if ((mv.Regs[CC] & 0x80000000) != 0)
    {
        mv.Regs[IP] = *b;
    }
}

void JNZ(int *a, int *b, MV mv)
{

    if ((mv.Regs[CC] & 0x40000000) != 0)
    {
        mv.Regs[IP] = *b;
    }
}
void LDL(int *a, int *b, MV mv)
{
    mv.Regs[AC] = (mv.Regs[AC] | 0x0000ffff) & (short int)(0x8000 | ((*b) & 0xffff));
}
void LDH(int *a, int *b, MV mv)
{
    mv.Regs[AC] = (mv.Regs[AC] & 0x0000ffff) + ((0x8000 | ((*b) & 0xffff)) << 16);
}

void NOT(int *a, int *b, MV mv)
{
    *b = ~(*b);
}

void STOP(int *a, int *b, MV mv)
{
}
void SYS(int *a, int *b, MV mv)
{
    unsigned int dirFis, dirLog = mv.Regs[EDX], seg, offset;
    unsigned int tamCel = (mv.Regs[ECX] >> 8) & 0x000000FF;
    unsigned int cantCel = mv.Regs[ECX] & 0x000000FF;
    unsigned int modSys = mv.Regs[EAX] & 0x000000FF;
    int i, j;
    int dato;

    switch (*b)
    {
    case 1:
        seg = dirLog >> 16;
        offset = dirLog & 0x0000FFFF;
        if (seg < 5) // direccion logica valida (puede acceder a un elemento de la TDS)
        {
            dirFis = mv.TDS[seg].base + offset;
            for (i = 0; i < cantCel; i++)
            {
                switch (modSys)
                {
                case 1:
                    scanf("%d \n", &dato);
                    break;
                case 2:
                    scanf("%c \n", &dato);
                    break;
                case 4:
                    scanf("%o \n", &dato);
                    break;
                case 8:
                    scanf("%x \n", &dato);
                    break;
                }
                if (dato < 0)
                {
                    for (j = 0; j < tamCel - 1; j++)
                        if ((dirFis + j >= mv.TDS[seg].base) && (dirFis + j < mv.TDS[seg].base + mv.TDS[seg].tam))
                            mv.RAM[dirFis + j] = 0xFF;
                        else
                            mv.VecError[2].valor = 1; // error fallo de segmento
                    mv.RAM[dirFis + tamCel - 1] = dato;
                }
                else
                {
                    for (j = 0; j < tamCel - 1; j++)
                        if ((dirFis + j >= mv.TDS[seg].base) && (dirFis + j < mv.TDS[seg].base + mv.TDS[seg].tam))
                            mv.RAM[dirFis + j] = 0x00;
                        else
                            mv.VecError[2].valor = 1; // error fallo de segmento
                    mv.RAM[dirFis + tamCel - 1] = dato;
                }
                dirFis += tamCel; // me muevo para guardar el siguiente dato ingresado
            }
        }
        else
            mv.VecError[3].valor = 1; // dir log invalida
        break;
    case 2:
        seg = dirLog >> 16;
        offset = dirLog & 0x0000FFFF;
        dato = 0;
        if (seg < 5) // direccion logica valida (puede acceder a un elemento de la TDS)
        {
            dirFis = mv.TDS[seg].base + offset;
            for (i = 0; i < cantCel; i++) // muestra los datos
            {
                for (j = 0; j < tamCel; j++) // arma los datos
                {
                    if ((dirFis + j >= mv.TDS[seg].base) && (dirFis + j < mv.TDS[seg].base + mv.TDS[seg].tam))
                    {
                        dato += mv.RAM[dirFis + j];
                        if (j < tamCel - 1)
                            dato = dato << 8;
                    }
                    else
                    {
                        mv.VecError[2].valor = 1; // error fallo de segmento
                    }
                }
                switch (modSys)
                {
                case 1:
                    printf("%d \n", dato);
                    break;
                case 2:
                    printf("%c \n", dato);
                    break;
                case 4:
                    printf("%o \n", dato);
                    break;
                case 8:
                    printf("%x \n", dato);
                    break;
                }
            }
        }
        else
            mv.VecError[3].valor = 1; // dir log invalida
        break;
    }
}
