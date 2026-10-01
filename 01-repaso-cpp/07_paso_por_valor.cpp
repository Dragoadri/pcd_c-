// =====================================================================
// Tema 1.7  |  Paso de parámetros por valor
// En este programa he pasado saldo por valor a sumar_diez para ver que la función cambia su copia y el saldo de main sigue en 100.
// Requisitos previos: 06_funciones_libres.cpp
// Qué observar al ejecutar: dentro de la función el valor cambia; fuera sigue igual.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout

using namespace std;

// int numero (sin &): al llamar, se crea una variable NUEVA con una copia del valor.
void sumar_diez(int numero) {
    numero = numero + 10;  // modifica la copia, no el original
    cout << "  Dentro de sumar_diez: numero = " << numero << "\n";
}   // aquí la copia desaparece y el +10 se pierde

int main() {
    int saldo = 100;

    cout << "Antes de llamar:   saldo = " << saldo << "\n";
    sumar_diez(saldo);   // se copia el valor 100 en numero; saldo no se toca
    cout << "Despues de llamar: saldo = " << saldo << "\n";

    // saldo sigue valiendo 100: la función recibió una copia.
    // Apúntalo: thread también COPIA los argumentos (tema 03).
    return 0;   // 0 = el programa ha terminado bien
}
