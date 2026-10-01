// =====================================================================
// Tema 15.4  |  fork(): un proceso se convierte en dos
// En este programa he llamado a fork para que el proceso se divida en padre e hijo, y he usado el valor que devuelve para saber quién es quién.
// Requisitos previos: 02-fundamentos-multiprogramacion (qué es un proceso), 03-hilos-creacion
// Qué observar al ejecutar: la línea "fork me ha devuelto" sale DOS veces con valores distintos; el hijo tiene otro PID.
// Salida determinista: NO (los PID cambian y el orden de las líneas de padre e hijo puede variar)
// Compilar con: cualquier nivel (solo Linux y macOS: fork no existe en Windows)
// =====================================================================
// Material original: Tema 4, ejemplo con os.fork() en Python.
//
// OJO CON EL BÚFER: cout guarda el texto en memoria antes de escribirlo.
// fork copia TODA la memoria del proceso, búfer incluido: lo que no se haya
// escrito aún saldría dos veces (se nota al redirigir a fichero:
// ./04_fork_primer_proceso > salida.txt). Por eso vaciamos con flush
// justo antes de cada fork.
#include <iostream>      // cout, flush
#include <string>        // string, to_string
#include <unistd.h>      // fork, getpid, getppid
#include <sys/wait.h>    // waitpid

using namespace std;

int main() {
    // getpid(): el PID (número de proceso) de este programa. flush: escribe ya lo que haya en el búfer.
    cout << "Antes del fork: un solo proceso, PID " << getpid() << "\n" << flush;

    pid_t pid = fork();                 // pid_t: tipo entero que usa el sistema para los PID
    // Desde aquí hay DOS procesos ejecutando el mismo código, cada uno con su copia de 'pid'.

    // La línea se construye entera con + (to_string pasa el número a texto) y se imprime de una vez,
    // para que las líneas del padre y del hijo no se mezclen a trozos.
    cout << "fork me ha devuelto " + to_string(pid) + "\n" << flush;

    if (pid > 0) {
        // En el padre, fork devuelve el PID del hijo.
        cout << "Soy el PADRE, PID " + to_string(getpid()) +
                     ", mi hijo es " + to_string(pid) + "\n" << flush;
        // nullptr: aquí no pedimos el código de salida del hijo; 0: sin opciones especiales.
        waitpid(pid, nullptr, 0);       // esperar al hijo: el "join" de los procesos (ver 06)
        cout << "Padre: mi hijo ha terminado\n";
    } else if (pid == 0) {
        // En el hijo, fork devuelve 0. Su padre se consulta con getppid.
        cout << "Soy el HIJO,  PID " + to_string(getpid()) +
                     ", mi padre es " + to_string(getppid()) + "\n" << flush;
    } else {
        cout << "fork ha fallado\n";    // fork devuelve -1 si no puede crear el proceso
    }
    return 0;   // este return lo ejecutan los DOS procesos: cada uno termina por su lado
}
