// =====================================================================
// Tema 1.12  |  Un ejercicio resuelto con for
// En este programa he recorrido el vector notas con un for para calcular la suma, la media y cuántos aprobados hay.
// Requisitos previos: 10_vector_y_string.cpp
// Qué observar al ejecutar: la salida es idéntica a la de 13_mismo_ejercicio_con_while.
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

    //   (1) inicialización   (2) condición              (3) actualización
    // (int) notas.size(): el tamaño (6) convertido a int para compararlo con i.
    for (int i = 0;i < (int) notas.size();i++) {
        suma = suma + notas[i];   // notas[i] es la nota de la posición i
        if (notas[i] >= 5.0) {
            aprobados = aprobados + 1;
        }
    }

    cout << "Suma: " << suma << "\n";
    // suma es double, así que la división tiene decimales (no es división entera).
    cout << "Media: " << suma / notas.size() << "\n";
    cout << "Aprobados: " << aprobados << " de " << notas.size() << "\n";

    // El for junta las tres piezas del bucle en una línea.
    // El while (fichero 13) las reparte: (1) antes, (2) arriba, (3) al final.
    return 0;   // 0 = el programa ha terminado bien
}
