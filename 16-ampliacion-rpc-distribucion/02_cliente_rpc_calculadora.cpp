// =====================================================================
// Tema 16.2  |  Cliente RPC: funciones "stub" que parecen locales pero se ejecutan en el servidor (AMPLIACIÓN)
// En este programa he escrito stubs como sumar_remoto, que parecen funciones normales pero mandan la operación al servidor RPC y leen su respuesta.
// Requisitos previos: 01_servidor_rpc_calculadora
// Qué observar al ejecutar: main parece un programa sin red; los resultados los calcula el otro proceso (mira su terminal).
// Salida determinista: SI
// Compilar con: cualquier nivel (solo Linux)
// =====================================================================
// CÓMO LANZARLO: primero ./01_servidor_rpc_calculadora en otra terminal.
// Para usar otro ordenador, cambia IP_SERVIDOR por su IP (ip a en Linux).
//
// Lo que RPC promete y no puede cumplir del todo: una llamada local no
// falla por la red, no tarda segundos y no se ejecuta "a medias". Por eso
// cada stub devuelve bool además del resultado (más en el GUION).
#include <iostream>               // cout
#include <string>                 // string, to_string, stoi
#include "red_utilidades.hpp"     // red_conectar, red_enviar_linea, red_recibir_linea... (y los includes de sockets)

using namespace std;

const string IP_SERVIDOR = "127.0.0.1";   // 127.0.0.1: este mismo ordenador
const int PUERTO = 5600;

int conexion_rpc = -1;   // solo la usan las funciones de este bloque, nunca main

// Empaqueta la llamada, la envía, espera la respuesta y la desempaqueta.
// int& resultado: el resultado se escribe en la variable de quien llama.
bool llamar_remoto(const string& operacion, int a, int b, int& resultado) {
    string respuesta;
    // Tres pasos unidos con &&: enviar, recibir y comprobar. Si uno falla, los siguientes ni se ejecutan.
    bool correcto = red_enviar_linea(conexion_rpc, operacion + " " + to_string(a) + " " + to_string(b)) &&
                    red_recibir_linea(conexion_rpc, respuesta) &&
                    respuesta.rfind("OK ", 0) == 0;          // rfind(..., 0) == 0: ¿empieza por "OK "?
    if (correcto) {
        resultado = stoi(respuesta.substr(3));   // substr(3): el texto desde la posición 3 (sin "OK "); stoi: texto a int
    } else {
        cout << "  (fallo remoto: " << respuesta << ")\n";
    }
    return correcto;
}

// STUBS: misma forma que una función local de calculadora.
bool sumar_remoto(int a, int b, int& resultado) { return llamar_remoto("SUMAR", a, b, resultado); }
bool multiplicar_remoto(int a, int b, int& resultado) { return llamar_remoto("MULTIPLICAR", a, b, resultado); }
bool dividir_remoto(int a, int b, int& resultado) { return llamar_remoto("DIVIDIR", a, b, resultado); }

int main() {
    conexion_rpc = red_conectar(IP_SERVIDOR, PUERTO);   // -1 si no hay ningún servidor escuchando
    if (conexion_rpc < 0) {
        cout << "No hay servidor en " << IP_SERVIDOR << ":" << PUERTO
                  << ". Arranca antes 01_servidor_rpc_calculadora\n";
    } else {
        red_poner_espera_maxima(conexion_rpc, 10);      // si el servidor no responde en 10 s, dejamos de esperar
        int resultado = 0;                              // cada stub lo rellena por referencia

        // A partir de aquí, el código se lee como si todo fuera local.
        if (sumar_remoto(3, 4, resultado)) {
            cout << "3 + 4 = " << resultado << "\n";
        }
        if (multiplicar_remoto(6, 7, resultado)) {
            cout << "6 * 7 = " << resultado << "\n";
        }
        if (dividir_remoto(7, 0, resultado)) {          // el servidor responde "ERROR division por cero": devuelve false
            cout << "7 / 0 = " << resultado << "\n";
        } else {
            cout << "7 / 0 no se pudo calcular\n";
        }

        red_enviar_linea(conexion_rpc, "FIN");          // avisa al servidor de que hemos terminado
        close(conexion_rpc);                            // close: cierra el socket
    }
    return 0;   // 0 = el programa ha terminado bien
}
