// =====================================================================
// Tema 5.4  |  Vivacidad y equidad: la inanición
// En este programa he simulado sin hilos una caja de supermercado con dos políticas, para ver que "menos compra primero" deja sin pagar al carro lleno.
// Requisitos previos: 05/03
// Qué observar al ejecutar: con "menos compra primero" el carro lleno no paga nunca; con "orden de llegada" paga en su turno.
// Salida determinista: SI (es una simulación sin hilos para poder razonar la traza)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Analogía del supermercado: una sola caja (recurso compartido).
//  - Seguridad: nunca pagan dos a la vez.          (se cumple)
//  - Vivacidad: la caja siempre atiende a alguien.  (se cumple)
//  - Equidad:   a todos les llega su turno.         (con la política 1 NO)
// Al hilo que nunca consigue el recurso le pasa la INANICIÓN (en material
// antiguo de la asignatura aparece con el nombre de "equitatividad").
// Vectores paralelos en lugar de un struct cliente (norma de la asignatura).
#include <iostream>   // cout
#include <string>     // string y to_string
#include <vector>     // vector

using namespace std;

void simular(bool menos_compra_primero) {
    // Vectores paralelos: la posición i de los tres vectores es el mismo cliente.
    // El cliente 0 es el carro lleno, que ya está en la cola desde el principio.
    vector<string> nombre = {"CarroLleno"};
    vector<int> articulos = {50};
    vector<bool> atendido = {false};

    for (int turno = 1; turno <= 10; turno++) {
        // Cada turno llega un cliente con poca compra (1, 2 o 3 artículos)
        nombre.push_back("Rapido" + to_string(turno));   // push_back añade al final; to_string pasa el número a texto
        articulos.push_back(1 + turno % 3);   // % es el resto de dividir: turno % 3 vale 0, 1 o 2
        atendido.push_back(false);

        int elegido = -1;   // -1 significa "todavía no he elegido a nadie"
        // size() = cuántos clientes hay; (int) lo convierte a int para compararlo con i.
        for (int i = 0; i < (int) nombre.size(); i++) {
            if (!atendido[i]) {
                if (elegido == -1) {
                    elegido = i;   // el primero en llegar de los que esperan
                } else if (menos_compra_primero && articulos[i] < articulos[elegido]) {
                    // Solo con la política 1: si este lleva menos cosas, le cuela.
                    elegido = i;
                }
            }
        }
        atendido[elegido] = true;
        cout << "  Turno " << turno << ": paga " << nombre[elegido] << "\n";
    }
    // Operador ternario (condición ? si_cierto : si_falso): elige qué texto imprimir.
    cout << "  CarroLleno " << (atendido[0] ? "ha pagado" : "SIGUE ESPERANDO: inanicion") << "\n";
}

int main() {
    cout << "Politica 1: menos compra primero\n";
    simular(true);
    cout << "\nPolitica 2: orden de llegada\n";
    simular(false);
    return 0;   // 0 = el programa ha terminado bien
}
