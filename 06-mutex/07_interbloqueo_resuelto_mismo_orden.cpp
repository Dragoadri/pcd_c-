// =====================================================================
// Tema 6.7  |  Interbloqueo resuelto: todos toman los candados en el mismo orden
// En este programa he hecho que los dos hilos tomen siempre mutex1 antes que mutex2, para que no pueda formarse el ciclo de espera.
// Requisitos previos: 06/06
// Qué observar al ejecutar: los dos hilos consiguen los dos candados y trabajan; uno espera un poco al otro, pero nadie se queda atascado.
// Salida determinista: NO (el orden de los mensajes puede variar; el final siempre es correcto)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Mismo programa que 06/06 con UN solo cambio en main: el hilo B recibe
// los candados en el mismo orden que A. Con un orden global fijo no puede
// formarse el ciclo "yo tengo el tuyo, tú tienes el mío".
// Ya no hace falta timed_mutex: lock() normal y nunca se cuelga.
// Cada mensaje sale con un solo << (línea ya construida): aquí no añadimos
// mtx_consola para no meter un tercer candado en un ejemplo de dos.
#include <iostream>   // cout
#include <string>     // string
#include <thread>     // thread y this_thread::sleep_for
#include <mutex>      // mutex
#include <chrono>     // milliseconds

using namespace std;

mutex mutex1;
mutex mutex2;

// const string& nombre: el texto sin copiarlo. mutex& primero y segundo: los
// candados globales de verdad, recibidos por referencia.
void trabajar(const string& nombre, mutex& primero, mutex& segundo) {
    primero.lock();                                        // ---- cojo el primero
    cout << nombre + " tiene su primer candado\n";
    this_thread::sleep_for(chrono::milliseconds(100));   // el mismo retraso que en 06/06

    // Si el otro hilo tiene mutex1, yo ni siquiera he llegado aquí: estoy
    // esperando en primero.lock(). Por eso nunca espero con un candado del otro.
    segundo.lock();                                        // ---- cojo el segundo
    cout << nombre + " tiene los dos candados y trabaja\n";
    segundo.unlock();

    primero.unlock();   // suelto en orden inverso al que los cogí
}

int main() {
    // ref(...): los hilos reciben los candados globales, no copias.
    thread hilo_a(trabajar, "Hilo A", ref(mutex1), ref(mutex2));
    thread hilo_b(trabajar, "Hilo B", ref(mutex1), ref(mutex2));   // MISMO orden
    hilo_a.join();   // join: main espera a los dos hilos
    hilo_b.join();
    cout << "Finalizado sin interbloqueo\n";
    return 0;   // 0 = el programa ha terminado bien
}
