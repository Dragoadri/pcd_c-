// =====================================================================
// Tema 2.4  |  Multiprogramación: aprovechar la CPU mientras otro espera la E/S
// En este programa he simulado ms a ms dos programas que alternan CPU y espera de E/S en una sola CPU, frente a ejecutarlos uno detrás de otro.
// Requisitos previos: 03_planificador_y_cambio_de_contexto.cpp
// Qué observar al ejecutar: secuencial tarda 160 ms; multiprogramado bastante menos, con una sola CPU.
// Salida determinista: SI (simulación milisegundo a milisegundo con datos fijos)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Figura del Tema 1 presencial: Programa A y Programa B alternan ráfagas de
// "Uso de CPU" y de "Espera" (disco, red...). Mientras uno espera, la CPU
// está libre para el otro. Cada programa: CPU 10 ms, espera 30 ms, CPU 10 ms, espera 30 ms.
#include <iostream>   // cout
#include <vector>     // vector

using namespace std;

int main() {
    // Fases de cada programa (sin struct): duración y si es de CPU o de espera.
    vector<int> duracion_fase = {10, 30, 10, 30};
    vector<bool> fase_es_cpu = {true, false, true, false};   // true = fase de CPU, false = espera
    const int NUM_FASES = 4;

    int secuencial = 2 * (10 + 30 + 10 + 30);  // A entero y después B entero
    cout << "Monoprogramado (A y luego B): " << secuencial << " ms\n";

    // Posición 0 = programa A, posición 1 = programa B.
    vector<int> fase = {0, 0};            // fase actual de A y de B
    vector<int> queda = {10, 10};         // ms que le quedan a esa fase
    int reloj = 0;
    int cpu_ociosa = 0;

    // Cada vuelta del while es 1 ms. Sigue mientras A O B tengan fases pendientes (||).
    while (fase[0] < NUM_FASES || fase[1] < NUM_FASES) {
        bool cpu_ocupada = false;         // en este ms, nadie ha cogido todavía la CPU
        for (int p = 0; p < 2; p++) {
            if (fase[p] < NUM_FASES) {    // este programa aún no ha terminado
                // fase_es_cpu[fase[p]]: primero se lee fase[p] y ese número se usa como posición.
                if (!fase_es_cpu[fase[p]]) {
                    queda[p] = queda[p] - 1;       // la espera avanza sola, sin CPU
                } else if (!cpu_ocupada) {
                    queda[p] = queda[p] - 1;       // este programa usa la única CPU
                    cpu_ocupada = true;
                }
                // Si quiere CPU y ya está ocupada, no entra en ninguna rama: pierde este ms.
                if (queda[p] == 0) {               // ha acabado su fase: pasa a la siguiente
                    fase[p] = fase[p] + 1;
                    if (fase[p] < NUM_FASES) {     // sin esta comprobación leería fuera del vector
                        queda[p] = duracion_fase[fase[p]];
                    }
                }
            }
        }
        if (!cpu_ocupada) {
            cpu_ociosa = cpu_ociosa + 1;
        }
        reloj = reloj + 1;
    }
    cout << "Multiprogramado (intercalando): " << reloj << " ms\n";
    cout << "Milisegundos con la CPU ociosa: " << cpu_ociosa << "\n";
    // No hay paralelismo: nunca hay dos programas en la CPU a la vez.
    // Se gana porque los tiempos de espera se solapan. Esto es CONCURRENCIA.
    return 0;   // 0 = el programa ha terminado bien
}
