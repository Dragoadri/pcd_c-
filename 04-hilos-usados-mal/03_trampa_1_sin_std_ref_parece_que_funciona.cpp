// =====================================================================
// Tema 4.3  |  TRAMPA 1 otra vez: sin ref "no hay carrera"
// En este programa he quitado el & y el ref al contador, y he visto que cada hilo cuenta bien en su copia pero main sigue viendo 0.
// Requisitos previos: 02_contador_compartido_roto, 03-hilos-creacion (04_trampa_1_...)
// Qué observar al ejecutar: cada hilo dice 1000000 exacto, pero main sigue viendo 0.
// Salida determinista: SI (cada hilo trabaja sobre su propia copia, no comparten nada)
// Compilar con: -O0 (target sin optimizar, igual que 02, para comparar en igualdad)
// =====================================================================
// Así llega un alumno a este código: escribe incrementar(int& contador),
// lanza thread(incrementar, contador), no compila, y "lo arregla"
// quitando el &. Ahora compila y los hilos cuentan perfecto... cada uno en
// su copia. No hay carrera porque no hay nada compartido: la demostración
// del 02 queda arruinada y parece que los hilos "no tienen ese problema".
#include <iostream>   // cout
#include <string>     // to_string
#include <thread>     // thread

using namespace std;

const int INCREMENTOS = 1000000;

void incrementar(int contador) {   // SIN &: el hilo recibe una copia
    for (int i = 0; i < INCREMENTOS; i++) {
        contador++;   // solo cambia la copia de ESTE hilo
    }
    // to_string convierte el número en texto para unirlo con + en una sola línea.
    cout << "[hilo] he contado " + to_string(contador) + "\n";
}

int main() {
    int contador = 0;
    thread hilo_1(incrementar, contador);   // sin ref: se copia
    thread hilo_2(incrementar, contador);   // sin ref: se copia
    hilo_1.join();   // espero a los dos hilos
    hilo_2.join();

    cout << "[main] contador = " << contador << "  (los hilos no han tocado mi variable)\n";
    return 0;   // 0 = el programa ha terminado bien
}
