// =====================================================================
// Tema 4.1  |  La consola es un recurso compartido: cout entremezclado
// En este programa he puesto 4 hilos a escribir 30 líneas cada uno en cout a la vez, para ver que las líneas se cortan y se mezclan.
// Requisitos previos: 03-hilos-creacion
// Qué observar al ejecutar: líneas cortadas y trozos de un hilo metidos en la línea de otro.
// Salida determinista: NO (la mezcla cambia en cada ejecución)
// Compilar con: -O0 (target sin optimizar, como todo el tema 04)
// =====================================================================
// Cada << es una escritura independiente. Entre dos << de la misma línea,
// otro hilo puede colar su texto. Es la primera condición de carrera visible
// del curso; de momento solo se observa, el nombre y la solución llegan después.
// Calibración (16 núcleos, GCC 13.3): 4 hilos x 30 líneas = 120 líneas.
// En 10 de 10 ejecuciones hay líneas rotas: entre 20 y 48 en un terminal
// (como la consola de CLion) y entre 39 y 109 si la salida va a un fichero.
#include <iostream>   // cout
#include <thread>     // thread
#include <vector>     // vector

using namespace std;

const int NUM_HILOS = 4;
const int NUM_LINEAS = 30;

// char: un único carácter ('A', 'B'...).
void escribir(char letra) {
    for (int i = 1; i <= NUM_LINEAS; i++) {
        // Cinco << encadenados = cinco escrituras separadas en la consola
        cout << letra << letra << letra << " hilo " << letra << " linea " << i << "\n";
    }
}

int main() {
    vector<thread> hilos;   // aquí guardo los 4 hilos para poder hacerles join
    for (int i = 0; i < NUM_HILOS; i++) {
        // 'A' + i: sumar i a un carácter avanza i letras en el alfabeto.
        hilos.push_back(thread(escribir, 'A' + i));   // 'A', 'B', 'C', 'D'
    }
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();   // espero a que cada hilo termine de escribir
    }
    return 0;   // 0 = el programa ha terminado bien
}
