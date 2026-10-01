// =====================================================================
// Tema 15.3  |  async y future: tareas que devuelven un resultado
// En este programa he sumado un rango en dos mitades con async, he recogido el total con get y he comparado launch::async con launch::deferred.
// Requisitos previos: 01_suma_paralela_speedup
// Qué observar al ejecutar: main sigue trabajando mientras calculan; get() espera; la tarea deferred se ejecuta en el hilo de main.
// Salida determinista: SI (salvo los identificadores de hilo y los tiempos)
// Compilar con: cualquier nivel
// =====================================================================
// Con thread, para devolver un resultado hacía falta un parámetro por
// referencia. async devuelve un future: una "promesa de valor"
// que se recoge con get().
//
// ¿Y el join? Aquí no creamos ningún thread: el hilo lo gestiona el
// future. get() espera a que la tarea termine (hace de join) y, si no se
// llama, el destructor del future también espera.
// Trampa: async(...) sin guardar el future en una variable espera en
// esa misma línea, y el programa vuelve a ser secuencial sin darte cuenta.
#include <iostream>   // cout
#include <thread>     // thread::id, this_thread::get_id
#include <future>     // async, future, launch
#include <chrono>     // chrono (en este ejemplo no llega a usarse)

using namespace std;

const long long N = 400000000LL;   // LL: el número es de tipo long long

long long sumar_rango(long long desde, long long hasta) {
    long long suma = 0;
    for (long long i = desde; i < hasta; i++) {
        suma += i % 7;                  // %: resto de dividir entre 7; solo da trabajo a la CPU
    }
    return suma;                        // un valor de retorno normal
}

// thread::id es el tipo del identificador de un hilo.
thread::id quien_me_ejecuta() {
    return this_thread::get_id();       // identificador del hilo que está ejecutando esta línea
}

int main() {
    cout << "Hilo de main: " << this_thread::get_id() << "\n";

    // launch::async: arranca YA en otro hilo.
    // future<long long>: aquí llegará el long long que devuelva sumar_rango.
    // 0LL: el cero escrito como long long, del mismo tipo que N / 2.
    future<long long> primera_mitad = async(launch::async, sumar_rango, 0LL, N / 2);
    future<long long> segunda_mitad = async(launch::async, sumar_rango, N / 2, N);

    cout << "main no espera: sigue con sus cosas mientras las dos mitades calculan\n";

    // get() BLOQUEA hasta que la tarea termina y devuelve su valor. Solo se puede llamar una vez.
    long long total = primera_mitad.get() + segunda_mitad.get();
    cout << "Total: " << total << "\n";

    // launch::async frente a launch::deferred
    future<thread::id> en_otro_hilo = async(launch::async, quien_me_ejecuta);   // este future traerá un thread::id
    future<thread::id> perezosa = async(launch::deferred, quien_me_ejecuta);
    // deferred no crea hilo: la función se ejecuta DENTRO de get(), en el hilo que llama.
    cout << "async    se ejecuto en el hilo " << en_otro_hilo.get() << "\n";
    cout << "deferred se ejecuto en el hilo " << perezosa.get() << " (el de main)\n";
    return 0;   // 0 = el programa ha terminado bien
}
