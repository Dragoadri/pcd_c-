// =====================================================================
// Tema 1.3  |  Condicionales y operadores de comparación y lógicos
// En este programa he calculado la calificación de una nota con if / else if / else y he combinado condiciones con &&, || y !.
// Requisitos previos: 02_operaciones_aritmeticas.cpp
// Qué observar al ejecutar: la calificación y los mensajes que salen según los valores.
// Salida determinista: SI (no hay hilos ni aleatoriedad)
// Compilar con: cualquier nivel de optimización
// =====================================================================
// Comparar:  ==  !=  <  <=  >  >=        Combinar:  && (y)   || (o)   ! (no)
#include <iostream>   // cout

using namespace std;

int main() {
    double nota = 6.5;
    int asistencia = 85;     // porcentaje

    // Cadena if / else if / else: se entra SOLO en la primera que se cumpla.
    // En la asignatura no usamos switch (necesita break); se hace así.
    // La condición va entre paréntesis y el código a ejecutar entre llaves { }.
    if (nota >= 9) {
        cout << "Sobresaliente\n";
    } else if (nota >= 7) {        // solo se mira si la anterior fue falsa: aquí nota < 9
        cout << "Notable\n";
    } else if (nota >= 5) {        // aquí ya se sabe que nota < 7
        cout << "Aprobado\n";
    } else {                       // ninguna se ha cumplido: nota < 5
        cout << "Suspenso\n";
    }

    // && : las dos condiciones tienen que ser ciertas.
    if (nota >= 5 && asistencia >= 80) {   // 6.5 >= 5 es cierto Y 85 >= 80 es cierto
        cout << "Puede presentarse a la defensa\n";
    }

    // || : basta con que una sea cierta.   ! : da la vuelta.
    bool tiene_convalidacion = false;
    // !tiene_convalidacion se lee "NO tiene convalidación": vale true porque la variable es false.
    if (nota < 5 || !tiene_convalidacion) {
        cout << "Tiene que entregar la practica\n";
    }

    // ERROR CLÁSICO: if (nota = 5) ASIGNA 5 en vez de comparar.
    // Comparar siempre con ==.
    return 0;   // 0 = el programa ha terminado bien
}
