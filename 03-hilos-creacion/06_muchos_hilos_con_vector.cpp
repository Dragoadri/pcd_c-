// =====================================================================
// Tema 3.6  |  Muchos hilos guardados en un vector
// En este programa he creado 5 hilos con un bucle, los he guardado en un vector<thread> y después los he esperado a todos con otro bucle.
// Requisitos previos: 03_hilo_con_argumentos
// Qué observar al ejecutar: cada hilo tiene su propio id y no terminan en el orden en que se crearon.
// Salida determinista: NO (los ids cambian en cada ejecución y el planificador decide el orden)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Es exactamente el esquema cobegin/coend de UA1_T01 traducido a C++:
//   primer bucle  = fork de cada tarea
//   segundo bucle = join de cada tarea (coend)
#include <chrono>     // milliseconds
#include <iostream>   // cout
#include <sstream>    // ostringstream, para montar un texto por partes
#include <string>     // string
#include <thread>     // thread y this_thread
#include <vector>     // vector

using namespace std;

const int NUM_HILOS = 5;

void tarea(int id) {
    // Los hilos con id alto duermen menos: suelen acabar antes que los primeros.
    // Con NUM_HILOS = 5: la tarea 0 duerme 500 ms y la tarea 4 solo 100 ms.
    this_thread::sleep_for(chrono::milliseconds(100 * (NUM_HILOS - id)));
    // this_thread::get_id() da el identificador del hilo que ejecuta esta línea.
    ostringstream linea;   // construimos la línea entera y la imprimimos de una vez
    linea << "tarea " << id << " terminada (id del hilo " << this_thread::get_id() << ")\n";
    cout << linea.str();   // str() devuelve todo el texto acumulado en linea
}

int main() {
    cout << "[main] mi id es " << this_thread::get_id() << "\n";
    vector<thread> hilos;   // vector vacío donde iré guardando los hilos

    // cobegin: crear y lanzar
    for (int i = 0; i < NUM_HILOS; i++) {
        // thread(tarea, i) crea el hilo (ya arranca) y push_back lo mete al final del vector.
        hilos.push_back(thread(tarea, i));
    }
    cout << "[main] he lanzado " << hilos.size() << " hilos\n";   // size(): cuántos hay en el vector

    // coend: esperar a todos (se esperan en orden, aunque terminen desordenados)
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i].join();   // hilos[i]: el hilo guardado en la posición i
    }

    cout << "[main] las " << NUM_HILOS << " tareas han terminado\n";
    return 0;   // 0 = el programa ha terminado bien
}
