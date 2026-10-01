// =====================================================================
// Tema 4.9  |  La optimización esconde la carrera (y la carrera sigue ahí)
// En este programa he compilado con -O2 el mismo contador roto del 02, y he visto que casi siempre da 2000000 aunque la carrera sigue ahí.
// Requisitos previos: 02_contador_compartido_roto
// Qué observar al ejecutar: ahora sale 2000000 (casi siempre), con el MISMO código que en 02.
// Salida determinista: NO (casi siempre 2000000, pero nada lo garantiza; alguna vez sale 1000000)
// Compilar con: -O2 (target optimizado A PROPÓSITO, ver CMakeLists.txt)
// =====================================================================
// Este fichero tiene exactamente el mismo código que 02_contador_compartido_roto.
// Solo cambia cómo se compila. Con -O2 el compilador ve que el bucle suma 1
// un millón de veces y lo convierte en algo equivalente a:
//     int copia = contador;  copia = copia + 1000000;  contador = copia;
// El bucle desaparece, la ventana en la que el otro hilo se cuela es de un
// instante y casi nunca coincide. Pero dos hilos siguen escribiendo la misma
// variable sin protección: sigue siendo un programa incorrecto.
// Por eso los ejemplos del tema 04 se compilan a -O0.
// Calibración (16 núcleos, -O2): GCC 13.3, 249 de 250 repeticiones dan 2 000 000
// y UNA dio 1 000 000 exacto (un hilo sobrescribió todo el trabajo del otro).
// Clang 18: 25 de 25 correctas. "Casi nunca" no es "nunca".
#include <functional>   // ref
#include <iostream>     // cout
#include <thread>       // thread

using namespace std;

const int INCREMENTOS = 1000000;
const int REPETICIONES = 5;

// int&: los dos hilos reciben la MISMA variable contador, no una copia cada uno.
void incrementar(int& contador) {
    for (int i = 0; i < INCREMENTOS; i++) {
        contador++;   // leer, sumar 1, escribir: sin protección entre los dos hilos
    }
}

int main() {
    for (int r = 1; r <= REPETICIONES; r++) {
        int contador = 0;   // contador nuevo, a 0, en cada repetición
        thread hilo_1(incrementar, ref(contador));   // ref: los dos hilos comparten contador
        thread hilo_2(incrementar, ref(contador));
        hilo_1.join();   // espero a los dos hilos antes de leer el resultado
        hilo_2.join();

        cout << "Repeticion " << r << ": contador = " << contador
                  << "  (esperado " << 2 * INCREMENTOS << ", perdidos "
                  << 2 * INCREMENTOS - contador << ")\n";   // perdidos = lo esperado menos lo obtenido
    }
    return 0;   // 0 = el programa ha terminado bien
}
