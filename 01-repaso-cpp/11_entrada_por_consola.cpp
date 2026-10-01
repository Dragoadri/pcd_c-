// =====================================================================
// Tema 1.11  |  Entrada por consola con cin
// En este programa he leído con cin cuántas tortugas corren y, si la lectura falla, he limpiado el error y he dejado el valor por defecto 3.
// Requisitos previos: 10_vector_y_string.cpp
// Qué observar al ejecutar: escribe un número; si escribes letras o no hay entrada, se usa 3.
// Salida determinista: SI (depende solo de lo que escriba el usuario)
// Compilar con: cualquier nivel de optimización
// =====================================================================
#include <iostream>   // cout (pantalla) y cin (teclado)

using namespace std;

int main() {
    int num_tortugas = 3;  // valor por defecto
    int leido = 0;

    // Sin \n al final: el cursor se queda en la misma línea, esperando a que escribas.
    cout << "Cuantas tortugas corren? ";

    // cin >> leido: espera a que el usuario escriba y pulse Intro, y guarda el número en leido.
    // Las flechas apuntan hacia donde van los datos: de cin hacia la variable.
    // (cin >> leido) vale "verdadero" solo si ha podido leer un número.
    if (cin >> leido) {
        num_tortugas = leido;
    } else {
        // Letras, o fin de la entrada (Ctrl+D): no nos quedamos colgados.
        cin.clear();  // quita el estado de error del flujo
        cout << "\n(entrada no valida, uso el valor por defecto)\n";
    }

    cout << "Correran " << num_tortugas << " tortugas\n";
    return 0;   // 0 = el programa ha terminado bien
}
