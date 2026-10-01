// =====================================================================
// Tema 13.6  |  Servidor de chat: difundir a todos los conectados
// En este programa he guardado en sockets_conectados, protegido con mtx_clientes, a quién está conectado, y he reenviado cada mensaje a los demás.
// Requisitos previos: 01 a 05 de esta carpeta, 06-mutex
// Qué observar al ejecutar: cada cliente ve los mensajes de los otros y los avisos de entrada y salida; el servidor muestra cuántos hay conectados.
// Salida determinista: NO (el orden de los mensajes depende de cuándo habla cada cliente)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/06_servidor_chat_difusion ; terminales 2 a 4 -> ./build/bin/07_cliente_chat
// PROTOCOLO: 1ª línea = nombre; luego mensajes; "SALIR" = desconectar. Esqueleto de la práctica WhatsApp.
#include <iostream>        // cout
#include <string>          // string, to_string
#include <vector>          // vector
#include <thread>          // thread
#include <mutex>           // mutex
#include "red_comun.hpp"   // fichero nuestro: abrir_escucha, configurar_timeout, enviar_todo, recibir_linea

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5304;
const int MAX_CLIENTES = 3;

// ---- Estado COMPARTIDO: quién está conectado. Solo se toca con mtx_clientes ----
vector<int> sockets_conectados;   // el número de socket de cada cliente que está en el chat
mutex mtx_clientes;

mutex mtx_consola;   // candado para que las líneas de varios hilos no se mezclen en pantalla
void imprimir(const string& texto) {   // const string&: el texto sin copiarlo y sin poder cambiarlo
    mtx_consola.lock();
    cout << texto;
    mtx_consola.unlock();
}

// Recorrer la lista mientras otro hilo la cambia (alguien entra o sale) sería una carrera: candado.
void difundir(const string& mensaje, int remitente) {
    mtx_clientes.lock();     // ---- empieza la sección crítica: nadie entra ni sale mientras recorro ----
    for (size_t i = 0; i < sockets_conectados.size(); i++) {
        if (sockets_conectados[i] != remitente) {   // a todos menos a quien lo ha escrito
            enviar_todo(sockets_conectados[i], mensaje);
        }
    }
    mtx_clientes.unlock();   // ---- termina la sección crítica ----
}

void atender_cliente(int conexion) {
    configurar_timeout(conexion, 60);   // un cliente mudo 60 s se da por desconectado
    string nombre;
    if (recibir_linea(conexion, nombre)) {   // la primera línea es el nombre del cliente
        mtx_clientes.lock();     // ---- empieza la sección crítica: añadirlo a la lista ----
        sockets_conectados.push_back(conexion);   // push_back: añadir al final del vector
        int total = sockets_conectados.size();    // leer el tamaño DENTRO del candado, para que sea el de este momento
        mtx_clientes.unlock();   // ---- termina la sección crítica ----
        imprimir(nombre + " entra al chat (conectados: " + to_string(total) + ")\n");
        difundir("*** " + nombre + " se ha unido al chat\n", conexion);

        string linea;
        // Primero intenta leer una línea; solo si lo consigue compara con "SALIR" (&&)
        while (recibir_linea(conexion, linea) && linea != "SALIR") {   // termina si cuelga, calla 60 s o dice SALIR
            difundir("[" + nombre + "] " + linea + "\n", conexion);
        }

        // ---- empieza la sección crítica: quitarlo de la lista ANTES de cerrar, así nadie envía a un socket cerrado ----
        mtx_clientes.lock();
        bool encontrado = false;   // bandera: para dejar de buscar en cuanto aparece (sin break)
        for (size_t i = 0; i < sockets_conectados.size() && !encontrado; i++) {
            if (sockets_conectados[i] == conexion) {
                // begin() + i = posición i del vector; erase borra ese elemento y junta los demás
                sockets_conectados.erase(sockets_conectados.begin() + i);
                encontrado = true;
            }
        }
        total = sockets_conectados.size();
        mtx_clientes.unlock();   // ---- termina la sección crítica ----
        imprimir(nombre + " sale del chat (conectados: " + to_string(total) + ")\n");
        difundir("*** " + nombre + " ha salido del chat\n", conexion);
    }
    close(conexion);   // colgar a este cliente
}

int main() {
    // (red_comun.hpp) socket + bind + listen. -1 si falla
    int escucha = abrir_escucha(IP_ESCUCHA, PUERTO, 30);   // 30 s sin conexiones nuevas: deja de aceptar
    if (escucha == -1) {
        return 1;
    }
    imprimir("Servidor de chat en el puerto " + to_string(PUERTO) + "\n");
    vector<thread> hilos;   // un hilo por cliente conectado
    bool seguir = true;
    // (int) convierte size(), que no tiene signo, para compararlo con MAX_CLIENTES
    while (seguir && (int) hilos.size() < MAX_CLIENTES) {
        // accept: espera a un cliente y devuelve un socket nuevo para hablar con él
        int conexion = accept(escucha, nullptr, nullptr);
        if (conexion == -1) {
            seguir = false;
        } else {
            hilos.push_back(thread(atender_cliente, conexion));   // hilo nuevo con atender_cliente(conexion), guardado en el vector
        }
    }
    for (size_t i = 0; i < hilos.size(); i++) {
        hilos[i].join();   // el chat se cierra cuando han salido todos
    }
    imprimir("Chat cerrado\n");
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}
