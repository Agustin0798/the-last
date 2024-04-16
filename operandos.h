#ifndef OPERANDOS_H
#define OPERANDOS_H
#include "MV.h"

int getOperando(MV mv, char op, char iptemp);
void setOperando(MV mv, char op, int valor);
int memoria(MV mv, int value);
int registro(MV mv, int);
int leerParametro(MV mv, char op, char);
#endif