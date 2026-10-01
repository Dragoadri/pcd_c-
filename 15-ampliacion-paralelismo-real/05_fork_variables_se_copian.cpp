// =====================================================================
// Tema 15.5  |  Procesos frente a hilos: el hijo trabaja sobre una COPIA de la memoria
// En este programa he sumado uno a variable_global en un hijo creado con fork y después en un hilo, para ver que solo el hilo cambia la del padre.
// Requisitos previos: 04_fork_primer_proceso
// Qué observar al ejecutar: el hijo ve 21 pero el padre sigue viendo 20; tras un hilo que hace lo mismo, el padre ve 21.
// Salida determinista: SI (salvo los PID; el padre espera al hijo antes de imprimir)
// Compilar con: cualquier nivel (solo Linux y macOS)
// =====================================================================
// Material original: Tema 4, "Copia de variables (y III): valores traspasados".
// Global a 10, el programa la pone a 20 y una función le suma 1 en un hijo.
//
// Moraleja: los procesos NO comparten memoria, así que no tienen condiciones
// de carrera sobre variables... pero para devolver un resultado necesitan
// comunicarse (código de salida, tuberías, sockets). Los hilos comparten
// memoria: comunicarse es gratis, protegerse es obligatorio.
#include <iostream>      // cout
#include <string>        // string, to_string
#include <thread>        // thread
#include <unistd.h>      // fork, getpid
#include <sys/wait.h>    // waitpid

using namespace std;

int variable_global = 10;   // global: declarada fuera de cualquier función

void sumar_uno() {
    variable_global = variable_global + 1;
}

int main() {
    variable_global = 20;
    // flush: vacía el búfer de cout ANTES del fork, para que este texto no se copie al hijo y salga dos veces.
    cout << "Padre antes del fork: variable_global = " << variable_global << "\n" << flush;

    pid_t pid = fork();     // desde aquí hay dos procesos, cada uno con SU copia de variable_global
    if (pid == 0) {         // fork devuelve 0 en el hijo
        sumar_uno();        // cambia la copia del hijo; la del padre no se entera
        // getpid: PID de este proceso. to_string: número a texto para pegarlo con +.
        cout << "Hijo (PID " + to_string(getpid()) + ") tras sumar uno: variable_global = "
                     + to_string(variable_global) + "\n" << flush;
    } else if (pid > 0) {   // en el padre, fork devuelve el PID del hijo
        waitpid(pid, nullptr, 0);   // espera a que termine el hijo (nullptr: no pido su código de salida)
        cout << "Padre tras esperar al hijo:      variable_global = " << variable_global << "\n";

        // Ahora lo mismo con un HILO: comparte la memoria del proceso.
        thread hilo(sumar_uno);     // el hilo arranca aquí y cambia la variable de ESTE proceso
        hilo.join();                // espera a que el hilo termine antes de imprimir
        cout << "Padre tras un HILO que suma uno: variable_global = " << variable_global << "\n";
    } else {
        cout << "fork ha fallado\n";    // fork devuelve -1 si no puede crear el proceso
    }
    return 0;   // 0 = el programa ha terminado bien
}
