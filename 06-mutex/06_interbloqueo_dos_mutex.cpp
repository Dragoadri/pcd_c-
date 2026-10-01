// =====================================================================
// Tema 6.6  |  Interbloqueo con dos candados tomados en orden opuesto
// En este programa he hecho que el hilo A tome mutex1 y luego mutex2, y el hilo B al revés, para provocar un interbloqueo y detectarlo con try_lock_for.
// Requisitos previos: 06/05, 05 (interbloqueo como fallo de vivacidad)
// Qué observar al ejecutar: cada hilo tiene SU candado y espera el del otro; los dos detectan el interbloqueo tras 1 s y renuncian.
// Salida determinista: NO (en teoría; con el sleep de 100 ms el interbloqueo sale siempre)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Port del ejemplo del Tema 2 (Python, Lock.acquire(timeout=2)).
//   Hilo A: toma mutex1 ... quiere mutex2
//   Hilo B: toma mutex2 ... quiere mutex1      -> ciclo de espera
// Cambio respecto al material: con lock() normal el programa se quedaría
// colgado. Usamos timed_mutex y try_lock_for para DETECTAR el
// interbloqueo e informar. Tras detectarlo cada hilo espera 200 ms antes de
// soltar su candado, para que los dos lleguen a detectarlo.
// Ojo: renunciar no es una solución (nadie hace su trabajo); la solución
// está en 06/07.
// Cada mensaje sale con un solo << (línea ya construida): aquí no añadimos
// mtx_consola para no meter un tercer candado en un ejemplo de dos.
#include <iostream>   // cout
#include <string>     // string
#include <thread>     // thread y this_thread::sleep_for
#include <mutex>      // timed_mutex
#include <chrono>     // milliseconds y seconds

using namespace std;

timed_mutex mutex1;
timed_mutex mutex2;

// const string& nombre: el texto sin copiarlo. timed_mutex& primero y segundo:
// por referencia, porque tienen que ser los candados globales de verdad.
void trabajar(const string& nombre, timed_mutex& primero, timed_mutex& segundo) {
    primero.lock();                                        // cojo mi primer candado
    cout << nombre + " tiene su primer candado\n";
    this_thread::sleep_for(chrono::milliseconds(100));   // da tiempo al otro

    // try_lock_for: intento coger el segundo durante 1 s; true si lo consigo, false si no.
    if (segundo.try_lock_for(chrono::seconds(1))) {
        cout << nombre + " tiene los dos candados y trabaja\n";   // aquí tengo los dos
        segundo.unlock();
    } else {
        // No tengo el segundo: el otro hilo lo tiene y a su vez espera el mío.
        cout << nombre + ": 1 s esperando el segundo candado, interbloqueo detectado\n";
        this_thread::sleep_for(chrono::milliseconds(200));   // aguanto el mío para que el otro también lo detecte
    }
    primero.unlock();   // suelto el primero en los dos caminos
}

int main() {
    // ref(...): los hilos reciben los candados globales, no copias (un mutex no se puede copiar).
    thread hilo_a(trabajar, "Hilo A", ref(mutex1), ref(mutex2));
    thread hilo_b(trabajar, "Hilo B", ref(mutex2), ref(mutex1));   // orden opuesto
    hilo_a.join();   // join: main espera a los dos hilos
    hilo_b.join();
    cout << "Finalizado\n";
    return 0;   // 0 = el programa ha terminado bien
}
