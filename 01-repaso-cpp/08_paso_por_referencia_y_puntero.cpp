// =====================================================================
// Tema 1.8  |  Paso de parámetros por referencia (y por puntero)
// En este programa he pasado saldo por referencia a sumar_diez y por puntero a sumar_cien, y he visto que ambas cambian el saldo de main.
// Requisitos previos: 07_paso_por_valor.cpp
// Qué observar al ejecutar: ahora el cambio hecho dentro de la función se ve fuera.
// Salida determinista: NO (la dirección de memoria impresa cambia en cada ejecución)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout

using namespace std;

// int& : "numero" es OTRO NOMBRE de la variable del llamante, no una copia.
void sumar_diez(int& numero) {
    numero = numero + 10;   // como numero ES saldo, esto cambia saldo en main
}

// int* : recibe la DIRECCIÓN de la variable. *puntero es "la variable apuntada".
void sumar_cien(int* puntero) {
    // A la derecha, *puntero LEE el valor que hay en esa dirección (110).
    // A la izquierda, *puntero ESCRIBE en esa dirección: guarda 210 dentro de saldo.
    *puntero = *puntero + 100;
}

int main() {
    int saldo = 100;

    sumar_diez(saldo);  // por referencia: se escribe igual que por valor
    cout << "Tras sumar_diez(saldo):  saldo = " << saldo << "\n";

    sumar_cien(&saldo);  // por puntero: &saldo es "la dirección de saldo"
    cout << "Tras sumar_cien(&saldo): saldo = " << saldo << "\n";
    // Una dirección se imprime en hexadecimal (0x7ffd...) y cambia en cada ejecución.
    cout << "Direccion de saldo: " << &saldo << "\n";

    // Resumen del símbolo &:  en un tipo (int&) declara una referencia;
    // delante de una variable (&saldo) obtiene su dirección.
    // Resumen del símbolo *:  en un tipo (int*) declara un puntero;
    // delante de un puntero (*puntero) accede a la variable apuntada.

    // Con referencia o puntero, dos funciones trabajan sobre LA MISMA
    // variable. Es lo que harán los hilos, y ahí empiezan los problemas:
    // si la modifican a la vez, o si la variable ya no existe (09_ambito_y_tiempo_de_vida).
    return 0;   // 0 = el programa ha terminado bien
}
