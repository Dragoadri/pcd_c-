// =====================================================================
// Tema 1.9  |  Ámbito y tiempo de vida de las variables
// En este programa he llamado tres veces a visitar para comparar una variable local, una static y una global, y he usado un bloque { }.
// Requisitos previos: 08_paso_por_referencia_y_puntero.cpp
// Qué observar al ejecutar: la local vuelve a 1 en cada llamada; la static y la global recuerdan.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout

using namespace std;

// Está fuera de cualquier función, por eso es global.
int llamadas_totales = 0;  // GLOBAL: visible desde todo el fichero, vive todo el programa

void visitar() {
    int local = 0;            // LOCAL: nace al entrar en la función y muere al salir
    // La línea del static solo se ejecuta la PRIMERA vez; en las siguientes llamadas se salta.
    static int recuerdo = 0;  // STATIC: se crea una vez y vive hasta el final

    local = local + 1;
    recuerdo = recuerdo + 1;
    llamadas_totales = llamadas_totales + 1;

    cout << "  local = " << local << ", static = " << recuerdo
              << ", global = " << llamadas_totales << "\n";
}   // aquí muere 'local'

int main() {
    for (int i = 0; i < 3; i++) {  // 'i' solo existe dentro del for
        visitar();
    }
    // cout << i;   // <- descomenta: error de compilación, i ya no existe

    // Unas llaves sueltas crean un bloque: lo declarado dentro muere al cerrar la llave.
    {
        int temporal = 42;  // solo existe entre estas llaves
        cout << "Dentro del bloque, temporal = " << temporal << "\n";
    }
    // cout << temporal;   // <- descomenta: error, temporal ya no existe

    // Regla para los hilos: si un hilo usa una referencia o un puntero a
    // una variable LOCAL, esa variable tiene que seguir viva mientras el
    // hilo la use. Si su función ya terminó, el hilo lee memoria muerta
    // (lo veremos fallar en 04-hilos-usados-mal: referencia colgante).
    return 0;   // 0 = el programa ha terminado bien
}
