// =====================================================================
// red_comun.hpp  |  Funciones de red reutilizadas en la carpeta 13
//
// Aquí NO hay nada nuevo: es lo que ya escribimos paso a paso en
// 12-sockets-tcp (socket, bind, listen, connect, send, recv, SO_RCVTIMEO),
// recogido en funciones libres para que los ejemplos de esta carpeta se
// centren en la concurrencia: hilos, candados y semáforos.
//
// Todos los ficheros .cpp de la carpeta lo usan con:  #include "red_comun.hpp"
//
// Las funciones llevan 'inline' porque están DEFINIDAS en un fichero de
// cabecera: así se podría incluir desde varios .cpp de un mismo programa
// sin que el enlazador protestase por definiciones repetidas.
//
// Protocolo común de la carpeta: mensajes de texto terminados en '\n'.
// =====================================================================
#pragma once   // si un .cpp incluye esta cabecera dos veces, el compilador solo la lee la primera

#include <string>          // string
#include <cstdio>          // perror
#include <sys/socket.h>    // socket, bind, listen, connect, send, recv, setsockopt
#include <netinet/in.h>    // sockaddr_in
#include <arpa/inet.h>     // htons, inet_pton
#include <sys/time.h>      // timeval
#include <unistd.h>        // close

// En una cabecera, using namespace std se desaconseja en proyectos reales porque se impone a quien la incluya.
// Aquí la incluyen solo los ejemplos del curso, que ya lo usan.
using namespace std;

// Tiempo máximo que esperarán recv y accept en este socket (luego devuelven -1).
// fd = el número (descriptor) del socket.
inline void configurar_timeout(int fd, int segundos) {
    timeval espera{};          // tiempo: tv_sec segundos y tv_usec microsegundos ({} = todo a 0)
    espera.tv_sec = segundos;
    // setsockopt(socket, nivel, opción, dirección del valor, tamaño). SO_RCVTIMEO = tiempo máximo de espera al recibir
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
}

// socket + SO_REUSEADDR + bind + listen, con timeout en accept.
// Devuelve el socket de escucha o -1 si algo falla.
inline int abrir_escucha(const char* ip, int puerto, int segundos_timeout) {
    // socket TCP: AF_INET = IPv4, SOCK_STREAM = flujo de bytes. Devuelve su número o -1 si falla
    int escucha = socket(AF_INET, SOCK_STREAM, 0);
    int activar = 1;
    // SO_REUSEADDR: poder relanzar el servidor en el mismo puerto sin esperar a que el sistema lo libere
    setsockopt(escucha, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));
    configurar_timeout(escucha, segundos_timeout);   // en el socket de escucha, el timeout afecta a accept
    sockaddr_in direccion{};                         // dirección IPv4: familia, puerto e IP
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);              // htons: puerto en orden de red
    inet_pton(AF_INET, ip, &direccion.sin_addr);     // IP de texto ("127.0.0.1") a sus 4 bytes
    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // Con &&, cada paso solo se intenta si el anterior fue bien: socket válido, luego bind, luego listen.
    // (sockaddr*) &direccion: la sockaddr_in vista como el tipo genérico que pide bind.
    // listen(escucha, 64): aceptar llamadas, con hasta 64 clientes esperando en cola.
    bool ok = escucha != -1 && ::bind(escucha, (sockaddr*) &direccion, sizeof(direccion)) == 0
              && listen(escucha, 64) == 0;
    if (!ok) {
        perror("socket, bind o listen");
        close(escucha);   // liberar el socket
        escucha = -1;
    }
    return escucha;
}

// socket + connect, con timeout en recv. Devuelve el socket conectado o -1.
inline int conectar_con_servidor(const char* ip, int puerto, int segundos_timeout) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    configurar_timeout(sock, segundos_timeout);   // recv no esperará más de esos segundos
    sockaddr_in servidor{};                       // dirección del servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);            // htons: puerto en orden de red
    inet_pton(AF_INET, ip, &servidor.sin_addr);   // IP de texto a sus 4 bytes
    // connect: "llamar" al servidor. Con &&, solo se intenta si socket no falló (-1)
    if (sock != -1 && connect(sock, (sockaddr*) &servidor, sizeof(servidor)) == -1) {
        close(sock);
        sock = -1;
    }
    return sock;
}

// send puede enviar menos bytes de los pedidos: se repite hasta que sale todo.
// MSG_NOSIGNAL evita que el programa muera si el otro extremo ya ha cerrado.
// const string& texto: el texto sin copiarlo (&) y sin poder cambiarlo (const).
inline bool enviar_todo(int fd, const string& texto) {
    size_t enviados = 0;   // bytes que ya han salido (size_t: entero sin signo para tamaños)
    bool ok = true;
    while (ok && enviados < texto.size()) {
        // c_str() + enviados: puntero al primer byte que falta; size() - enviados: cuántos faltan
        ssize_t n = send(fd, texto.c_str() + enviados, texto.size() - enviados, MSG_NOSIGNAL);
        if (n <= 0) {
            ok = false;
        } else {
            enviados += n;
        }
    }
    return ok;
}

// Lee de byte en byte hasta '\n' (que no se guarda).
// Devuelve false si el otro cerró, hubo error o se agotó el timeout.
// string& linea, sin const: la función escribe la línea en la variable de quien la llama.
inline bool recibir_linea(int fd, string& linea) {
    linea = "";
    bool ok = true;
    bool fin_de_linea = false;
    while (ok && !fin_de_linea) {
        char c = 0;
        // recv de 1 byte, que se deja en c (&c = su dirección). Devuelve 1, 0 si el otro cerró o -1 si error o timeout
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0) {
            ok = false;
        } else if (c == '\n') {
            fin_de_linea = true;
        } else {
            linea += c;   // añadir el carácter al final de la línea
        }
    }
    return ok;
}
