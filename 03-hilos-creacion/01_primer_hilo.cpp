// =====================================================================
// Tema 3.1  |  Primer hilo: thread y join
// En este programa he lanzado la función saludar en un hilo nuevo mientras main sigue a lo suyo, y al final he esperado al hilo con join.
// Requisitos previos: 01-repaso-cpp (funciones, sleep_for)
// Qué observar al ejecutar: el hilo principal sigue trabajando mientras el nuevo hilo saluda.
// Salida determinista: NO (el orden de las líneas de main y del hilo puede cambiar)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <chrono>     // milliseconds, para indicar cuánto tiempo dormir
#include <iostream>   // cout
#include <thread>     // thread y this_thread

using namespace std;

// La función que ejecutará el hilo. Es una función libre normal y corriente.
void saludar() {
    for (int i = 1; i <= 3; i++) {
        cout << "  [hilo nuevo] hola " << i << "\n";
        // Duerme ESTE hilo 300 milisegundos; los demás hilos siguen trabajando.
        this_thread::sleep_for(chrono::milliseconds(300));
    }
}

int main() {
    cout << "[main] voy a crear el hilo\n";

    // FORK: el hilo arranca en el mismo instante en que se construye el objeto.
    // Se pasa el NOMBRE de la función, sin paréntesis: saludar, no saludar().
    thread hilo(saludar);

    // Mientras tanto, main (que también es un hilo) hace su propio trabajo.
    for (int i = 1; i <= 3; i++) {
        cout << "[main] sigo con lo mio " << i << "\n";
        this_thread::sleep_for(chrono::milliseconds(300));
    }

    // JOIN: main se queda aquí parado hasta que el hilo termina.
    // Si comentas esta línea el programa aborta ("terminate called without an
    // active exception"): destruir un thread sin join detiene el programa entero.
    hilo.join();
    // Pregunta para clase: ¿qué pasaría si main usara un resultado del hilo
    // ANTES de esta línea? Que el hilo quizá aún no lo ha calculado.

    cout << "[main] el hilo ha terminado, fin del programa\n";
    return 0;   // 0 = el programa ha terminado bien
}
