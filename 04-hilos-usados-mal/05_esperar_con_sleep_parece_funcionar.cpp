// =====================================================================
// Tema 4.5  |  Coordinar hilos con sleep: parece que funciona
// En este programa he hecho que el camarero (main) espere 500 ms con sleep_for en vez de join, y como la cocina tarda 300 ms parece que funciona.
// Requisitos previos: 03-hilos-creacion
// Qué observar al ejecutar: los 5 platos salen listos. Todo parece correcto.
// Salida determinista: SI (aquí la cocina siempre tarda 300 ms y el camarero espera 500 ms)
// Compilar con: -O0 (target sin optimizar, como todo el tema 04)
// =====================================================================
// El camarero (main) no sabe cuándo termina el cocinero (hilo): lo supone.
// Espera 500 ms "que seguro que da tiempo" y sirve el plato.
// Ojo: 'plato_listo' se escribe en un hilo y se lee en otro sin protección,
// que también es una carrera. Aquí lo importante es el sleep; lo otro, en 06.
#include <chrono>       // milliseconds
#include <functional>   // ref
#include <iostream>     // cout
#include <thread>       // thread y this_thread

using namespace std;

const int NUM_PLATOS = 5;
const int ESPERA_CAMARERO_MS = 500;

// bool&: referencia al plato_listo de main; el hilo lo pone a true cuando acaba.
void cocinar(bool& plato_listo, int tiempo_cocina_ms) {
    this_thread::sleep_for(chrono::milliseconds(tiempo_cocina_ms));   // simula que cocina
    plato_listo = true;   // aviso de que el plato está hecho
}

int main() {
    for (int plato = 1; plato <= NUM_PLATOS; plato++) {
        bool plato_listo = false;
        int tiempo_cocina_ms = 300;   // la cocina tarda siempre lo mismo

        // plato_listo se comparte con ref; tiempo_cocina_ms basta con copiarlo.
        thread cocinero(cocinar, ref(plato_listo), tiempo_cocina_ms);
        this_thread::sleep_for(chrono::milliseconds(ESPERA_CAMARERO_MS));   // "seguro que da tiempo"

        // Miro la bandera sin saber si el cocinero ha terminado: solo lo he supuesto.
        if (plato_listo) {
            cout << "Plato " << plato << ": servido (cocina " << tiempo_cocina_ms << " ms)\n";
        } else {
            cout << "Plato " << plato << ": SERVIDO CRUDO (cocina " << tiempo_cocina_ms << " ms)\n";
        }
        cocinero.join();   // aquí sí espero de verdad, pero el plato ya se ha servido
    }
    return 0;   // 0 = el programa ha terminado bien
}
