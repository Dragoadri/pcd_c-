// =====================================================================
// Tema 2.3  |  Planificador: FCFS frente a turno rotatorio, y cambios de contexto
// En este programa he planificado P1, P2 y P3 con FCFS y con turno rotatorio de QUANTUM 5 ms, comparando espera media y cambios de contexto.
// Requisitos previos: 02_estados_de_un_proceso.cpp
// Qué observar al ejecutar: con turno rotatorio baja la espera media pero hay más cambios de contexto.
// Salida determinista: SI (simulación con duraciones fijas)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Datos del diagrama de Gantt del Tema 1: P1 20 ms, P2 10 ms, P3 5 ms.
// Una sola CPU: en cada instante SOLO un proceso avanza. Sin struct: vectores paralelos.
#include <iostream>   // cout
#include <string>     // string
#include <vector>     // vector

using namespace std;

int main() {
    // Vectores paralelos: nombres[p] y duracion[p] hablan del mismo proceso.
    vector<string> nombres = {"P1", "P2", "P3"};
    vector<int> duracion = {20, 10, 5};
    const int QUANTUM = 5;  // rodaja de tiempo del turno rotatorio (ms)

    // ---- FCFS: primero en llegar, primero en ser servido, hasta que acaba ----
    int reloj = 0;          // instante simulado, en ms
    int espera_total = 0;
    cout << "FCFS:\n";
    for (int p = 0; p < 3; p++) {
        cout << "  " << reloj << "-" << reloj + duracion[p] << " ms: " << nombres[p] << "\n";
        espera_total = espera_total + reloj;  // ha esperado desde 0 hasta ahora
        reloj = reloj + duracion[p];
    }
    // Divido entre 3.0 (con decimales) y no entre 3, para que no sea división entera.
    cout << "  Espera media: " << espera_total / 3.0 << " ms, cambios de contexto: 2\n";

    // ---- Turno rotatorio (round robin): cada uno como mucho QUANTUM ms ----
    vector<int> restante = duracion;        // "contexto" guardado de cada proceso
    vector<int> fin(3, 0);                  // 3 casillas a 0: instante en que acaba cada proceso
    int terminados = 0;
    int cambios = 0;
    int ultimo = -1;                        // proceso que usó la CPU la última vez (-1 = ninguno aún)
    reloj = 0;
    cout << "Turno rotatorio (quantum " << QUANTUM << " ms):\n";
    while (terminados < 3) {                // se repiten rondas hasta que acaban los tres
        for (int p = 0; p < 3; p++) {       // una ronda: cada proceso pendiente recibe su turno
            if (restante[p] > 0) {          // los que ya han terminado no usan la CPU
                // Hay cambio si ya había alguien antes (distinto de -1) Y ese alguien no es p.
                if (ultimo != -1 && ultimo != p) {
                    cambios = cambios + 1;  // guardar contexto de 'ultimo', cargar el de p
                }
                int rodaja = QUANTUM;
                if (restante[p] < QUANTUM) {
                    rodaja = restante[p];  // si le queda menos, solo usa lo que le queda
                }
                cout << "  " << reloj << "-" << reloj + rodaja << " ms: " << nombres[p]
                          << " (le quedan " << restante[p] - rodaja << " ms)\n";
                reloj = reloj + rodaja;
                restante[p] = restante[p] - rodaja;
                ultimo = p;
                if (restante[p] == 0) {    // acaba de terminar en esta rodaja
                    fin[p] = reloj;
                    terminados = terminados + 1;
                }
            }
        }
    }
    espera_total = 0;
    for (int p = 0; p < 3; p++) {
        espera_total = espera_total + fin[p] - duracion[p];  // espera = fin - lo que ha usado la CPU
    }
    cout << "  Espera media: " << espera_total / 3.0 << " ms, cambios de contexto: " << cambios << "\n";
    // Cada cambio de contexto guarda y restaura registros: tiempo de CPU que
    // no hace trabajo útil. Y el proceso no decide cuándo le quitan la CPU.
    return 0;   // 0 = el programa ha terminado bien
}
