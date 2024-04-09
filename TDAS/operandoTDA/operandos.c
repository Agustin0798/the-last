#include "operandos.h"
#include "../MVTDA/MV.h"
#include <stdlib.h>

int getParametro(MV mv, char op)
{ // EJ op=0010 el valor llega con el desplazamiento. cambiar para segundo parametro

  int parametro, value = 0;
  parametro = leerParametro(~op, mv);
  switch (op)
  {
  case 0:
    value = memoria(mv, parametro);
    break;

  case 1:
    value = parametro;
    break;

  case 2:
    value = registro(mv, parametro);
    break;

  default:
    value = NULL;
    break;
  }
  return value;
}

int leerParametro(char op, MV mv) // Op ya esta negado(tamano)
{
  int i, paramValue;
  char aux;
  for (i = 1; i <= op; i++)
  {
    if (mv.tablaSegmentos[0].size > mv.registros[5] + i) // 5 ip no hardcodear registro cs parte alta
    {

      aux = mv.memoria[mv.registros[5] & 0x0000ffff + i];
      paramValue = (paramValue << 8) + aux; // byte a byte
      // mv.registros[5] += i; //Uso ip sin actualizarlo
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
  char aux, tamLectura = value >> 20;
  int i, dato;
  char codReg = (value >> 16) & 0x0f;
  char offset = value & 0x00ffff; // Tal vez sea innecesario por truncamiento

  char s;
  s = mv.registros[codReg] >> 16;
  int direccionFisica = mv.tablaSegmentos[s].base + mv.registros[codReg] & 0x0000ffff + offset;
  for (i = 0; i < 4; i++)
  {

    if (mv.tablaSegmentos[s].base >= direccionFisica && direccionFisica < mv.tablaSegmentos[s].base + mv.tablaSegmentos[s].size)
    {

      aux = mv.memoria[direccionFisica];
      dato = (dato << 8) + aux;
    }
    else
    {
      return NULL; // Segmentation fault
    }
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
    return (mv.registros[value & 0x0f] & 0x0000ff00) >> 8;
    break;
  case 3:
    return mv.registros[value & 0x0f] & 0x0000ffff;
    break;
  }
}