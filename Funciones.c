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
    mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
}
void JZ(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0x40000000) == 0x40000000)
    {
        mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
    }
}

void JNN(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0x80000000) != 0X80000000)
    {
        mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
    }
}

void JNP(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0xc0000000) != 0)
    {
        mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
    }
}
void JP(int *a, int *b, MV *mv)
{
    if ((mv->Regs[CC] & 0xc0000000) == 0)
    {
        mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
    }
}
void JN(int *a, int *b, MV *mv)
{

    if ((mv->Regs[CC] & 0x80000000) != 0)
    {
        mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
    }
}

void JNZ(int *a, int *b, MV *mv)
{

    if ((mv->Regs[CC] & 0x40000000) != 0)
    {
         mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | ((*b) & 0x0000FFFF);
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

void PUSH(int *a, int *b, MV *mv)
{
    int dirFis,dirLog,seg,offset;

    dirLog = mv->Regs[SP]-=4;
    seg = (dirLog >> 16) & 0x0000FFFF;
    offset = dirLog & 0x0000FFFF;
    dirFis = mv->TDS[seg].base + offset;
    if (dirFis < mv->TDS[seg].base)
        mv->VecError[5].valor=1; // se produjo stack overflow
    else
    {
        mv->RAM[dirFis]=((*b) >> 24) & 0x000000FF;
        mv->RAM[dirFis+1]=((*b) >> 16) & 0x000000FF;
        mv->RAM[dirFis+2]=((*b) >> 8) & 0x000000FF;
        mv->RAM[dirFis+3]=(*b) & 0x000000FF;
    }
}

void POP(int *a, int *b, MV *mv)
{
    int dirFis,dirLog,seg,offset,dato;

    dirLog = mv->Regs[SP]+=4;
    seg = (dirLog >> 16) & 0x0000FFFF;
    offset = dirLog & 0x0000FFFF;
    dirFis = mv->TDS[seg].base + offset;
    if (dirFis >= (mv->TDS[seg].base + mv->TDS[seg].tam))
        mv->VecError[6].valor=1; //se produjo stack underflow;
    else
    {
        dato=0;
        dato=(dato & 0x00FFFFFF) | (mv->RAM[dirFis-4] << 24);
        dato=(dato & 0xFF00FFFF) | (mv->RAM[dirFis-3] << 16);
        dato=(dato & 0xFFFF00FF) | (mv->RAM[dirFis-2] << 8);
        dato=(dato & 0xFFFFFF00) | (mv->RAM[dirFis-1]);
        (*b)=dato;
    }
}

void CALL(int *a, int *b, MV *mv)
{
    int ip=mv->Regs[IP];

    PUSH(a,&ip,mv);
    JMP(a,b,mv);
}

void RET(int *a, int *b, MV *mv)
{
    int auxIP;

    POP(a,&auxIP,mv);
    mv->Regs[IP] = (mv->Regs[IP] & 0xFFFF0000) | (auxIP & 0x0000FFFF);
}

void SYS(int *a, int *b, MV *mv)
{
    unsigned int dirFis, dirLog = mv->Regs[EDX], seg, offset;
    unsigned int tamCel = (mv->Regs[ECX] >> 8) & 0x000000FF;
    unsigned int cantCel = mv->Regs[ECX] & 0x000000FF;
    unsigned int modSys = mv->Regs[EAX] & 0x000000FF;
    int i, j, numero;
    char aux,Op;
    char *string;
    int dato;
    FILE *archIMG;

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
            printf("\n%d tAMANO\n", tamCel);
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
                        switch (modSys)
                        {
                        case 1:
                            printf("%d\n", dato);
                            break;
                        case 2:
                            printf("%c\n", dato);
                            break;
                        case 4:
                            printf("%o\n", dato);
                            break;
                        case 8:
                            printf("%x\n", dato);
                        }
                    }
                    i++;
                }
            }
            if (i < cantCel)
            {
                mv->VecError[2].valor = 1;
            }
        }
        break;
    case 'F':
            if (mv->enter == 1)
            {
                archIMG=fopen(mv->imagen,"Wb");
                if (archIMG != NULL)
                {
                    fwrite("VMX24",5,1,archIMG);
                    fwrite('1',1,1,archIMG);
                    fwrite(mv->tamMem,2,1,archIMG);
                    for (i=0; i<16; i++)
                        fwrite(mv->Regs[i],4,1,archIMG);
                    for (i=0; i<5; i++)
                    {
                        fwrite(mv->TDS[i].base,2,1,archIMG);
                        fwrite(mv->TDS[i].tam,2,1,archIMG);
                    }
                    for (i=0; i<mv->tamMem; i++)
                        fwrite(mv->RAM[i],1,1,archIMG);
                    switch (getchar())
                    {
                    case '\n':mv->enter=1;
                        break;
                    case 'q':
                    case 'Q': mv->Regs[IP]=(mv->Regs[IP] & 0xFFFF0000) | mv->tamMem;
                        break;
                    case 'g':
                    case 'G':
                        break;
                    }
                }
            }
        break;
    case 3:
            seg= dirLog >> 16;
            offset= dirLog & 0x0000FFFF;
            if (seg < 5)
            {
                dirFis= mv->TDS[seg].base + offset;
                i=0;
                //string
                //while (i )
            }
        break;
    case 4:
        break;
    }
}
