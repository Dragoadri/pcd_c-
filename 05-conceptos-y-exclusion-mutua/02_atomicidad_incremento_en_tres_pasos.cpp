// =====================================================================
// Tema 5.2  |  Atomicidad: contador++ son tres pasos
// En este programa he escrito contador++ como cargar, sumar y almacenar, con un yield en medio, para ver que dos hilos pierden sumas.
// Requisitos previos: 05/01, 04-hilos-usados-mal (contador compartido roto)
// Qué observar al ejecutar: el total es muy inferior a 20000; los incrementos perdidos son los que se pisaron.
// Salida determinista: NO (el número exacto de pérdidas cambia, pero siempre hay pérdidas)
// Compilar con: -O0 (target sin optimizar, como todos los ejemplos de carreras)
// =====================================================================
// Escribimos a mano lo que la máquina hace con "contador++":
//   1. cargar    contador -> registro
//   2. sumar     registro + 1
//   3. almacenar registro -> contador
// El yield() entre leer y escribir cede la CPU a propósito: así la ventana
// es enorme y la pérdida aparece siempre, incluso con un solo núcleo
// (probado con "taskset -c 0"). En 04 la ventana era de nanosegundos.
// Calibración (16 núcleos, GCC 13.3, -O0, 2 hilos x 10000): totales entre
// 10000 y 10400 de 20000 en 6 ejecuciones. Con taskset -c 0: ~10000.
#include <iostream>     // cout
#include <thread>       // thread y this_thread::yield
#include <functional>   // ref, para pasar una variable por referencia a un hilo

using namespace std;

const int VECES = 10000;

// int& contador: el hilo recibe la MISMA variable de main, no una copia.
void incrementar_en_tres_pasos(int& contador) {
    for (int i = 0; i < VECES; i++) {
        int registro = contador;          // 1. cargar
        this_thread::yield();        //    (aquí se cuela el otro hilo) yield: "planificador, deja pasar a otro"
        registro = registro + 1;          // 2. sumar
        contador = registro;              // 3. almacenar (si el otro escribió mientras, lo pisamos)
    }
}

int main() {
    int contador = 0;
    // ref(contador) hace que el hilo reciba la variable de verdad; sin ref,
    // thread copiaría el valor y cada hilo sumaría en su propia copia.
    thread a(incrementar_en_tres_pasos, ref(contador));
    thread b(incrementar_en_tres_pasos, ref(contador));
    a.join();   // join: main espera aquí a que termine el hilo a
    b.join();

    cout << "Esperado: " << 2 * VECES << "\n";
    cout << "Obtenido: " << contador << "\n";
    cout << "Incrementos perdidos: " << 2 * VECES - contador << "\n";
    cout << "Los dos leen 41, los dos escriben 42: se pierde uno.\n";
    return 0;   // 0 = el programa ha terminado bien
}
