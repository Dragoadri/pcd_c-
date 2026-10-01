// =====================================================================
// Tema 3.4  |  TRAMPA 1: thread copia los argumentos
// En este programa he pasado la variable contador a un hilo sin ref, y he visto que el hilo suma en una copia y main sigue viendo 0.
// Requisitos previos: 03_hilo_con_argumentos, 01-repaso-cpp (paso por valor y por referencia)
// Qué observar al ejecutar: el hilo cuenta hasta 5, pero main sigue viendo contador = 0.
// Salida determinista: SI (el hilo trabaja sobre su copia; main imprime tras el join)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// TRAMPA 1 del curso (error número uno de los principiantes).
//
// Caso A, parámetro por referencia SIN ref: NO COMPILA.
//     void contar_ref(int& contador);
//     thread hilo(contar_ref, contador);
// GCC responde con decenas de líneas de plantillas; lo importante es:
//     error: static assertion failed: thread arguments must be
//            invocable after conversion to rvalues
// Traducción: thread copia 'contador' y una copia temporal no se
// puede atar a un int&. Descomenta las dos líneas marcadas para verlo.
//
// Caso B, parámetro por valor: COMPILA y parece correcto, pero no comparte nada.
#include <iostream>   // cout
#include <thread>     // thread

using namespace std;

void contar_copia(int contador) {          // recibe una COPIA
    for (int i = 0; i < 5; i++) {
        contador++;   // suma 1 a la copia del hilo, no a la variable de main
    }
    cout << "[hilo] mi contador vale " << contador << "\n";
}

// int&: el & pegado al tipo significa referencia (otro nombre para la variable original).
void contar_ref(int& contador) {           // pide la variable original
    contador++;
}

int main() {
    int contador = 0;

    thread hilo(contar_copia, contador);   // Caso B: se copia 'contador'
    hilo.join();   // espero a que el hilo acabe antes de imprimir

    // thread hilo_ref(contar_ref, contador);   // Caso A: descomenta y NO compila
    // hilo_ref.join();                              // Caso A

    cout << "[main] contador vale " << contador << "\n";
    cout << "[main] el hilo ha trabajado sobre su copia, no sobre mi variable\n";
    return 0;   // 0 = el programa ha terminado bien
}
