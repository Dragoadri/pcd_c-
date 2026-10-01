// =====================================================================
// Tema 4.7  |  Espera activa: preguntar sin parar "¿ya?" quema un núcleo entero
// En este programa he esperado a un hilo de 3 s primero con un bucle que pregunta sin parar y luego con join, y he medido la CPU gastada.
// Requisitos previos: 03-hilos-creacion, 06_esperar_con_sleep_falla
// Qué observar al ejecutar: millones de vueltas y ~3 s de CPU en la espera activa; casi 0 s con join.
// Salida determinista: NO (el número de vueltas depende de la velocidad de la máquina)
// Compilar con: -O0 (target sin optimizar, como todo el tema 04)
// =====================================================================
// Mientras corre la primera parte, abre otra terminal con 'top' o 'htop':
// el programa aparece al 100 % de un núcleo.
// atomic<bool> es un bool que se puede leer y escribir desde varios
// hilos sin carrera (se explica en la carpeta 06). Se usa aquí para no mezclar
// dos problemas: con un bool normal y -O2, el compilador podría sacar la lectura
// fuera del bucle y el programa no terminaría nunca.
// clock() mide el tiempo de CPU que ha gastado el proceso, no el de reloj.
// Medido (16 núcleos, GCC 13.3, -O0): ~640 millones de vueltas y 3,0 s de CPU
// con espera activa, frente a 0,0001 s de CPU esperando en join.
#include <atomic>       // atomic
#include <chrono>       // seconds
#include <ctime>        // clock, clock_t y CLOCKS_PER_SEC
#include <functional>   // ref
#include <iostream>     // cout
#include <thread>       // thread y this_thread

using namespace std;

// atomic<bool>&: referencia a la bandera de main, compartida sin carrera.
void trabajar_3_segundos(atomic<bool>& terminado) {
    this_thread::sleep_for(chrono::seconds(3));   // simula un trabajo de 3 segundos
    terminado = true;   // en un atomic, = equivale a store(true): escritura segura
}

int main() {
    // ---- 1. Espera activa (mal) ----
    atomic<bool> terminado(false);   // bandera que empieza a false
    clock_t cpu_inicio = clock();    // CPU gastada hasta ahora por el proceso
    thread hilo_1(trabajar_3_segundos, ref(terminado));

    long long vueltas = 0;
    // !terminado lee el atomic (equivale a load()) y significa "todavía no ha terminado".
    while (!terminado) {   // "¿ya? ¿ya? ¿ya?..." millones de veces por segundo
        vueltas++;   // no hace nada útil: solo cuenta las preguntas
    }
    hilo_1.join();
    // clock() - cpu_inicio = CPU gastada en ticks; double(...) evita perder decimales
    // al dividir, y dividir entre CLOCKS_PER_SEC pasa los ticks a segundos.
    double cpu_segundos = double(clock() - cpu_inicio) / CLOCKS_PER_SEC;
    cout << "Espera activa: " << vueltas << " vueltas, " << cpu_segundos << " s de CPU\n";

    // ---- 2. Esperar bloqueado en join (el sistema no nos da CPU hasta que acabe) ----
    cpu_inicio = clock();
    atomic<bool> terminado_2(false);
    thread hilo_2(trabajar_3_segundos, ref(terminado_2));
    hilo_2.join();   // main se bloquea 3 s sin gastar CPU
    cpu_segundos = double(clock() - cpu_inicio) / CLOCKS_PER_SEC;
    cout << "Espera con join: " << cpu_segundos << " s de CPU\n";
    return 0;   // 0 = el programa ha terminado bien
}
