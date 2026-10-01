#include <thread>
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>

using namespace std;

// Funcion que ejecutara cada hilo de forma independiente

void tarea(int id) {
    // Registra el instante exacto en que empieza este hilo

    auto inicio = chrono::high_resolution_clock::now();

    // Pausa este hilo en particular durante (100 * id) milisegundos
    //this_thread::sleep_for(chrono::milliseconds(100 * id));

    // Registra el instante en que termina la pausa
    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duracion = fin - inicio;

    // stringstream evita la condicion de carrera en la salida por pantalla
    stringstream ss;
    ss << "tarea " << id
       << " en el hilo " << this_thread::get_id()
       << " tardo " << duracion.count() << " ms\n";

    // Imprime de golpe todo el bloque de texto ya formateado
    cout << ss.str();
}

int main() {
    // Vector para almacenar los objetos std::thread
    vector<thread> hilos;

    for (int i = 1; i <= 3; ++i) {
        // emplace_back(funcion, arg1, arg2...)
        // Construye un std::thread directamente en el vector.
        // Pasa 'tarea' como la funcion a ejecutar y 'i' como el parametro 'id'.
        // El hilo comienza a ejecutarse INMEDIATAMENTE en segundo plano.
        hilos.emplace_back(tarea, i);
    }

    // Bucle para esperar a que terminen los 3 hilos
    // Se usa 'auto&' (referencia) porque std::thread NO se puede copiar

    for (auto& h : hilos) {
        // join() bloquea main() hasta que el hilo 'h' complete la funcion 'tarea'
        h.join();
    }




    // Este punto solo se alcanza cuando los 3 hilos terminaron su join()
    cout << "las tres tareas han terminado\n";
}