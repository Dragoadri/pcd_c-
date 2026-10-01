// =====================================================================
// Tema 7.3  |  binary_semaphore: semáforo de un solo permiso
// En este programa he usado un binary_semaphore como candado de un contador compartido y después como aviso que libera un hilo distinto del que espera.
// Requisitos previos: 02_restaurante_con_counting_semaphore.cpp, 06-mutex (contador con mutex)
// Qué observar al ejecutar: el contador llega exacto a 2000000 y otro hilo puede liberar el permiso.
// Salida determinista: SI (el total siempre es 2000000 y el orden de los mensajes está forzado)
// Compilar con: -O0 (mismo target que el contador roto de 04, para comparar en igualdad)
// =====================================================================
// binary_semaphore es exactamente counting_semaphore<1>.
// Diferencia con mutex:
//   - Un mutex tiene DUEÑO: solo el hilo que hizo lock() puede hacer unlock().
//   - Un semáforo NO: cualquier hilo puede hacer release(). Por eso sirve también
//     como SEÑAL entre hilos ("ya puedes seguir"), y un mutex no.
#include <iostream>     // cout
#include <semaphore>    // binary_semaphore (C++20)
#include <thread>       // thread y this_thread
#include <functional>   // ref, para pasar variables por referencia a un hilo
#include <chrono>       // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

binary_semaphore permiso(1);    // 1 = empieza libre

// int&: recibe el contador de main por referencia, así los dos hilos suman sobre la MISMA variable.
void incrementar(int& contador) {
    for (int i = 0; i < 1000000; i++) {
        permiso.acquire();    // entrada a la sección crítica (si el permiso está cogido, espera)
        contador++;           // aquí solo puede estar un hilo a la vez
        permiso.release();    // salida (devuelve el permiso)
    }
}

// binary_semaphore&: el semáforo de main, no una copia (un semáforo no se puede copiar).
void liberar_desde_otro_hilo(binary_semaphore& aviso) {
    this_thread::sleep_for(chrono::milliseconds(500));   // espero medio segundo: main llega antes a acquire
    cout << "Hilo ayudante: libero un permiso que yo no he cogido\n";
    aviso.release();          // suma 1 permiso: main, bloqueado en acquire, despierta
}

int main() {
    // Parte 1: como candado
    int contador = 0;
    // ref(contador): pasa la variable por referencia; sin ref() no compila, porque thread copia los argumentos
    thread a(incrementar, ref(contador));
    thread b(incrementar, ref(contador));
    a.join();                 // espero a que termine el hilo a
    b.join();                 // y después al b
    cout << "Contador: " << contador << " (esperado 2000000)\n";

    // Parte 2: como señal. Empieza a 0: el primer acquire se bloquea.
    binary_semaphore aviso(0);
    thread ayudante(liberar_desde_otro_hilo, ref(aviso));
    cout << "Main: espero el aviso...\n";
    aviso.acquire();          // main se bloquea hasta que el ayudante hace release
    cout << "Main: aviso recibido, sigo\n";
    ayudante.join();
    return 0;   // 0 = el programa ha terminado bien
}
