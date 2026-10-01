// =====================================================================
// Tema 2.2  |  Estados de un proceso y sus transiciones
// En este programa he aplicado una lista fija de sucesos a un proceso con aplicar_suceso, imprimiendo cada cambio y rechazando los no permitidos.
// Requisitos previos: 01_automata_frente_a_ordenador.cpp
// Qué observar al ejecutar: al desbloquearse, el proceso vuelve a PREPARADO, no directamente a EJECUCION.
// Salida determinista: SI (secuencia de sucesos fija)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Diagrama del Tema 1 presencial y de UA1_T02:
//   NUEVO --crear--> PREPARADO --planificacion--> EJECUCION --terminar--> TERMINADO
//   EJECUCION --expropiado--> PREPARADO     (se acabó su rodaja de tiempo)
//   EJECUCION --espera E/S--> BLOQUEADO     (pide algo que tarda: disco, red, teclado)
//   BLOQUEADO --ocurre suceso--> PREPARADO  (vuelve a la cola, NO a la CPU)
#include <iostream>   // cout
#include <string>     // string, y comparar textos con ==
#include <vector>     // vector

using namespace std;

// Devuelve el nuevo estado, o el mismo si la transición no está permitida.
// const string& : recibe los textos sin copiarlos y sin poder modificarlos.
string aplicar_suceso(const string& estado, const string& suceso) {
    string nuevo = estado;   // por defecto el estado no cambia
    // Cada condición exige DOS cosas a la vez (&&): estar en ese estado Y recibir ese suceso.
    if (estado == "NUEVO" && suceso == "crear") {
        nuevo = "PREPARADO";
    } else if (estado == "PREPARADO" && suceso == "planificacion") {
        nuevo = "EJECUCION";
    } else if (estado == "EJECUCION" && suceso == "expropiado") {
        nuevo = "PREPARADO";
    } else if (estado == "EJECUCION" && suceso == "espera E/S") {
        nuevo = "BLOQUEADO";
    } else if (estado == "BLOQUEADO" && suceso == "ocurre suceso") {
        nuevo = "PREPARADO";
    } else if (estado == "EJECUCION" && suceso == "terminar") {
        nuevo = "TERMINADO";
    } else {
        // Ninguna flecha del diagrama encaja: nuevo se queda igual que estado.
        cout << "  (transicion NO permitida desde " << estado << ")\n";
    }
    return nuevo;
}

int main() {
    // La lista de sucesos ocupa tres líneas, pero es una sola instrucción (acaba en ;).
    vector<string> sucesos = {"crear", "planificacion", "expropiado",
        "planificacion", "espera E/S", "planificacion", "ocurre suceso",
        "planificacion", "terminar"};
    string estado = "NUEVO";

    // (int) sucesos.size(): número de sucesos convertido a int para compararlo con i.
    for (int i = 0; i < (int) sucesos.size(); i++) {
        string anterior = estado;   // lo guardo antes de cambiarlo para poder imprimir los dos
        estado = aplicar_suceso(estado, sucesos[i]);
        cout << anterior << " --" << sucesos[i] << "--> " << estado << "\n";
    }
    // El sexto suceso ("planificacion" estando BLOQUEADO) se rechaza: un proceso
    // bloqueado no puede ejecutarse hasta que ocurra lo que espera.
    // Consecuencia para hilos: nunca sabes cuándo volverá a ejecutarse uno.
    return 0;   // 0 = el programa ha terminado bien
}
