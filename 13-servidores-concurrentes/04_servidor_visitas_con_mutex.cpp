// =====================================================================
// Tema 13.4  |  Contador de visitas compartido CON candado
// En este programa he protegido con mtx_visitas.lock y unlock la lectura, la pausa y la escritura del contador visitas, y ya no se pierde ninguna.
// Requisitos previos: 03_servidor_visitas_sin_mutex.cpp, 06-mutex
// Qué observar al ejecutar: "visitas contadas" igual a "conexiones aceptadas" y ninguna línea del libro sin firma, en todas las ejecuciones.
// Salida determinista: SI en los totales (el orden en que firman los clientes sigue cambiando)
// Compilar con: -O0 (igual que su pareja 03, para comparar en las mismas condiciones)
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/04_servidor_visitas_con_mutex
//                terminal 2 -> printf "5302\n50\n" | ./build/bin/02_lanzador_de_clientes_concurrentes
// El candado se mantiene durante la pausa de 1 ms porque leer, trabajar y escribir forman
// UNA operación: si lo soltáramos en medio, volvería la carrera. Precio: se atienden en fila
// durante ese milisegundo (50 ms en total con 50 clientes).
#include <iostream>        // cout
#include <string>          // string, to_string
#include <vector>          // vector
#include <thread>          // thread, this_thread::sleep_for
#include <mutex>           // mutex
#include <chrono>          // milliseconds
#include "red_comun.hpp"   // fichero nuestro: abrir_escucha, configurar_timeout, enviar_todo, recibir_linea

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5302;
const int MAX_CONEXIONES = 50;

// ---- Estado COMPARTIDO por todos los hilos ----
int visitas = 0;                                       // cuántas visitas llevamos
// vector<int> con MAX_CONEXIONES casillas, todas a 0 (0 = línea sin firmar)
vector<int> libro_de_visitas(MAX_CONEXIONES, 0);  // posición i = número de conexión de la visita i+1
mutex mtx_visitas;                                // protege visitas Y libro_de_visitas

void atender_cliente(int conexion, int numero_conexion) {
    configurar_timeout(conexion, 30);   // recv no espera más de 30 s
    string nombre;
    if (recibir_linea(conexion, nombre)) {   // lee la primera línea del cliente y la deja en nombre
        // lock: si otro hilo tiene el candado, este se queda esperando aquí hasta que lo suelte
        mtx_visitas.lock();   // ---- EMPIEZA LA SECCIÓN CRÍTICA ----
        int leido = visitas;
        this_thread::sleep_for(chrono::milliseconds(1));   // dormir con el candado: nadie más lee visitas mientras tanto
        visitas = leido + 1;
        if (leido < MAX_CONEXIONES) {                               // no salirse del vector
            libro_de_visitas[leido] = numero_conexion;              // cada visita en su propia línea
        }
        // unlock: soltar el candado para que entre el siguiente hilo. Si se olvida, los demás esperan para siempre
        mtx_visitas.unlock(); // ---- TERMINA LA SECCIÓN CRÍTICA ----
        // Enviar la respuesta queda FUERA: no toca nada compartido y puede ser lento
        enviar_todo(conexion, "HOLA " + nombre + ", eres la visita " + to_string(leido + 1) + "\n");
    }
    close(conexion);   // colgar a este cliente
}

int main() {
    // (red_comun.hpp) socket + bind + listen, con accept limitado a 30 s. -1 si falla
    int escucha = abrir_escucha(IP_ESCUCHA, PUERTO, 30);
    if (escucha == -1) {
        return 1;
    }
    cout << "Servidor de visitas CON mutex en el puerto " << PUERTO << "\n";

    vector<thread> hilos;   // un hilo por conexión aceptada
    bool seguir = true;
    // (int) convierte size(), que no tiene signo, para compararlo con MAX_CONEXIONES
    while (seguir && (int) hilos.size() < MAX_CONEXIONES) {
        // accept: espera a un cliente y devuelve un socket nuevo para hablar con él
        int conexion = accept(escucha, nullptr, nullptr);
        if (conexion == -1) {
            seguir = false;                      // 30 s sin conexiones nuevas
        } else {
            // Hilo nuevo que ejecuta atender_cliente(conexion, número de conexión); push_back lo guarda en el vector
            hilos.push_back(thread(atender_cliente, conexion, (int) hilos.size() + 1));
        }
    }
    for (size_t i = 0; i < hilos.size(); i++) {
        hilos[i].join();   // esperar a que termine cada hilo antes de leer los resultados
    }

    // Solo main cuenta las conexiones aceptadas: ese número sí es fiable
    int sin_firma = 0;
    for (size_t i = 0; i < hilos.size(); i++) {
        if (libro_de_visitas[i] == 0) {   // nadie escribió en esta línea
            sin_firma++;
        }
    }
    cout << "Conexiones aceptadas: " << hilos.size() << "\n";
    cout << "Visitas contadas:     " << visitas << "\n";
    cout << "Lineas del libro sin firma: " << sin_firma << "\n";
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}
