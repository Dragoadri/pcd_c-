// =====================================================================
// Tema 16.4  |  Trabajador: proceso que recibe encargos por red, calcula y devuelve el resultado (AMPLIACIÓN)
// En este programa he hecho un trabajador que se conecta al maestro, recibe "CONTAR_PRIMOS desde hasta", cuenta con es_primo y responde "RESULTADO n".
// Requisitos previos: 03_maestro_reparte_trabajo
// Qué observar al ejecutar: su PID, el rango que le toca y cuántos primos encuentra; varios trabajadores calculan a la vez.
// Salida determinista: SI en el resultado de cada rango; NO en qué rango le toca a cada trabajador
// Compilar con: cualquier nivel (solo Linux)
// =====================================================================
// CÓMO LANZARLO: primero ./03_maestro_reparte_trabajo, después uno o varios
// ./04_trabajador_remoto. Si el maestro aún no está, reintenta 10 veces
// cada medio segundo y luego se rinde (nunca se queda colgado).
// Protocolo: recibe "CONTAR_PRIMOS desde hasta", responde "RESULTADO n"; "FIN" termina.
#include <iostream>               // cout
#include <string>                 // string, to_string
#include <sstream>                // istringstream
#include <thread>                 // this_thread::sleep_for
#include <chrono>                 // milliseconds
#include <unistd.h>               // getpid
#include "red_utilidades.hpp"     // red_conectar, red_enviar_linea, red_recibir_linea... (y los includes de sockets)

using namespace std;

const string IP_MAESTRO = "127.0.0.1";   // en el aula: la IP del ordenador del profesor
const int PUERTO = 5603;

bool es_primo(int numero) {
    bool primo = numero >= 2;           // 0 y 1 no son primos
    // Basta con probar divisores mientras divisor * divisor no pase de numero (hasta la raíz cuadrada).
    for (int divisor = 2; primo && divisor * divisor <= numero; divisor++) {
        if (numero % divisor == 0) {    // %: resto; si es 0, divisor lo divide y no es primo
            primo = false;              // la condición del for corta el bucle, sin break
        }
    }
    return primo;
}

int contar_primos(int desde, int hasta) {
    int cuenta = 0;
    for (int numero = desde; numero < hasta; numero++) {
        if (es_primo(numero)) {
            cuenta++;
        }
    }
    return cuenta;
}

int main() {
    string yo = "Trabajador PID " + to_string(getpid());   // getpid: PID de este proceso, para distinguir trabajadores
    int conexion = -1;                                     // -1: todavía sin conectar
    // Hasta 10 intentos; en cuanto conecta, conexion deja de ser negativa y el for se para.
    for (int intento = 1; intento <= 10 && conexion < 0; intento++) {
        conexion = red_conectar(IP_MAESTRO, PUERTO);
        if (conexion < 0) {
            this_thread::sleep_for(chrono::milliseconds(500));   // sleep_for: espera medio segundo antes de reintentar
        }
    }

    if (conexion < 0) {
        cout << yo + ": no encuentro al maestro en el puerto " + to_string(PUERTO) + "\n";
    } else {
        red_poner_espera_maxima(conexion, 60);   // si el maestro no manda nada en 60 s, dejamos de esperar
        cout << yo + ": conectado, esperando encargos\n" << flush;   // flush: que se vea ya
        string encargo;
        bool seguir = true;                      // bandera: "FIN" la pone a false
        while (seguir && red_recibir_linea(conexion, encargo)) {
            istringstream lector(encargo);       // istringstream: permite leer del string con >> como de cin
            string orden;
            int desde = 0;
            int hasta = 0;
            if (encargo == "FIN") {
                seguir = false;
            } else if (lector >> orden >> desde >> hasta && orden == "CONTAR_PRIMOS") {   // se leen las tres partes Y la orden es la esperada
                int cuenta = contar_primos(desde, hasta);
                cout << yo + ": [" + to_string(desde) + ", " + to_string(hasta) + ") tiene "
                             + to_string(cuenta) + " primos\n" << flush;
                red_enviar_linea(conexion, "RESULTADO " + to_string(cuenta));
            } else {
                red_enviar_linea(conexion, "ERROR encargo no entendido");
            }
        }
        close(conexion);                         // close: cierra la conexión con el maestro
        cout << yo + ": termino\n";
    }
    return 0;   // 0 = el programa ha terminado bien
}
