// =====================================================================
// Tema 3.7  |  El orden de ejecución de los hilos no está garantizado
// En este programa he lanzado 8 hilos en orden, todos con el mismo trabajo, y he visto que en cada ronda imprimen sus números en otro orden.
// Requisitos previos: 06_muchos_hilos_con_vector
// Qué observar al ejecutar: en cada ronda el orden de los números es distinto.
// Salida determinista: NO (el planificador del sistema decide qué hilo avanza en cada momento)
// Compilar con: -O0 (con -O2 el compilador borra el bucle de trabajo y el desorden se ve menos)
// =====================================================================
// Nadie duerme: los hilos se crean 0, 1, 2... y TODOS hacen exactamente el
// mismo trabajo antes de imprimir. Aun así el orden cambia de una ronda a otra
// y de una ejecución a otra: lo decide el planificador, no el código.
#include <iostream>   // cout
#include <string>     // to_string
#include <thread>     // thread
#include <vector>     // vector

using namespace std;

const int NUM_HILOS = 8;
const int NUM_RONDAS = 5;

const int TRABAJO = 3000000;   // mismo trabajo para todos los hilos

void decir_numero(int numero) {
    long long cuenta = 0;              // variable LOCAL: cada hilo tiene la suya
    // Bucle que solo gasta tiempo de CPU, para que los hilos coincidan trabajando.
    for (int i = 0; i < TRABAJO; i++) {
        cuenta = cuenta + 1;
    }
    cout << to_string(numero) + " ";   // número y espacio en un solo texto: una sola escritura
}

int main() {
    for (int ronda = 1; ronda <= NUM_RONDAS; ronda++) {
        cout << "Ronda " << ronda << ": ";

        vector<thread> hilos;   // vector nuevo en cada ronda
        for (int i = 0; i < NUM_HILOS; i++) {
            hilos.push_back(thread(decir_numero, i));   // crea el hilo i y lo guarda al final
        }
        for (int i = 0; i < NUM_HILOS; i++) {
            hilos[i].join();   // espero a los 8 antes de pasar a la siguiente ronda
        }

        cout << "\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}
