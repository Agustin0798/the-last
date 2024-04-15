#include "operandos.h"
#include "MV.h"
#include <stdlib.h>

int leerParametro(MV mv, char op) // Op ya esta negado(tamano)
{
  int i, paramValue;
  char aux;
  for (i = 1; i <= op; i++)
  {
    if (mv.TDS[0].tam > mv.Regs[5] + i) // 5 ip no hardcodear registro cs parte alta
    {

      aux = mv.RAM[mv.Regs[5] & 0x0000ffff + i];
      paramValue = (paramValue << 8) + aux; // byte a byte
      // mv.Regs[5] += i; //Uso ip sin actualizarlo
    }
    else
    {
      return -1; // CAmbiar Vector de errores; // Segmentation fault
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
  s = mv.Regs[codReg] >> 16;
  int direccionFisica = mv.TDS[s].base + mv.Regs[codReg] & 0x0000ffff + offset;
  for (i = 0; i < 4; i++)
  {

    if (mv.TDS[s].base >= direccionFisica && direccionFisica < mv.TDS[s].base + mv.TDS[s].tam)
    {

      aux = mv.RAM[direccionFisica];
      dato = (dato << 8) + aux;
    }
    else
    {
        mv.VecError[2].valor=1;
        return -1; // CAmbiar Vector de errores; // Segmentation fault
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
    return mv.Regs[value & 0x0f];
    break;
  case 1:
    return mv.Regs[value & 0x0f] & 0x000000ff;
    break;
  case 2:
    return (mv.Regs[value & 0x0f] & 0x0000ff00) >> 8;
    break;
  case 3:
    return mv.Regs[value & 0x0f] & 0x0000ffff;
    break;
  }
}

void setOperando(MV mv, char op, int dato)
{
}
int getOperando(MV mv, char op, char ipTemp)
{ // EJ op=0010 el valor llega con el desplazamiento. cambiar para segundo parametro

  int parametro, value = 0;
  parametro = leerParametro(mv, ~op);
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
    value = -1;
    break;
  }
  return value;
}
