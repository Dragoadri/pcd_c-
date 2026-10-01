// =====================================================================
// Tema 12.1  |  Servidor TCP de eco (un cliente)
// En este programa he montado un servidor TCP con socket, bind, listen y accept que atiende a un cliente y le devuelve cada mensaje con send.
// Requisitos previos: 11-sockets-udp (01_servidor_udp_eco.cpp)
// Qué observar al ejecutar: dos descriptores distintos (el de escucha y el del cliente) y el eco de cada mensaje.
// Salida determinista: NO (el puerto del cliente lo elige el sistema)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/01_servidor_tcp_eco
//                terminal 2 -> ./build/bin/02_cliente_tcp_eco
// Atiende a UN cliente y termina. Si en 30 s no llega nadie (o el cliente no habla), termina igual.
#include <iostream>       // cout
#include <string>         // string
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, bind, listen, accept, send, recv, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, ntohs, inet_pton, inet_ntop
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

const char* IP_ESCUCHA = "127.0.0.1";   // "0.0.0.0" para aceptar clientes de otros equipos
const int PUERTO = 5200;                // el puerto donde esperamos a los clientes
const int SEGUNDOS_INACTIVIDAD = 30;

int main() {
    // 1) socket: SOCK_STREAM = flujo de bytes (TCP)
    // AF_INET = IPv4. Devuelve un número (descriptor) que identifica el socket, o -1 si falla.
    // Este socket solo sirve para ESPERAR llamadas; para hablar con el cliente habrá otro.
    int escucha = socket(AF_INET, SOCK_STREAM, 0);
    if (escucha == -1) {
        perror("socket");   // imprime "socket: <motivo del error>"
        return 1;
    }
    int activar = 1;
    // SO_REUSEADDR: poder relanzar el servidor en el mismo puerto sin esperar a que el sistema lo libere
    setsockopt(escucha, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));
    timeval espera{};   // tiempo: tv_sec segundos y tv_usec microsegundos ({} = todo a 0)
    espera.tv_sec = SEGUNDOS_INACTIVIDAD;
    // SO_RCVTIMEO: tiempo máximo de espera; en el socket de escucha también afecta a accept
    setsockopt(escucha, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));  // accept no espera para siempre

    // 2) bind: igual que en UDP
    sockaddr_in direccion{};                               // dirección IPv4: familia, puerto e IP
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(PUERTO);                    // htons: puerto en orden de red
    inet_pton(AF_INET, IP_ESCUCHA, &direccion.sin_addr);   // IP de texto a 4 bytes
    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // (sockaddr*) &direccion: nuestra sockaddr_in vista como el tipo genérico sockaddr que pide bind
    if (::bind(escucha, (sockaddr*) &direccion, sizeof(direccion)) == -1) {
        perror("bind");
        close(escucha);   // liberar el socket antes de salir
        return 1;
    }

    // 3) listen: "acepto llamadas". 10 = cuántas pueden esperar en cola sin atender
    listen(escucha, 10);
    cout << "Servidor TCP en " << IP_ESCUCHA << ":" << PUERTO << " (socket de escucha = " << escucha << ")\n";

    // 4) accept: espera a un cliente y devuelve OTRO socket para hablar solo con él
    sockaddr_in cliente{};             // accept lo rellena con la IP y el puerto del cliente
    socklen_t tam = sizeof(cliente);   // socklen_t: tipo entero para tamaños de direcciones
    // El programa se queda PARADO aquí hasta que un cliente conecta (o pasan 30 s: devuelve -1)
    int conexion = accept(escucha, (sockaddr*) &cliente, &tam);
    if (conexion == -1) {
        cout << "Ningun cliente en " << SEGUNDOS_INACTIVIDAD << " s (o error): cierro\n";
    } else {
        char ip[INET_ADDRSTRLEN];   // hueco para el texto de una IPv4 (16 caracteres)
        // inet_ntop: IP del cliente a texto. ntohs: su puerto del orden de red al de la máquina
        inet_ntop(AF_INET, &cliente.sin_addr, ip, sizeof(ip));
        cout << "Cliente " << ip << ":" << ntohs(cliente.sin_port)
                  << " conectado (socket de conexion = " << conexion << ")\n";
        // El timeout se pone también en el socket de conexión, que es donde haremos recv
        setsockopt(conexion, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));

        // 5) Eco: devolver lo que llegue hasta que el cliente cuelgue (recv devuelve 0)
        bool conectado = true;   // bandera para salir del bucle sin break
        while (conectado) {
            char buffer[1024];
            // recv(socket, dónde guardar, cuánto cabe, opciones): espera bytes del cliente.
            // Devuelve cuántos bytes llegaron (ssize_t, entero que admite el -1).
            ssize_t n = recv(conexion, buffer, sizeof(buffer), 0);
            if (n <= 0) {
                conectado = false;   // 0 = el cliente cerró; -1 = error o 30 s en silencio
            } else {
                // string(buffer, n): texto con los n primeros bytes del buffer (no hace falta '\0')
                cout << "Recibidos " << n << " bytes: " << string(buffer, n) << "\n";
                // MSG_NOSIGNAL: si el cliente ya ha colgado, send devuelve -1 en vez de matar el programa
                // Ojo: send puede enviar MENOS bytes de los pedidos. En 05 lo arreglamos con enviar_todo.
                // send(socket, datos, cuántos bytes, opciones): en TCP no se dice a quién, ya hay conexión
                send(conexion, buffer, n, MSG_NOSIGNAL);
            }
        }
        cout << "El cliente ha colgado\n";
        close(conexion);   // colgar la conexión con este cliente
    }
    close(escucha);   // dejar de escuchar y liberar el puerto
    return 0;         // 0 = el programa ha terminado bien
}
