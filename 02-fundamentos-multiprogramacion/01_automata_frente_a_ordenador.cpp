// =====================================================================
// Tema 2.1  |  Autómata frente a ordenador: el modelo de Von Neumann
// En este programa he simulado una CPU (función cpu) que capta, decodifica y ejecuta dos programas distintos guardados en un vector memoria.
// Requisitos previos: 01-repaso-cpp
// Qué observar al ejecutar: la MISMA función cpu() ejecuta dos programas distintos, instrucción a instrucción.
// Salida determinista: SI (simulación de un solo hilo, sin aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Un autómata (una máquina expendedora) hace siempre lo mismo: su "programa"
// está soldado en el circuito. En la máquina de Von Neumann el programa son
// DATOS en memoria: instrucciones y datos comparten la misma memoria, y la
// CPU repite captar -> decodificar -> ejecutar. Cambiar la memoria = otro programa.
//
// Cada casilla de instrucción vale codigo * 100 + direccion:
//   1 CARGAR d (acumulador = memoria[d])   2 SUMAR d   3 GUARDAR d (memoria[d] = acumulador)
//   4 IMPRIMIR d                          0 PARAR
#include <iostream>   // cout
#include <vector>     // vector, que aquí hace de memoria del ordenador

using namespace std;

// vector<int>& : la CPU trabaja sobre la memoria ORIGINAL (por referencia), no sobre una copia,
// porque la instrucción GUARDAR tiene que escribir en ella.
void cpu(vector<int>& memoria) {
    int contador_programa = 0;  // UC: dirección de la siguiente instrucción
    int acumulador = 0;         // registro donde la ALU deja los resultados
    bool parada = false;        // bandera: se pone a true al leer la instrucción 0 (PARAR)

    while (!parada) {
        int instruccion = memoria[contador_programa];   // 1. CAPTAR
        contador_programa = contador_programa + 1;      // el PC ya apunta a la siguiente
        // Con 207: 207 / 100 = 2 (división entera) y 207 % 100 = 7 (el resto).
        int codigo = instruccion / 100;                  // 2. DECODIFICAR
        int direccion = instruccion % 100;

        if (codigo == 1) {                               // 3. EJECUTAR
            acumulador = memoria[direccion];
        } else if (codigo == 2) {
            acumulador = acumulador + memoria[direccion];  // la ALU suma
        } else if (codigo == 3) {
            memoria[direccion] = acumulador;
        } else if (codigo == 4) {
            cout << "  SALIDA: " << memoria[direccion] << "\n";
        } else {
            parada = true;   // código 0: la condición del while será falsa y la CPU se detiene
        }
        // contador_programa - 1 porque ya lo avancé al captar: así imprimo la instrucción que acabo de ejecutar.
        cout << "  PC=" << contador_programa - 1 << " codigo=" << codigo
                  << " dir=" << direccion << " acumulador=" << acumulador << "\n";
    }
}

int main() {
    // 106 = CARGAR 6, 207 = SUMAR 7, 308 = GUARDAR 8, 408 = IMPRIMIR 8, 0 = PARAR.
    //                        instrucciones            | datos (direcciones 6, 7, 8)
    vector<int> suma = {106, 207, 308, 408, 0, 0,   20, 22, 0};
    cout << "Programa 1: sumar memoria[6] + memoria[7]\n";
    cpu(suma);

    // "contador++" son exactamente tres instrucciones: CARGAR, SUMAR 1, GUARDAR.
    // Un hilo puede ser interrumpido entre cualquiera de ellas (tema 04).
    // 106 = CARGAR 6 (41), 207 = SUMAR 7 (el 1), 306 = GUARDAR en 6, 406 = IMPRIMIR 6.
    vector<int> incremento = {106, 207, 306, 406, 0, 0,   41, 1, 0};
    cout << "Programa 2: contador++ (contador en memoria[6])\n";
    cpu(incremento);
    return 0;   // 0 = el programa ha terminado bien
}
