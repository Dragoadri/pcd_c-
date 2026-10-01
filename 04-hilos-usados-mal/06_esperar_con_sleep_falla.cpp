// =====================================================================
// Tema 4.6  |  Coordinar hilos con sleep: en cuanto el trabajo tarda más, falla
// En este programa he hecho que la cocina tarde al azar entre 200 y 800 ms mientras el camarero sigue esperando 500 ms, y salen platos crudos.
// Requisitos previos: 05_esperar_con_sleep_parece_funcionar
// Qué observar al ejecutar: algunos platos salen crudos; cuáles, cambia en cada ejecución.
// Salida determinista: NO (el tiempo de cocina es aleatorio, como la carga real de un ordenador)
// Compilar con: -O0 (target sin optimizar, como todo el tema 04)
// =====================================================================
// Único cambio respecto a 05: la cocina ya no tarda siempre 300 ms, sino entre
// 200 y 800 ms (otro programa usando la CPU, un disco lento, un ordenador más
// viejo que el del profesor...). El camarero sigue esperando 500 ms.
// Calibración: con 5 platos, en 10 de 10 ejecuciones salieron entre 2 y 4 crudos
// (probabilidad de que no salga ninguno: 0,5 elevado a 5, un 3 %).
#include <chrono>       // milliseconds
#include <functional>   // ref
#include <iostream>     // cout
#include <random>       // random_device, mt19937, uniform_int_distribution
#include <thread>       // thread y this_thread

using namespace std;

const int NUM_PLATOS = 5;
const int ESPERA_CAMARERO_MS = 500;

// bool&: referencia al plato_listo de main; el hilo lo pone a true cuando acaba.
void cocinar(bool& plato_listo, int tiempo_cocina_ms) {
    this_thread::sleep_for(chrono::milliseconds(tiempo_cocina_ms));   // simula que cocina
    plato_listo = true;   // aviso de que el plato está hecho
}

int main() {
    random_device semilla;   // número de arranque impredecible que da el sistema
    mt19937 generador(semilla());   // generador de números aleatorios arrancado con la semilla
    uniform_int_distribution<int> tiempo_aleatorio(200, 800);   // enteros de 200 a 800, ambos incluidos

    for (int plato = 1; plato <= NUM_PLATOS; plato++) {
        bool plato_listo = false;
        int tiempo_cocina_ms = tiempo_aleatorio(generador);   // ahora varía

        // plato_listo se comparte con ref; tiempo_cocina_ms basta con copiarlo.
        thread cocinero(cocinar, ref(plato_listo), tiempo_cocina_ms);
        this_thread::sleep_for(chrono::milliseconds(ESPERA_CAMARERO_MS));   // espera fija de 500 ms

        // Si la cocina tarda más de 500 ms, la bandera aún está a false.
        if (plato_listo) {
            cout << "Plato " << plato << ": servido (cocina " << tiempo_cocina_ms << " ms)\n";
        } else {
            cout << "Plato " << plato << ": SERVIDO CRUDO (cocina " << tiempo_cocina_ms << " ms)\n";
        }
        cocinero.join();   // aquí sí espero de verdad, pero el plato ya se ha servido
    }
    return 0;   // 0 = el programa ha terminado bien
}
