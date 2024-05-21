#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "MV.h"
void VACIO(int *a, int *b, MV *mv);

void MOV(int *a, int *b, MV *mv);

void ADD(int *a, int *b, MV *mv);

void SUB(int *a, int *b, MV *mv);

void MUL(int *a, int *b, MV *mv);

void DIV(int *a, int *b, MV *mv);

void SWAP(int *a, int *b, MV *mv);

void CMP(int *a, int *b, MV *mv);

void AND(int *a, int *b, MV *mv);

void OR(int *a, int *b, MV *mv);

void XOR(int *a, int *b, MV *mv);

void SHL(int *a, int *b, MV *mv);

void SHR(int *a, int *b, MV *mv);

void RND(int *a, int *b, MV *mv);

void JMP(int *a, int *b, MV *mv);

void JZ(int *a, int *b, MV *mv);

void SYS(int *a, int *b, MV *mv);

void JP(int *a, int *b, MV *mv);

void JN(int *a, int *b, MV *mv);

void JNZ(int *a, int *b, MV *mv);

void JNP(int *a, int *b, MV *mv);

void JNN(int *a, int *b, MV *mv);

void LDL(int *a, int *b, MV *mv);

void LDH(int *a, int *b, MV *mv);

void NOT(int *a, int *b, MV *mv);

void STOP(int *a, int *b, MV *mv);

void POP(int *a, int *b, MV *mv);

void PUSH(int *a, int *b, MV *mv);

void CALL(int *a, int *b,  MV *mv);

void RET(int *a, int *b, MV *mv);

#endif
