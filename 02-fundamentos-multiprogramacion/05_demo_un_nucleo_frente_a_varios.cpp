// =====================================================================
// Tema 2.5  |  Demo: con un solo núcleo no hay paralelismo real
// En este programa he cronometrado contar_primos dos veces seguidas y luego en dos hilos a la vez, para comparar con varios núcleos y con uno solo.
// Requisitos previos: 04_cpu_y_espera_de_entrada_salida.cpp
// Qué observar al ejecutar: con varios núcleos las dos tareas a la vez tardan la mitad; con taskset -c 0, lo mismo que en secuencia.
// Salida determinista: NO (los tiempos dependen de la máquina y de la carga)
// Compilar con: cualquier nivel (los tiempos citados son del perfil Debug, -O0)
// =====================================================================
// DEMO DEL PROFESOR, NO SE TECLEA. Usa thread como caja negra: se
// explica en 03-hilos-creacion. Ejecutar dos veces desde el terminal:
//     ./build/bin/05_demo_un_nucleo_frente_a_varios
//     taskset -c 0 ./build/bin/05_demo_un_nucleo_frente_a_varios   (solo el núcleo 0)
// Medido en el equipo de preparación (16 núcleos, GCC 13, -O0): secuencial
// ~1,7 s y a la vez ~0,95 s; con taskset -c 0, las dos versiones ~1,65 s.
#include <chrono>     // steady_clock, milliseconds y duration_cast
#include <iostream>   // cout
#include <thread>     // thread, hardware_concurrency

using namespace std;

const int LIMITE = 4000000;   // cuenta los primos menores que 4 millones

// Trabajo de CPU puro: contar primos por división (no hay esperas).
// int& resultado: la función deja la cuenta en la variable de main (por referencia).
void contar_primos(int& resultado) {
    int cuenta = 0;
    for (int n = 2; n < LIMITE; n++) {
        bool es_primo = true;
        // Pruebo divisores d mientras d * d <= n (basta llegar a la raíz de n)
        // Y mientras siga pareciendo primo: en cuanto encuentro un divisor, el for acaba.
        for (int d = 2; d * d <= n && es_primo; d++) {
            if (n % d == 0) {   // resto 0: d divide a n, así que n no es primo
                es_primo = false;
            }
        }
        if (es_primo) {
            cuenta = cuenta + 1;
        }
    }
    resultado = cuenta;   // escribe en r1 o r2 de main
}

// Devuelve cuántos milisegundos han pasado desde el instante inicio.
long long milisegundos_desde(chrono::steady_clock::time_point inicio) {
    chrono::steady_clock::time_point ahora = chrono::steady_clock::now();   // instante actual
    // ahora - inicio es la duración; duration_cast la pasa a ms y .count() da el número.
    return chrono::duration_cast<chrono::milliseconds>(ahora - inicio).count();
}

int main() {
    // hardware_concurrency: cuántos núcleos lógicos ve el programa.
    cout << "Nucleos disponibles: " << thread::hardware_concurrency() << "\n";
    int r1 = 0;
    int r2 = 0;

    // Primera medida: las dos tareas una detrás de otra, en el hilo principal.
    chrono::steady_clock::time_point inicio = chrono::steady_clock::now();
    contar_primos(r1);
    contar_primos(r2);
    cout << "Una tarea detras de otra: " << milisegundos_desde(inicio) << " ms\n";

    // Segunda medida: las dos tareas a la vez, cada una en su hilo.
    inicio = chrono::steady_clock::now();
    // thread tarea1(funcion, argumento): arranca un hilo que ejecuta contar_primos(r1).
    // ref(r1) hace que el hilo reciba r1 por referencia; sin ref, thread pasaría una copia.
    thread tarea1(contar_primos, ref(r1));
    thread tarea2(contar_primos, ref(r2));
    // join: main se queda esperando aquí hasta que cada hilo termina.
    tarea1.join();
    tarea2.join();
    cout << "Las dos a la vez (2 hilos): " << milisegundos_desde(inicio) << " ms\n";
    cout << "Primos encontrados por cada tarea: " << r1 << " y " << r2 << "\n";
    return 0;   // 0 = el programa ha terminado bien
}
