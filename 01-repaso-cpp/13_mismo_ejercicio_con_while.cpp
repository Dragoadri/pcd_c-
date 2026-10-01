// =====================================================================
// Tema 1.13  |  El mismo ejercicio resuelto con while
// En este programa he resuelto con while el ejercicio de las notas del fichero 12, repartiendo inicialización, condición y actualización.
// Requisitos previos: 12_mismo_ejercicio_con_for.cpp
// Qué observar al ejecutar: la salida es idéntica a la de 12_mismo_ejercicio_con_for.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// ENUNCIADO (el mismo en 12 y 13): dadas las notas de la clase, calcular
// la suma, la media y cuántos alumnos han aprobado (nota >= 5).
#include <iostream>   // cout
#include <vector>     // vector

using namespace std;

int main() {
    // Vector de 6 double inicializado con llaves: notas[0] vale 7.5 y notas[5] vale 6.5.
    vector<double> notas = {7.5, 4.0, 9.0, 5.0, 3.5, 6.5};
    double suma = 0.0;
    int aprobados = 0;

    int i = 0;                         // (1) inicialización: ANTES del bucle
    // (int) notas.size(): el tamaño (6) convertido a int para compararlo con i.
    while (i < (int) notas.size()) {   // (2) condición: arriba
        suma = suma + notas[i];        // notas[i] es la nota de la posición i
        if (notas[i] >= 5.0) {
            aprobados = aprobados + 1;
        }
        i++;                           // (3) actualización: AL FINAL del cuerpo
    }

    cout << "Suma: " << suma << "\n";
    // suma es double, así que la división tiene decimales (no es división entera).
    cout << "Media: " << suma / notas.size() << "\n";
    cout << "Aprobados: " << aprobados << " de " << notas.size() << "\n";

    // Error típico: olvidar el i++ del final. El for no deja olvidarlo;
    // el while sí, y entonces el bucle no termina nunca.
    return 0;   // 0 = el programa ha terminado bien
}
