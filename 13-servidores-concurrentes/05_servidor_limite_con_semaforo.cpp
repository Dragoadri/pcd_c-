// =====================================================================
// Tema 13.5  |  Limitar clientes atendidos a la vez con un semáforo
// En este programa he limitado con un counting_semaphore de 3 permisos los clientes atendidos a la vez, rechazando a quien no consigue hueco en 5 s.
// Requisitos previos: 01_servidor_un_hilo_por_conexion.cpp, 07-semaforos (try_acquire_for)
// Qué observar al ejecutar: "atendiendo" nunca pasa de 3; con 10 clientes a la vez, entran de 3 en 3 y uno es rechazado con SERVIDOR LLENO.
// Salida determinista: NO (qué clientes consiguen hueco primero lo decide el planificador; el total suele ser 9 atendidos y 1 rechazado)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/05_servidor_limite_con_semaforo
//                terminal 2 -> printf "5303\n10\n" | ./build/bin/02_lanzador_de_clientes_concurrentes
// Cuentas: 3 huecos, 2 s de trabajo por cliente, 5 s de espera máxima por hueco.
// t=0 entran 3, t=2 entran 3, t=4 entran 3; el décimo lleva 5 s esperando y se rinde.
#include <iostream>        // cout
#include <string>          // string, to_string
#include <vector>          // vector
#include <thread>          // thread, this_thread::sleep_for
#include <mutex>           // mutex
#include <atomic>          // atomic
#include <semaphore>       // counting_semaphore
#include <chrono>          // steady_clock, duration, seconds
#include "red_comun.hpp"   // fichero nuestro: abrir_escucha, configurar_timeout, enviar_todo, recibir_linea

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5303;
const int MAX_CONEXIONES = 10;
const int ATENDIDOS_A_LA_VEZ = 3;
const int SEGUNDOS_ESPERA_HUECO = 5;
// steady_clock::now(): instante en que arranca el programa, para imprimir tiempos relativos a él
const chrono::steady_clock::time_point INICIO = chrono::steady_clock::now();

// counting_semaphore<máximo>(permisos iniciales): cada cliente coge un permiso para trabajar y lo devuelve al acabar
counting_semaphore<ATENDIDOS_A_LA_VEZ> huecos(ATENDIDOS_A_LA_VEZ);  // 3 permisos
atomic<int> atendiendo(0);   // solo para mostrarlo; atomic evita otro candado

mutex mtx_consola;   // candado para que las líneas de varios hilos no se mezclen en pantalla
void imprimir(const string& texto) {
    mtx_consola.lock();     // empieza la sección crítica: solo un hilo escribe a la vez
    // Restar dos instantes da una duración en segundos; count() da el número y (int) quita los decimales
    chrono::duration<double> t = chrono::steady_clock::now() - INICIO;
    cout << "t=" << (int) t.count() << "s " << texto;
    mtx_consola.unlock();   // termina la sección crítica
}

void atender_cliente(int conexion) {
    configurar_timeout(conexion, 30);   // recv no espera más de 30 s
    string nombre;
    if (recibir_linea(conexion, nombre)) {   // lee la primera línea del cliente y la deja en nombre
        // Pedir un hueco, pero sin esperar para siempre
        // try_acquire_for: si hay permiso lo coge y devuelve true; si no, espera hasta 5 s y, si sigue sin haber, devuelve false
        if (huecos.try_acquire_for(chrono::seconds(SEGUNDOS_ESPERA_HUECO))) {
            // ++atendiendo suma 1 de forma atómica y devuelve el valor ya sumado, sin que otro hilo se cuele
            int ahora = ++atendiendo;
            imprimir(nombre + " entra (atendiendo " + to_string(ahora) + " de " +
                     to_string(ATENDIDOS_A_LA_VEZ) + ")\n");
            this_thread::sleep_for(chrono::seconds(2));   // simula el trabajo
            enviar_todo(conexion, "HOLA " + nombre + ", atendido\n");
            atendiendo--;   // resta 1, también de forma atómica
            huecos.release();   // devolver el permiso: si se olvida, el servidor se va llenando para siempre
        } else {
            imprimir(nombre + " rechazado tras " + to_string(SEGUNDOS_ESPERA_HUECO) + " s: servidor lleno\n");
            enviar_todo(conexion, "SERVIDOR LLENO, prueba mas tarde\n");
        }
    }
    close(conexion);   // colgar a este cliente
}

int main() {
    // (red_comun.hpp) socket + bind + listen, con accept limitado a 30 s. -1 si falla
    int escucha = abrir_escucha(IP_ESCUCHA, PUERTO, 30);
    if (escucha == -1) {
        return 1;
    }
    imprimir("Servidor con limite de " + to_string(ATENDIDOS_A_LA_VEZ) + " clientes a la vez, puerto " +
             to_string(PUERTO) + "\n");

    // Se sigue creando un hilo por conexión; el semáforo limita cuántos TRABAJAN a la vez.
    vector<thread> hilos;
    bool seguir = true;   // bandera: a false cuando pasan 30 s sin conexiones nuevas
    // (int) convierte size(), que no tiene signo, para compararlo con MAX_CONEXIONES
    while (seguir && (int) hilos.size() < MAX_CONEXIONES) {
        // accept: espera a un cliente y devuelve un socket nuevo para hablar con él
        int conexion = accept(escucha, nullptr, nullptr);
        if (conexion == -1) {
            seguir = false;
        } else {
            hilos.push_back(thread(atender_cliente, conexion));   // hilo nuevo con atender_cliente(conexion), guardado al final del vector
        }
    }
    for (size_t i = 0; i < hilos.size(); i++) {
        hilos[i].join();   // esperar a que termine cada hilo
    }
    imprimir("Fin del servidor\n");
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}
