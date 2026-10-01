// =====================================================================
// Tema 16.0  |  Utilidades de red compartidas por los ejemplos de RPC (AMPLIACIÓN)
// En este programa he reunido las funciones de sockets ya vistas en 12-sockets-tcp y 13-servidores-concurrentes para no repetirlas en cada fichero.
// Requisitos previos: 12-sockets-tcp, 13-servidores-concurrentes
// Qué observar al ejecutar: no se ejecuta; lo incluyen 01 a 04 con #include "red_utilidades.hpp".
// Salida determinista: SI
// Compilar con: cualquier nivel (solo Linux: sockets POSIX)
// =====================================================================
// Todo son funciones libres. Llevan 'inline' porque están definidas en una
// cabecera: así se podría incluir desde varios .cpp del mismo programa sin
// error de "definición múltiple".
//
// Protocolo de todos los ejemplos: texto, una petición o respuesta por línea
// terminada en '\n'. TCP es un flujo de bytes, no de mensajes: la línea es
// nuestra forma de saber dónde acaba cada mensaje.
#pragma once   // evita que la cabecera se incluya dos veces en el mismo .cpp

#include <iostream>       // cout
#include <string>         // string
#include <sys/socket.h>   // socket, setsockopt, bind, listen, accept, connect, send, recv
#include <netinet/in.h>   // sockaddr_in, htons, htonl, INADDR_ANY
#include <arpa/inet.h>    // inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close

// En una cabecera, using namespace std se desaconseja en proyectos reales porque se impone a quien la incluya.
// Aquí la incluyen solo los ejemplos del curso, que ya lo usan.
using namespace std;

// Tras 'segundos' sin recibir nada, recv y accept devuelven -1 en lugar de esperar para siempre.
inline void red_poner_espera_maxima(int socket_red, int segundos) {
    timeval espera{};             // timeval: segundos + microsegundos; {} lo pone todo a 0
    espera.tv_sec = segundos;     // tv_sec: la parte de segundos
    // SO_RCVTIMEO: tiempo máximo esperando al recibir. &espera y sizeof(espera): dirección y tamaño del dato.
    setsockopt(socket_red, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
}

// Socket de escucha TCP en todas las interfaces. Devuelve -1 si falla.
inline int red_crear_servidor(int puerto, int segundos_sin_clientes) {
    int servidor = socket(AF_INET, SOCK_STREAM, 0);   // AF_INET: IPv4; SOCK_STREAM: TCP. Devuelve un número de socket
    int activar = 1;
    // SO_REUSEADDR: permite volver a abrir el puerto nada más cerrar el servidor, sin esperar.
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activar, sizeof(activar));

    sockaddr_in direccion{};                          // dirección IPv4 (familia, IP y puerto), a cero con {}
    direccion.sin_family = AF_INET;                   // familia IPv4
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);    // INADDR_ANY: todas las interfaces; htonl: al orden de bytes de la red
    direccion.sin_port = htons(puerto);               // htons: el puerto al orden de bytes de la red

    // ::bind es el bind de POSIX. Con using namespace std, sin los '::' el compilador elegiría bind de la librería estándar.
    // (sockaddr*) &direccion: la dirección de nuestra estructura, vista como la sockaddr genérica que pide bind.
    bool correcto = ::bind(servidor, (sockaddr*) &direccion, sizeof(direccion)) == 0 &&
                    listen(servidor, 8) == 0;   // listen: empieza a escuchar, con hasta 8 clientes en cola
    if (correcto) {
        red_poner_espera_maxima(servidor, segundos_sin_clientes);
    } else {
        cout << "No se pudo abrir el puerto " << puerto << " (esta ocupado?)\n";
        close(servidor);          // close: libera el socket
        servidor = -1;            // -1 avisa a quien llama de que ha fallado
    }
    return servidor;
}

// Conecta con ip:puerto. Devuelve el socket o -1 si no hay nadie escuchando.
inline int red_conectar(const string& ip, int puerto) {
    int conexion = socket(AF_INET, SOCK_STREAM, 0);   // socket TCP sobre IPv4
    sockaddr_in direccion{};
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    inet_pton(AF_INET, ip.c_str(), &direccion.sin_addr);   // inet_pton: pasa el texto "127.0.0.1" a los bytes de la IP
    // connect: intenta conectar con el servidor; devuelve 0 si lo consigue.
    if (connect(conexion, (sockaddr*) &direccion, sizeof(direccion)) != 0) {
        close(conexion);
        conexion = -1;
    }
    return conexion;
}

// Envía la línea completa más '\n'. send puede enviar menos bytes de los pedidos: se repite.
// MSG_NOSIGNAL: si el otro extremo ya cerró, send devuelve error en vez de matar el programa.
inline bool red_enviar_linea(int conexion, const string& linea) {
    string datos = linea + "\n";
    size_t enviados = 0;          // size_t: entero sin signo, el tipo de los tamaños
    bool correcto = true;
    while (correcto && enviados < datos.size()) {   // sigue mientras no haya error Y falten bytes por enviar
        // datos.c_str() + enviados: puntero al primer byte que aún no se ha enviado.
        // ssize_t: como size_t pero con signo, para que send pueda devolver -1.
        ssize_t n = send(conexion, datos.c_str() + enviados, datos.size() - enviados, MSG_NOSIGNAL);
        if (n <= 0) {             // -1: error; 0: no se ha enviado nada
            correcto = false;
        } else {
            enviados += n;
        }
    }
    return correcto;
}

// Lee bytes de uno en uno hasta '\n' (lento pero muy fácil de seguir).
// Devuelve false si la conexión se cerró, hubo error o venció la espera máxima.
// string& linea: la línea recibida se deja en la variable de quien llama.
inline bool red_recibir_linea(int conexion, string& linea) {
    linea = "";                   // se vacía por si traía algo de antes
    char caracter = 0;
    bool fin_de_linea = false;
    bool conexion_viva = true;
    while (!fin_de_linea && conexion_viva) {
        ssize_t leidos = recv(conexion, &caracter, 1, 0);   // recv: lee como mucho 1 byte y lo deja en caracter (&caracter: su dirección)
        if (leidos <= 0) {
            conexion_viva = false;      // 0: el otro cerró; -1: error o tiempo agotado
        } else if (caracter == '\n') {
            fin_de_linea = true;
        } else {
            linea += caracter;          // añade el carácter al final del texto
        }
    }
    return fin_de_linea;          // true solo si llegó una línea completa
}
