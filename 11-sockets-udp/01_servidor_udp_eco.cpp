// =====================================================================
// Tema 11.1  |  Servidor UDP de eco
// En este programa he montado un servidor UDP con socket y bind que recibe datagramas con recvfrom y los devuelve con sendto a quien los envió.
// Requisitos previos: 10-fundamentos-de-redes (IP, puerto, htons, inet_pton)
// Qué observar al ejecutar: cada mensaje llega con la IP y el PUERTO del cliente; dos clientes distintos tienen puertos distintos.
// Salida determinista: NO (depende de cuándo y cuántos clientes envíen; el puerto del cliente lo elige el sistema)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: terminal 1 -> ./build/bin/01_servidor_udp_eco
//                terminal 2 -> ./build/bin/02_cliente_udp_eco   (dos veces)
// Termina solo tras MAX_MENSAJES mensajes o tras SEGUNDOS_INACTIVIDAD sin recibir nada
// (el eco típico de los apuntes es un while(true) infinito: se acota para no dejarlo colgado).
#include <iostream>
#include <string>
#include <cerrno>         // errno, EAGAIN, EWOULDBLOCK: el código del último error
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, bind, recvfrom, sendto, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, ntohs, inet_pton, inet_ntop
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

using namespace std;

// const char*: un texto fijo entre comillas. 127.0.0.1 (localhost) = este mismo equipo
const char* IP_ESCUCHA = "127.0.0.1";  // pon "0.0.0.0" para aceptar mensajes de otros equipos
const int PUERTO = 5100;   // el "número de puerta" dentro del equipo donde espera este programa
const int MAX_MENSAJES = 6;
const int SEGUNDOS_INACTIVIDAD = 30;

int main() {
    // 1) Crear el socket: AF_INET = IPv4, SOCK_DGRAM = datagramas (UDP)
    // Un socket es el "enchufe" por el que el programa envía y recibe por la red.
    // socket devuelve un número entero (descriptor) que lo identifica, o -1 si falla.
    // El 0 final = protocolo por defecto para ese tipo (UDP en este caso).
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");   // muestra "socket: <motivo del error>"
        return 1;           // 1 = terminar con error
    }

    // 2) Opciones: reutilizar el puerto al relanzar, y no esperar para siempre en recvfrom
    int activar = 1;   // 1 = activar la opción
    // setsockopt(socket, nivel, opción, dirección del valor, tamaño del valor).
    // SOL_SOCKET = opción general del socket. SO_REUSEADDR = poder reutilizar el puerto
    // aunque el sistema aún lo tenga reservado de una ejecución anterior.
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));
    timeval espera{};   // timeval guarda un tiempo: tv_sec segundos y tv_usec microsegundos ({} = todo a 0)
    espera.tv_sec = SEGUNDOS_INACTIVIDAD;
    // SO_RCVTIMEO = tiempo máximo esperando datos: pasado ese tiempo, recvfrom devuelve -1
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));

    // 3) bind: reservar IP y puerto. La dirección se rellena en orden de red.
    sockaddr_in direccion{};   // sockaddr_in = dirección IPv4 (familia, puerto e IP); {} la pone a 0
    direccion.sin_family = AF_INET;              // familia: IPv4
    direccion.sin_port = htons(PUERTO);          // puerto en orden de red (htons da la vuelta a sus 2 bytes)
    inet_pton(AF_INET, IP_ESCUCHA, &direccion.sin_addr);   // IP de texto a 4 bytes, escritos en sin_addr
    // Las llamadas de sockets aceptan cualquier familia (IPv4, IPv6...), así que piden un
    // puntero al tipo genérico sockaddr. (sockaddr*) le dice al compilador: "trátalo como tal".
    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // bind asocia el socket a esa IP y puerto: desde aquí, lo que llegue a 127.0.0.1:5100 es para nosotros.
    if (::bind(sock, (sockaddr*) &direccion, sizeof(direccion)) == -1) {
        perror("bind");   // típico: "Address already in use" si otro programa usa ese puerto
        close(sock);      // close libera el socket (como cerrar un fichero)
        return 1;
    }
    cout << "Servidor UDP escuchando en " << IP_ESCUCHA << ":" << PUERTO << "\n";

    // 4) Bucle: recibir un datagrama y devolverlo a quien lo envió
    int atendidos = 0;
    bool seguir = true;   // bandera: se pone a false para salir del bucle (sin break)
    while (seguir && atendidos < MAX_MENSAJES) {
        char buffer[1024];                   // aquí se copian los bytes recibidos
        sockaddr_in cliente{};               // recvfrom lo rellena: quién nos escribe
        socklen_t tam_cliente = sizeof(cliente);   // socklen_t: tipo entero para tamaños de direcciones
        // recvfrom(socket, dónde guardar, cuánto cabe, opciones, dónde apuntar el remitente, su tamaño).
        // Se deja 1 byte libre (sizeof - 1) para poner luego el '\0'. El programa se queda PARADO
        // aquí hasta que llega un datagrama o pasa el timeout. Devuelve los bytes recibidos o -1
        // (por eso ssize_t, un entero de tamaños que admite negativos).
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*) &cliente, &tam_cliente);

        // errno guarda el motivo del último error. EAGAIN o EWOULDBLOCK = "se agotó el tiempo de espera"
        if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            cout << "Nadie escribe desde hace " << SEGUNDOS_INACTIVIDAD << " s: cierro\n";
            seguir = false;
        } else if (n == -1) {
            perror("recvfrom");   // cualquier otro error
            seguir = false;
        } else {
            buffer[n] = '\0';   // recvfrom no añade el fin de cadena
            char ip_cliente[INET_ADDRSTRLEN];   // hueco para el texto de una IPv4
            // inet_ntop: IP del cliente de 4 bytes a texto. ntohs: su puerto del orden de red al de la máquina
            inet_ntop(AF_INET, &cliente.sin_addr, ip_cliente, sizeof(ip_cliente));
            cout << "De " << ip_cliente << ":" << ntohs(cliente.sin_port)
                      << " -> \"" << buffer << "\"\n";

            // Contestar a la MISMA dirección de la que vino: no hay conexión, hay que decir a quién
            // sendto(socket, datos, cuántos bytes, opciones, dirección de destino, su tamaño)
            sendto(sock, buffer, n, 0, (sockaddr*) &cliente, tam_cliente);
            atendidos++;
        }
    }

    cout << "Mensajes atendidos: " << atendidos << "\n";
    close(sock);   // liberar el puerto
    return 0;      // 0 = el programa ha terminado bien
}
