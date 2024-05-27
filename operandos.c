#include "operandos.h"
#include "MV.h"
#include <stdlib.h>
#include <stdio.h>

int leerParametro(MV *mv, char op, int ipAct, int ipTemp)
{
  int i, paramValue = 0;
  short int dirFis = mv->TDS[ipAct >> 16].base + ipAct & 0xffff;
  char aux;
  for (i = 1; i <= op; i++)
  {
    if ((mv->TDS[mv->Regs[CS] >> 16].tam + mv->TDS[mv->Regs[CS] >> 16].base) > (dirFis + ipTemp + i))
    {
      aux = (unsigned)mv->RAM[dirFis + ipTemp + i];

      paramValue = ((paramValue << 8) | (aux & 0xff));
    }
    else
    {
      mv->VecError[2].valor = 1;
    }
  }

  return paramValue;
}
int leerMemoria(MV *mv, int value)
{
  char tamLectura = value >> 20;
  char codReg = (value >> 16) & 0x0f;
  char offset = value & 0x00ffff;
  int i, aux, dato = 0;

  char s;
  s = mv->Regs[codReg] >> 16;
  int direccionFisica = mv->TDS[s].base + (mv->Regs[codReg] & 0x0000ffff) + offset;
  for (i = 3; i >= 0; i--)
  {
    if (mv->TDS[s].base <= direccionFisica && direccionFisica < mv->TDS[s].base + mv->TDS[s].tam)
    {
      aux = mv->RAM[direccionFisica];
      dato |= (aux << i * 8);
      direccionFisica++;
    }
    else
    {
      mv->VecError[2].valor = 1;
      return -1;
    }
  }

  return dato;
}
void escribeMemoria(MV *mv, int valor, int parametro)
{
  char s, codReg = (parametro >> 16) & 0x0f, tamEscritura = (~(parametro >> 22) & 0x3) + 1;
  int i;
  char offset = 0;
  offset = (parametro & 0x0000ffff);
  int direccionFisica;
  if (codReg == 0)
  {
    s = mv->Regs[DS] >> 16;
  }
  else
  {
    s = ((*mv).Regs[codReg] >> 16) & 0x0000ffff;
  }
  direccionFisica = (*mv).TDS[s].base + ((*mv).Regs[codReg] & 0x0000ffff) + offset;
  if (((*mv).TDS[s].base <= direccionFisica) && ((direccionFisica + tamEscritura) < (*mv).TDS[s].base + (*mv).TDS[s].tam))
  {

    for (i = tamEscritura - 1; i >= 0; i--)
    {
      mv->RAM[direccionFisica] = 0;

      mv->RAM[direccionFisica] |= (unsigned char)(((valor >> (8 * i)) & 0xFF));
      direccionFisica++;
    }
  }
  else
  {
    (*mv).VecError[2].valor = 1;
  }
}
int inmediato(MV mv, int value)
{
  return value;
}

int leerRegistro(MV *mv, int value)
{
  char secReg = value >> 4;
  switch (secReg)
  {
  case 0:
    return mv->Regs[value & 0x0f];
    break;
  case 1:
    return mv->Regs[value & 0x0f] & 0x000000ff;
    break;
  case 2:
    return (mv->Regs[value & 0x0f] >> 8) & 0x000000ff;
    break;
  case 3:
    return mv->Regs[value & 0x0f] & 0x0000ffff;
    break;
  }
}
void escribeRegistro(MV *mv, int valor, int parametro)
{
  char secReg = (parametro >> 4) & 0b11;
  switch (secReg)
  {
  case 0:
    (*mv).Regs[parametro & 0x0f] = valor;
    break;
  case 1:

    (*mv).Regs[parametro & 0x0f] = ((*mv).Regs[parametro & 0x0f] & 0xffffff00) | (valor & 0x000000ff);

    break;
  case 2:

    (*mv).Regs[parametro & 0x0f] = ((*mv).Regs[parametro & 0x0f] & 0xffff00ff) | ((valor & 0x000000ff) << 8);

    break;
  case 3:

    (*mv).Regs[parametro & 0x0f] = ((*mv).Regs[parametro & 0x0f] & 0xffff0000) | (valor & 0x0000ffff);
    break;
  }
}

void setOperando(MV *mv, char op, int dato, int ipAct, int ipTemp)
{
  int parametro;
  parametro = leerParametro(mv, (~op) & 0x03, ipAct, ipTemp);
  switch (op)
  {
  case 0:
    escribeMemoria(mv, dato, parametro);

    break;

  case 2:
    escribeRegistro(mv, dato, parametro);
    break;
  }
}
int getOperando(MV *mv, char op, int ipAct, int ipTemp)
{

  int parametro, value = 0;
  parametro = leerParametro(mv, (~op) & 0x03, mv->Regs[IP], ipTemp);
  switch (op)
  {
  case 0:
    value = leerMemoria(mv, parametro);
    break;

  case 1:
    value = (parametro);

    break;

  case 2:
    value = leerRegistro(mv, parametro);
    break;

  default:
    value = -1;
    break;
  }
  return value;
}
