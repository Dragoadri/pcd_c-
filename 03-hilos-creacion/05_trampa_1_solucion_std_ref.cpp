// =====================================================================
// Tema 3.5  |  TRAMPA 1 resuelta: ref para compartir una variable
// En este programa he compartido de verdad contador con dos hilos: al primero se lo paso con ref(contador) y al segundo con su dirección &contador.
// Requisitos previos: 04_trampa_1_los_argumentos_se_copian
// Qué observar al ejecutar: ahora main ve contador = 5 (con referencia) y luego 10 (con puntero).
// Salida determinista: SI (un solo hilo trabaja cada vez y main imprime después del join)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Recuerda: thread SIEMPRE copia lo que le pasas.
//   ref(x)  -> copia un "envoltorio" que se comporta como una referencia a x.
//   &x           -> copia un puntero, y el puntero sigue apuntando a la x original.
// Las dos formas comparten la MISMA variable. A partir de aquí, compartir
// variables entre hilos abre la puerta a los problemas de la carpeta 04.
// Y otra vez el join antes de leer: si main leyera 'contador' antes del join,
// el hilo podría no haber terminado todavía.
#include <functional>   // ref
#include <iostream>     // cout
#include <thread>       // thread

using namespace std;

// int&: referencia; dentro de la función, contador ES la variable de main con otro nombre.
void contar_ref(int& contador) {
    for (int i = 0; i < 5; i++) {
        contador++;
    }
}

// int*: puntero; contador guarda la DIRECCIÓN de la variable de main, no su valor.
void contar_puntero(int* contador) {
    for (int i = 0; i < 5; i++) {
        (*contador)++;   // paréntesis: primero desreferencio, luego incremento
    }
}

int main() {
    int contador = 0;

    // ref(contador): el hilo recibe una referencia a contador en lugar de una copia.
    thread hilo_ref(contar_ref, ref(contador));
    hilo_ref.join();   // espero al hilo ANTES de leer contador
    cout << "[main] tras el hilo con ref, contador = " << contador << "\n";

    // &contador: el & delante de una variable da su dirección de memoria (un int*).
    thread hilo_puntero(contar_puntero, &contador);
    hilo_puntero.join();
    cout << "[main] tras el hilo con puntero, contador = " << contador << "\n";
    return 0;   // 0 = el programa ha terminado bien
}
