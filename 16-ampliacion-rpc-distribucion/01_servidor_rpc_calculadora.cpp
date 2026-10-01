// =====================================================================
// Tema 16.1  |  Servidor RPC: recibe "SUMAR 3 4" y llama a una función normal (AMPLIACIÓN)
// En este programa he hecho un servidor RPC que recibe por TCP líneas como "SUMAR 3 4", llama a la función local que toca y responde "OK resultado".
// Requisitos previos: 12-sockets-tcp, 13-servidores-concurrentes, red_utilidades.hpp
// Qué observar al ejecutar: cada petición que llega, qué función se ejecuta y qué respuesta se devuelve.
// Salida determinista: SI (depende solo de las peticiones que envíe el cliente)
// Compilar con: cualquier nivel (solo Linux)
// =====================================================================
// CÓMO LANZARLO (dos terminales, desde la carpeta build/bin):
//   Terminal 1:  ./01_servidor_rpc_calculadora
//   Terminal 2:  ./02_cliente_rpc_calculadora
// El servidor atiende hasta 3 clientes, UNO DETRÁS DE OTRO, y termina solo
// tras 30 s sin clientes. Versión concurrente: un hilo por conexión (carpeta 13).
//
// Protocolo:  petición "OPERACION a b"   respuesta "OK resultado" o "ERROR motivo"
//             "FIN" cierra la conexión.
#include <iostream>               // cout
#include <string>                 // string, to_string
#include <sstream>                // istringstream
#include "red_utilidades.hpp"     // red_crear_servidor, red_recibir_linea, red_enviar_linea... (y los includes de sockets)

using namespace std;

const int PUERTO = 5600;
const int MAX_CLIENTES = 3;

// Funciones NORMALES: no saben nada de redes.
int sumar(int a, int b) { return a + b; }
int restar(int a, int b) { return a - b; }
int multiplicar(int a, int b) { return a * b; }

// "Esqueleto" del servidor: desempaqueta la petición y llama a la función que toca.
string ejecutar_peticion(const string& peticion) {
    istringstream lector(peticion);   // istringstream: permite leer de un string con >> igual que de cin
    string operacion;
    int a = 0;
    int b = 0;
    string respuesta;
    // Lee una palabra y dos enteros. Si algo no encaja la lectura falla, y el ! convierte ese fallo en true.
    if (!(lector >> operacion >> a >> b)) {
        respuesta = "ERROR formato, se esperaba: OPERACION numero numero";
    } else if (operacion == "SUMAR") {
        respuesta = "OK " + to_string(sumar(a, b));   // to_string: número a texto para pegarlo con +
    } else if (operacion == "RESTAR") {
        respuesta = "OK " + to_string(restar(a, b));
    } else if (operacion == "MULTIPLICAR") {
        respuesta = "OK " + to_string(multiplicar(a, b));
    } else if (operacion == "DIVIDIR" && b == 0) {    // va antes que la división normal para no dividir nunca entre 0
        respuesta = "ERROR division por cero";
    } else if (operacion == "DIVIDIR") {
        respuesta = "OK " + to_string(a / b);         // división entera: 7 / 2 da 3
    } else {
        respuesta = "ERROR operacion desconocida: " + operacion;
    }
    return respuesta;
}

int main() {
    int servidor = red_crear_servidor(PUERTO, 30);   // socket de escucha; accept dará -1 tras 30 s sin clientes
    int atendidos = 0;
    bool seguir = (servidor >= 0);                   // si el puerto no se pudo abrir, ni se entra en el bucle
    cout << "Servidor RPC calculadora en el puerto " << PUERTO << "\n" << flush;   // flush: que se vea ya

    // Para cuando accept falla por la espera o cuando ya se han atendido MAX_CLIENTES.
    while (seguir && atendidos < MAX_CLIENTES) {
        // accept: espera a un cliente y devuelve un socket nuevo solo para él. nullptr: no nos interesa su dirección.
        int cliente = accept(servidor, nullptr, nullptr);
        if (cliente < 0) {                           // -1: han pasado 30 s sin clientes
            cout << "30 s sin clientes: el servidor termina\n";
            seguir = false;
        } else {
            atendidos++;
            red_poner_espera_maxima(cliente, 30);    // si el cliente se calla 30 s, dejamos de esperarle
            cout << "Cliente " << atendidos << " conectado\n" << flush;
            string peticion;
            bool conectado = true;
            // Sigue mientras no llegue "FIN" y la conexión siga entregando líneas.
            while (conectado && red_recibir_linea(cliente, peticion)) {
                if (peticion == "FIN") {
                    conectado = false;
                } else {
                    string respuesta = ejecutar_peticion(peticion);
                    cout << "  recibido [" << peticion << "] -> respondo [" << respuesta << "]\n" << flush;
                    red_enviar_linea(cliente, respuesta);
                }
            }
            close(cliente);                          // cierra la conexión con ESTE cliente; el servidor sigue escuchando
            cout << "Cliente " << atendidos << " desconectado\n" << flush;
        }
    }
    if (servidor >= 0) {
        close(servidor);                             // cierra el socket de escucha
    }
    return 0;   // 0 = el programa ha terminado bien
}
