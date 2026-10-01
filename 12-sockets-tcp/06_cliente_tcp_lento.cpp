// =====================================================================
// Tema 12.6  |  Cliente lento: conecta y tarda en hablar
// En este programa he conectado con el servidor secuencial y he esperado 3 s antes de mandar mi nombre, midiendo cuánto tardo en total en ser atendido.
// Requisitos previos: 05_servidor_tcp_secuencial.cpp
// Qué observar al ejecutar: lanzando dos a la vez, uno tarda unos 5 s en total y el otro unos 7 s, aunque ambos conectaron en el instante 0.
// Salida determinista: NO (depende de cuántos clientes haya delante)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// CÓMO LANZARLO: con 05_servidor_tcp_secuencial arrancado, dos a la vez:
//   ./build/bin/06_cliente_tcp_lento & ./build/bin/06_cliente_tcp_lento
// Ojo: connect funciona al instante para los dos. El sistema acepta la conexión y la deja
// en la cola de listen, pero el programa servidor no la atiende hasta que acaba con el anterior.
#include <iostream>       // cout
#include <string>         // string, to_string
#include <thread>         // this_thread::sleep_for
#include <chrono>         // steady_clock, duration, seconds
#include <cstdio>         // perror
#include <sys/socket.h>   // socket, connect, send, recv, setsockopt
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // htons, inet_pton
#include <sys/time.h>     // timeval
#include <unistd.h>       // close, getpid

using namespace std;

const char* IP_SERVIDOR = "127.0.0.1";   // localhost: el servidor está en este mismo equipo
const int PUERTO = 5202;                 // el puerto de 05_servidor_tcp_secuencial
const int SEGUNDOS_PENSANDO = 3;

// Repite send hasta que sale todo el texto (send puede enviar menos de lo pedido). false = conexión rota
bool enviar_todo(int fd, const string& texto) {
    size_t enviados = 0;
    bool ok = true;
    while (ok && enviados < texto.size()) {
        // c_str() + enviados: puntero al primer byte pendiente. MSG_NOSIGNAL: si el otro cerró, -1 en vez de morir
        ssize_t n = send(fd, texto.c_str() + enviados, texto.size() - enviados, MSG_NOSIGNAL);
        if (n <= 0) {
            ok = false;
        } else {
            enviados += n;
        }
    }
    return ok;
}

// Lee byte a byte hasta '\n' y deja el texto en 'linea' (string& sin const: la función la rellena)
bool recibir_linea(int fd, string& linea) {
    linea = "";
    bool ok = true;
    bool fin_de_linea = false;
    while (ok && !fin_de_linea) {
        char c = 0;
        ssize_t n = recv(fd, &c, 1, 0);   // recibir 1 byte en c (&c = su dirección); 0 = cerrado, -1 = error o timeout
        if (n <= 0) {
            ok = false;
        } else if (c == '\n') {
            fin_de_linea = true;
        } else {
            linea += c;
        }
    }
    return ok;
}

int main() {
    // steady_clock::now(): el instante actual, en un reloj que nunca va hacia atrás
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();
    // getpid(): número de proceso que el sistema da a este programa. to_string lo pasa a texto
    string nombre = "lento_" + to_string(getpid());   // el PID distingue a cada cliente

    // socket TCP: AF_INET = IPv4, SOCK_STREAM = flujo de bytes. Devuelve su número o -1
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    timeval espera{};   // tiempo en segundos (tv_sec) y microsegundos (tv_usec)
    espera.tv_sec = 30;
    // SO_RCVTIMEO: recv espera la respuesta como mucho 30 s
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &espera, sizeof(espera));
    sockaddr_in servidor{};                                // dirección del servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);                     // htons: puerto en orden de red
    inet_pton(AF_INET, IP_SERVIDOR, &servidor.sin_addr);   // IP de texto a 4 bytes
    // Con ||, si socket falló (-1) ya no se llama a connect.
    // connect: "llamar" al servidor; (sockaddr*) &servidor = la dirección vista como el tipo genérico que pide.
    if (sock == -1 || connect(sock, (sockaddr*) &servidor, sizeof(servidor)) == -1) {
        perror("connect");   // sin servidor: "Connection refused"
        cout << "Arranca primero 05_servidor_tcp_secuencial\n";
        return 1;
    }
    cout << nombre + ": conectado, me pienso el nombre " + to_string(SEGUNDOS_PENSANDO) + " s\n";
    this_thread::sleep_for(chrono::seconds(SEGUNDOS_PENSANDO));   // el cliente lento: conectado pero sin hablar

    enviar_todo(sock, nombre + "\n");
    string respuesta;
    if (recibir_linea(sock, respuesta)) {
        // Restar dos instantes da una duración; duration<double> la guarda en segundos con decimales
        chrono::duration<double> t = chrono::steady_clock::now() - inicio;
        // Se construye un único string con + para que la línea salga de una vez y no se mezcle con la del otro cliente
        cout << nombre + ": tras " + to_string(t.count()) + " s recibo: " + respuesta + "\n";   // count(): los segundos
    } else {
        cout << nombre + ": el servidor no contesto\n";
    }
    close(sock);   // colgar la conexión
    return 0;      // 0 = el programa ha terminado bien
}
