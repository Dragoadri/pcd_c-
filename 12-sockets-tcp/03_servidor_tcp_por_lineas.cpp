// =====================================================================
// Tema 12.3  |  TCP es un flujo: los mensajes llegan pegados o partidos
// En este programa he acumulado lo que trae cada recv en un string y lo he cortado por '\n' para reconstruir las líneas, contestándolas en mayúsculas.
// Requisitos previos: 01_servidor_tcp_eco.cpp, 02_cliente_tcp_eco.cpp
// Qué observar al ejecutar: el primer "trozo" trae tres send del cliente pegados ("uno\ndos\ntr"); aun así salen las líneas uno, dos, tres, FIN.
// Salida determinista: SI en localhost (los trozos dependen de las pausas del cliente; en una red real pueden variar)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/03_servidor_tcp_por_lineas
//                terminal 2 -> ./build/bin/04_cliente_tcp_por_lineas
// PROTOCOLO: cada mensaje es una línea terminada en '\n'. El servidor contesta cada línea
// en mayúsculas. La línea "FIN" cierra la conversación.
#include <iostream>       // cout
#include <string>         // string, find, substr, npos
#include <thread>         // this_thread::sleep_for
#include <chrono>         // milliseconds
#include <cctype>         // toupper
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, bind, listen, accept, send, recv, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // localhost: solo clientes de este mismo equipo
const int PUERTO = 5201;

int main() {
    // socket TCP (AF_INET = IPv4, SOCK_STREAM = flujo de bytes): devuelve su número o -1
    int escucha = socket(AF_INET, SOCK_STREAM, 0);
    int activar = 1;
    // SO_REUSEADDR: poder relanzar el servidor en el mismo puerto enseguida
    setsockopt(escucha, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));
    timeval espera{};   // tiempo: tv_sec segundos, tv_usec microsegundos
    espera.tv_sec = 30;
    // SO_RCVTIMEO: accept (y recv) esperan como mucho 30 s y luego devuelven -1
    setsockopt(escucha, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
    sockaddr_in direccion{};                               // dirección IPv4 donde escuchar
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);                    // htons: puerto en orden de red
    inet_pton(AF_INET, IP_ESCUCHA, &direccion.sin_addr);   // IP de texto a 4 bytes
    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // Con || , si socket ya falló (-1) no se llega a llamar a bind: se entra directamente al if.
    // (sockaddr*) &direccion: la sockaddr_in vista como el tipo genérico que pide bind.
    if (escucha == -1 || ::bind(escucha, (sockaddr*) &direccion, sizeof(direccion)) == -1) {
        perror("socket o bind");
        return 1;
    }
    listen(escucha, 10);   // listen: empezar a aceptar llamadas; hasta 10 esperando en cola
    cout << "Servidor por lineas en el puerto " << PUERTO << "\n";

    // accept: espera a un cliente y devuelve un socket NUEVO para hablar con él.
    // nullptr, nullptr = no me interesa saber la IP ni el puerto del cliente.
    int conexion = accept(escucha, nullptr, nullptr);
    if (conexion == -1) {
        cout << "Ningun cliente en 30 s: cierro\n";
    } else {
        setsockopt(conexion, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));   // recv tampoco espera más de 30 s
        // Medio segundo sin leer: los envíos del cliente se acumulan en el búfer del sistema.
        // En una red real esto ocurre solo, sin pedirlo.
        this_thread::sleep_for(chrono::milliseconds(500));   // dormir este hilo 500 ms

        string pendiente;      // bytes recibidos que aún no forman una línea completa
        bool seguir = true;
        while (seguir) {
            char buffer[1024];
            // recv: devuelve cuántos bytes llegaron (pueden ser varias líneas o media), 0 si el cliente cerró, -1 si error
            ssize_t n = recv(conexion, buffer, sizeof(buffer), 0);
            if (n <= 0) {
                seguir = false;
            } else {
                string trozo(buffer, n);   // los n bytes recibidos convertidos en string
                cout << "recv trajo " << n << " bytes: [" << trozo << "]\n";
                pendiente += trozo;        // += añade el trozo al final de lo pendiente

                // Mientras haya un '\n' en lo pendiente, hay al menos una línea completa
                // find devuelve la posición del primer '\n', o string::npos si no hay ninguno.
                // size_t: entero sin signo para posiciones y tamaños.
                size_t pos = pendiente.find('\n');
                while (seguir && pos != string::npos) {
                    // substr(0, pos): los pos primeros caracteres, es decir, la línea sin el '\n'
                    string linea = pendiente.substr(0, pos);
                    // substr(pos + 1): desde después del '\n' hasta el final
                    pendiente = pendiente.substr(pos + 1);   // quitar la línea y su '\n'
                    cout << "   linea completa: \"" << linea << "\"\n";
                    if (linea == "FIN") {
                        seguir = false;   // pone fin a los dos bucles
                    } else {
                        string respuesta = "";
                        for (size_t i = 0; i < linea.size(); i++) {
                            // toupper pasa una letra a mayúscula; devuelve int, por eso el cast (char)
                            respuesta += (char) toupper(linea[i]);
                        }
                        respuesta += "\n";   // la respuesta también es una línea
                        // send: enviar la respuesta. MSG_NOSIGNAL = si el cliente ya cerró, devuelve -1 en vez de matar el programa
                        send(conexion, respuesta.c_str(), respuesta.size(), MSG_NOSIGNAL);
                        pos = pendiente.find('\n');   // buscar si queda otra línea completa
                    }
                }
            }
        }
        cout << "Conversacion terminada\n";
        close(conexion);   // colgar al cliente: su recv devolverá 0
    }
    close(escucha);   // liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}
