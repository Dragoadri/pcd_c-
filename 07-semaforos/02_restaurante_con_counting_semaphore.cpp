// =====================================================================
// Tema 7.2  |  counting_semaphore: limitar el aforo
// En este programa he limitado con un counting_semaphore de 4 permisos cuántos clientes se sientan a la vez en el restaurante.
// Requisitos previos: 01_restaurante_sin_limite.cpp
// Qué observar al ejecutar: nunca hay más de 4 ocupadas; los clientes 5 a 10 esperan su turno.
// Salida determinista: NO (quién espera cambia en cada ejecución; el máximo nunca pasa de 4)
// Compilar con: cualquier nivel
// =====================================================================
// El semáforo es un contador de permisos:
//   acquire(): si quedan permisos, resta 1 y sigue; si no, el hilo se BLOQUEA (no gasta CPU).
//   release(): suma 1 y despierta a un hilo que estuviera esperando.
// Cuidado: release() sin haber hecho acquire() puede pasar del máximo, y eso es
// comportamiento indefinido. Cada acquire con su release, igual que lock con unlock.
#include <iostream>    // cout
#include <mutex>       // mutex
#include <semaphore>   // counting_semaphore (C++20)
#include <string>      // string y to_string
#include <thread>      // thread y this_thread
#include <vector>      // vector
#include <chrono>      // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

const int MESAS = 4;
const int CLIENTES = 10;

// <máximo> y (valor inicial): como mucho 4 permisos, y empieza con los 4 disponibles (4 mesas libres)
counting_semaphore<MESAS> mesas_libres(MESAS);

mutex mtx_consola;      // protege cout
mutex mtx_ocupacion;    // protege los dos contadores de abajo (solo sirven para medir)
int mesas_ocupadas = 0;
int maximo_observado = 0;   // el valor más alto que ha llegado a tener mesas_ocupadas

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // empieza la sección crítica: solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();     // acaba la sección crítica
}

void cliente(int id) {
    // to_string convierte el número en texto para poder unirlo con +
    imprimir("Cliente " + to_string(id) + " llega y espera mesa");
    mesas_libres.acquire();                 // aquí se queda parado si no hay mesa (resta 1 permiso)

    mtx_ocupacion.lock();                       // empieza la sección crítica de los contadores
    mesas_ocupadas++;
    if (mesas_ocupadas > maximo_observado) {    // ¿es la ocupación más alta vista hasta ahora?
        maximo_observado = mesas_ocupadas;
    }
    int ocupadas_ahora = mesas_ocupadas;        // copia local para imprimirla fuera del mutex
    mtx_ocupacion.unlock();                     // acaba la sección crítica

    imprimir("Cliente " + to_string(id) + " se sienta. Ocupadas: " + to_string(ocupadas_ahora));
    this_thread::sleep_for(chrono::milliseconds(200));   // come: duerme ESTE hilo 200 ms

    mtx_ocupacion.lock();
    mesas_ocupadas--;
    mtx_ocupacion.unlock();

    imprimir("Cliente " + to_string(id) + " se va");
    mesas_libres.release();                 // deja la mesa para otro (suma 1 permiso y despierta a uno)
}

int main() {
    vector<thread> clientes;    // vector de hilos: guardo los 10 para hacerles join después
    for (int i = 0; i < CLIENTES; i++) {
        // thread(cliente, i) crea el hilo (arranca ya) con id = i; push_back lo añade al final del vector
        clientes.push_back(thread(cliente, i));
    }
    for (int i = 0; i < CLIENTES; i++) {
        clientes[i].join();     // espero a que el cliente i termine
    }
    cout << "Mesas que hay: " << MESAS << ". Maximo ocupadas a la vez: " << maximo_observado << "\n";
    return 0;   // 0 = el programa ha terminado bien
}
