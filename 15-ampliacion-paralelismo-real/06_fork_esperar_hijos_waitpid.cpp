// =====================================================================
// Tema 15.6  |  waitpid: el "join" de los procesos y el código de salida
// En este programa he creado NUM_HIJOS procesos con fork en un bucle, los he esperado con waitpid y he leído el código de salida de cada uno.
// Requisitos previos: 05_fork_variables_se_copian
// Qué observar al ejecutar: los hijos terminan en orden inverso, pero el padre los recoge en orden y lee su código de salida.
// Salida determinista: SI (salvo los PID; las esperas están separadas 300 ms)
// Compilar con: cualquier nivel (solo Linux y macOS)
// =====================================================================
// Un hijo que termina y nadie espera se queda ZOMBI (aparece como <defunct>
// con estado Z en "ps"). Prueba: durante la pausa del padre, en otra terminal,
//   ps -o pid,stat,cmd --ppid <PID del padre>
// Regla, igual que con los hilos: todo proceso creado se espera.
//
// El bucle NO usa break para que el hijo deje de crear hijos: la bandera
// 'numero_hijo' vale -1 en el padre y la condición del for lo detiene en el hijo.
#include <iostream>      // cout
#include <string>        // string, to_string
#include <vector>        // vector
#include <thread>        // this_thread::sleep_for
#include <chrono>        // milliseconds
#include <unistd.h>      // fork, getpid
#include <sys/wait.h>    // waitpid, WIFEXITED, WEXITSTATUS

using namespace std;

const int NUM_HIJOS = 3;

int main() {
    vector<pid_t> hijos(NUM_HIJOS, 0);  // aquí el padre apunta el PID de cada hijo
    int numero_hijo = -1;               // -1: soy el padre; 0, 1, 2: soy ese hijo

    // Condición compuesta: sigue mientras queden hijos por crear Y este proceso siga siendo el padre.
    for (int i = 0; i < NUM_HIJOS && numero_hijo == -1; i++) {
        cout << flush;                  // vacía el búfer de cout antes de copiar el proceso
        pid_t pid = fork();
        if (pid == 0) {
            numero_hijo = i;            // en el hijo: el for ya no dará más vueltas
        } else {
            hijos[i] = pid;             // en el padre: fork devolvió el PID del hijo y lo guardamos
        }
    }

    if (numero_hijo >= 0) {
        // Código del HIJO: el primero duerme más, así terminan en orden inverso.
        // sleep_for duerme este proceso: hijo 0 900 ms, hijo 1 600 ms, hijo 2 300 ms.
        this_thread::sleep_for(chrono::milliseconds(300 * (NUM_HIJOS - numero_hijo)));
        cout << "  hijo " + to_string(numero_hijo) + " (PID " + to_string(getpid())
                     + ") termina con codigo " + to_string(10 + numero_hijo) + "\n" << flush;
        // return en main termina el proceso HIJO; ese número es lo que recogerá el padre.
        return 10 + numero_hijo;        // el código de salida es un número de 0 a 255
    }

    // Código del PADRE
    cout << "Padre (PID " << getpid() << "): he creado " << NUM_HIJOS << " hijos\n" << flush;
    for (int i = 0; i < NUM_HIJOS; i++) {
        int estado = 0;                 // waitpid escribirá aquí cómo terminó el hijo
        // &estado: le pasamos la dirección de la variable para que waitpid pueda rellenarla.
        waitpid(hijos[i], &estado, 0);  // bloquea hasta que ESE hijo termina (con -1: cualquiera)
        if (WIFEXITED(estado)) {        // WIFEXITED: cierto si el hijo terminó normalmente (con return)
            cout << "Padre: recogido el hijo " << i << ", codigo de salida "
                      << WEXITSTATUS(estado) << "\n";   // WEXITSTATUS: saca de estado el número devuelto
        }
    }
    cout << "Padre: todos los hijos recogidos, ninguno queda zombi\n";
    return 0;   // 0 = el programa ha terminado bien
}
