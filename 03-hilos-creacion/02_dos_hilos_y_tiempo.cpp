// =====================================================================
// Tema 3.2  |  Dos tareas en secuencia frente a dos tareas en hilos
// En este programa he llamado dos veces a esperar, primero una detrás de otra y luego en dos hilos, y he medido que con hilos tarda la mitad.
// Requisitos previos: 01_primer_hilo, 01-repaso-cpp (chrono y sleep_for)
// Qué observar al ejecutar: en secuencia ~4 s; con hilos ~2 s, y las dos tareas se duermen a la vez.
// Salida determinista: NO (en la parte con hilos el orden entre A y B puede variar)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Origen: Tema 4 presencial, "función de espera" sin hilos y con hilos
// (Python: H1.start(), H2.start(), H1.join(), H2.join()).
// En C++ no existe start(): construir el thread ya lo pone en marcha.
#include <chrono>     // seconds para dormir, steady_clock para medir tiempos
#include <iostream>   // cout
#include <string>     // string
#include <thread>     // thread y this_thread

using namespace std;

const int SEGUNDOS_DE_ESPERA = 2;

// Simula una tarea que pasa casi todo su tiempo esperando (disco, red, usuario).
// const string&: uso el texto original sin copiarlo y sin poder modificarlo.
void esperar(const string& nombre) {
    // Línea completa en un solo cout: reduce (no elimina) la mezcla de texto. Ver 04.
    cout << nombre + " ---> a dormir...\n";
    this_thread::sleep_for(chrono::seconds(SEGUNDOS_DE_ESPERA));   // duerme ESTE hilo 2 segundos
    cout << nombre + " ---> ... y despertamos!\n";
}

int main() {
    // ---- 1. Programa lineal: B no empieza hasta que termina A ----
    // steady_clock::now() da la hora actual de un reloj que nunca va hacia atrás.
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();
    esperar("Tarea A");
    esperar("Tarea B");
    // Restar dos instantes da una duración; con <double> queda en segundos con decimales.
    chrono::duration<double> tiempo = chrono::steady_clock::now() - inicio;
    cout << "Tiempo en secuencia: " << tiempo.count() << " s\n\n";   // count(): el número de segundos

    // ---- 2. Programa concurrente: A y B esperan a la vez ----
    inicio = chrono::steady_clock::now();
    // Detrás de la función va su argumento: el hilo llamará a esperar("Tarea A").
    thread hilo_a(esperar, "Tarea A");   // arranca ya
    thread hilo_b(esperar, "Tarea B");   // arranca ya, sin esperar a A
    hilo_a.join();   // main se para hasta que acaba hilo_a
    hilo_b.join();   // si hilo_b ya ha acabado, join vuelve enseguida
    tiempo = chrono::steady_clock::now() - inicio;
    cout << "Tiempo con hilos:    " << tiempo.count() << " s\n";
    return 0;   // 0 = el programa ha terminado bien
}
