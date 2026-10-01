// =====================================================================
// Tema 7.1  |  Recurso limitado sin control: el restaurante sin aforo
// En este programa he dejado entrar a 10 clientes en un restaurante de 4 mesas, con un mutex en el contador, para ver que el mutex no limita el aforo.
// Requisitos previos: 06-mutex (lock/unlock a mano, imprimir con mtx_consola)
// Qué observar al ejecutar: el máximo de mesas ocupadas a la vez supera las 4 que existen.
// Salida determinista: NO (el orden de llegada cambia; el aforo se supera en todas las ejecuciones)
// Compilar con: cualquier nivel (no hay carrera de datos: el contador va protegido)
// =====================================================================
// Material original: Tema 2, ejercicio 2 (restaurante con mesas limitadas).
#include <iostream>   // cout
#include <mutex>      // mutex
#include <string>     // string y to_string
#include <thread>     // thread y this_thread
#include <vector>     // vector
#include <chrono>     // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

const int MESAS = 4;
const int CLIENTES = 10;

mutex mtx_consola;      // protege cout para que no se mezclen las líneas de dos hilos
mutex mtx_ocupacion;    // protege los dos contadores de abajo
int mesas_ocupadas = 0;
int maximo_observado = 0;   // el valor más alto que ha llegado a tener mesas_ocupadas

// const string&: recibe el texto por referencia (sin copiarlo) y sin poder modificarlo.
void imprimir(const string& texto) {
    mtx_consola.lock();       // empieza la sección crítica: solo un hilo escribe a la vez
    cout << texto << "\n";
    mtx_consola.unlock();     // acaba la sección crítica: otro hilo ya puede escribir
}

void cliente(int id) {
    // Entrar: nadie comprueba si queda mesa libre
    mtx_ocupacion.lock();                       // empieza la sección crítica de los contadores
    mesas_ocupadas++;
    if (mesas_ocupadas > maximo_observado) {    // ¿es la ocupación más alta vista hasta ahora?
        maximo_observado = mesas_ocupadas;
    }
    int ocupadas_ahora = mesas_ocupadas;        // copia local para imprimirla fuera del mutex
    mtx_ocupacion.unlock();                     // acaba la sección crítica

    // to_string convierte el número en texto para poder unirlo con +
    imprimir("Cliente " + to_string(id) + " se sienta. Ocupadas: " + to_string(ocupadas_ahora));
    this_thread::sleep_for(chrono::milliseconds(200));   // come: duerme ESTE hilo 200 ms

    mtx_ocupacion.lock();       // se levanta: otra sección crítica corta para restar
    mesas_ocupadas--;
    mtx_ocupacion.unlock();
    imprimir("Cliente " + to_string(id) + " se va");
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
    // Aquí ya no hace falta mutex: todos los hilos han terminado y solo queda main.
    cout << "Mesas que hay: " << MESAS << ". Maximo ocupadas a la vez: " << maximo_observado << "\n";
    if (maximo_observado > MESAS) {
        cout << "ERROR: se ha superado el aforo\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}
