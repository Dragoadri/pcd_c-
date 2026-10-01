// =====================================================================
// Tema 1.4  |  Bucle for
// En este programa he usado for para contar 5 vueltas, sumar de 1 a 100, contar los pares hasta 20 e imprimir una tabla 3 x 3 con dos for anidados.
// Requisitos previos: 03_condicionales.cpp
// Qué observar al ejecutar: i empieza en 0 y termina en 4; la suma de 1 a 100 da 5050.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout

using namespace std;

int main() {
    // for (inicio; condición; actualización)  -> 5 vueltas: i = 0, 1, 2, 3, 4
    // i++ suma 1 a i al final de cada vuelta; cuando i vale 5, i < 5 es falso y se sale.
    for (int i = 0; i < 5; i++) {
        cout << "vuelta " << i << "\n";
    }

    // Acumulador: una variable que va sumando en cada vuelta.
    int suma = 0;                      // se crea FUERA del for para que no se reinicie en cada vuelta
    for (int i = 1; i <= 100; i++) {   // <= incluye el 100
        suma += i;                     // suma = suma + i;
    }
    cout << "Suma de 1 a 100: " << suma << "\n";

    // Contar solo los que cumplen algo: if dentro del for.
    int pares = 0;
    for (int i = 1; i <= 20; i++) {
        if (i % 2 == 0) {   // el resto de dividir entre 2 es 0: el número es par
            pares++;
        }
    }
    cout << "Pares entre 1 y 20: " << pares << "\n";

    // Bucles anidados: por cada vuelta de fila, el de columna da todas las suyas.
    for (int fila = 1; fila <= 3; fila++) {
        for (int columna = 1; columna <= 3; columna++) {
            cout << fila * columna << "\t";   // \t es un tabulador: separa las columnas
        }
        cout << "\n";   // al acabar cada fila, salto de línea
    }
    return 0;   // 0 = el programa ha terminado bien
}
