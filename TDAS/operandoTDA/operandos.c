#include "operandos.h"
#include "../MVTDA/MV.h"
#include <stdlib.h>

int getParametro(MV mv, char op)
{ // EJ op=0010 el valor llega con el desplazamiento

  int parametro = 0, value;
  leerParametro(~op, mv);
  switch (op)
  {
  case 0:
    parametro = memoria(mv, value);
    break;

  case 1:
    parametro = inmediato(mv, value);
    break;

  case 2:
    parametro = registro(mv, value);
    break;

  default:
    parametro = NULL;
    break;
  }
  return parametro;
}

int leerParametro(char op, MV mv)
{
  int i, paramValue;
  for (i = 0; i < op - 1; i++)
  {
    if (mv.tablaSegmentos[0].size > mv.registros[5])
    {

      paramValue = mv.memoria[mv.registros[5] & 0x00ff + i];
      paramValue = paramValue << 4;
      mv.registros[5] += i;
    }
    else
    {
      return NULL; // Segmentation fault
    }
  }
  return paramValue;
}
int memoria(MV mv, int value) // Si
{
  char relleno = value >> 20;
  char codReg = value >> 16;
  char offset = value & 0x00ffff; // Tal vez sea innecesario por truncamiento
  if (mv.tablaSegmentos[1].size > mv.tablaSegmentos[1].base + offset)
  {
    return mv.memoria[mv.tablaSegmentos[1].base + offset];
  }
  else
  {
    return NULL; // Segmentation fault
  }
}

int inmediato(MV mv, int value)
{ // El value es directamente el valor del operando
  return value;
}

int registro(MV mv, int value)
{
  char secReg = value >> 4;
  switch (secReg)
  {
  case 0:
    return mv.registros[value & 0x0f];
    break;
  case 1:
    return mv.registros[value & 0x0f] & 0x000000ff;
    break;
  case 2:
    return mv.registros[value & 0x0f] & 0x0000ff00;
    break;
  case 3:
    return mv.registros[value & 0x0f] & 0x0000ffff;
    break;
  }
}