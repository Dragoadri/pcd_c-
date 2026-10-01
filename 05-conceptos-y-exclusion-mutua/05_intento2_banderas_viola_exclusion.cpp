// =====================================================================
// Tema 5.5  |  Intento 2 de exclusión mutua: mirar antes de señalizar (LECTURA, NO EXAMINABLE)
// En este programa he hecho que cada hilo mire la bandera del otro y DESPUÉS levante la suya, para contar las veces que entran los dos a la vez.
// Requisitos previos: 05/03, LECTURA_cuatro_intentos.md (intentos 1 y 2)
// Qué observar al ejecutar: "veces con DOS hilos dentro" mayor que 0: se viola la exclusión mutua (fallo de seguridad).
// Salida determinista: NO (cambia el número de violaciones, pero aparecen siempre)
// Compilar con: cualquier nivel (las banderas son atomic)
// =====================================================================
// Pseudocódigo (UA1_T04), c[i] = 1 libre, 0 dentro:
//   while c[otro] == 0: {}     <- comprobar
//   c[yo] := 0                 <- señalizar
//   SC ; c[yo] := 1
// TRAMPA 3: la teoría supone memoria secuencialmente consistente, cosa que
// el hardware real no da gratis. Con int normales el compilador podría
// sacar la lectura del bucle de espera (se cuelga) o reordenar escrituras
// (fallaría por otro motivo). Por eso atomic y memory_order_seq_cst.
// El sleep entre comprobar y señalizar abre la ventana A PROPÓSITO para
// que el fallo salga siempre. Espera activa acotada a MAX_VUELTAS.
// Calibración (16 núcleos, GCC 13.3): 20 rondas por hilo, en 6 ejecuciones
// salieron 20 violaciones de 20 las seis veces.
#include <iostream>   // cout
#include <thread>     // thread y this_thread::sleep_for
#include <atomic>     // atomic y memory_order_seq_cst
#include <chrono>     // milliseconds, para indicar cuánto tiempo dormir

using namespace std;

const int RONDAS = 20;
const long MAX_VUELTAS = 50000000;   // límite de la espera activa para que el programa no se cuelgue nunca
atomic<int> c[2];            // array de 2 banderas atómicas: c[0] del proceso 0 y c[1] del proceso 1
atomic<int> dentro(0);       // cuántos hilos hay ahora mismo en la sección crítica
atomic<int> violaciones(0);  // cuántas veces he visto a los dos dentro

void proceso(int yo) {
    int otro = 1 - yo;   // si yo soy 0 el otro es 1, y si yo soy 1 el otro es 0
    for (int ronda = 0; ronda < RONDAS; ronda++) {
        long vueltas = 0;
        // load lee la bandera; memory_order_seq_cst pide que la lectura vea
        // las escrituras en el orden real en que ocurrieron.
        // Sigo esperando mientras el otro esté dentro (0) y no haya agotado las vueltas.
        while (c[otro].load(memory_order_seq_cst) == 0 && vueltas < MAX_VUELTAS) {
            vueltas++;                                           // espera activa
        }
        this_thread::sleep_for(chrono::milliseconds(1));  // ventana abierta: el otro también ve mi bandera a 1
        c[yo].store(0, memory_order_seq_cst);               // señalizo (tarde); store escribe la bandera

        dentro++;                                                // SECCIÓN CRÍTICA (empieza)
        if (dentro.load() > 1) {   // más de uno dentro: exclusión mutua violada
            violaciones++;
        }
        this_thread::sleep_for(chrono::milliseconds(1));   // me quedo un rato dentro
        dentro--;                                                // (acaba la sección crítica)

        c[yo].store(1, memory_order_seq_cst);               // salgo
    }
}

int main() {
    // Las dos banderas empiezan a 1: nadie está dentro.
    c[0].store(1, memory_order_seq_cst);
    c[1].store(1, memory_order_seq_cst);
    thread p1(proceso, 0);   // el 0 se pasa como argumento yo
    thread p2(proceso, 1);
    p1.join();   // join: main espera a que termine cada proceso
    p2.join();
    cout << "Veces con DOS hilos dentro de la seccion critica: " << violaciones << "\n";
    // Operador ternario (condición ? si_cierto : si_falso): elige qué texto imprimir.
    cout << "Exclusion mutua " << (violaciones > 0 ? "VIOLADA" : "respetada (esta vez)") << "\n";
    return 0;   // 0 = el programa ha terminado bien
}
