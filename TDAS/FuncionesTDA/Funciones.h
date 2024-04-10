#include "../MVTDA/MV.h"

void MOV(int *a, int *b, MV mv);

void ADD(int *a, int *b, MV mv);

void SUB(int *a, int *b, MV mv);

void MUL(int *a, int *b, MV mv);

void DIV(int *a, int *b, MV mv);

void SWAP(int *a, int *b, MV mv);

void CMP(int *a, int *b, MV mv);

void AND(int *a, int *b, MV mv);

void OR(int *a, int *b, MV mv);

void XOR(int *a, int *b, MV mv);

void SHL(int *a, int *b, MV mv);

void SHR(int *a, int *b, MV mv);

void RND(int *a, int *b, MV mv);

void JMP(int *a, int *b, MV mv);
void JZ(int *a, int *b, MV mv);
// SYS (agus)

// jumps
// ldh carga parate alta de acc
// ldl carga parte baja
// NOT
// STOP (agus)
// Funcion cambia cc