// =====================================================================
// Tema 14.4  |  Gestor de tareas SIN capas: menú, reglas y SQL mezclados
// En este programa he hecho un gestor de tareas con menú en el que los cout, las reglas y el SQL están mezclados dentro de main.
// Requisitos previos: 03_consulta_preparada_con_bind
// Qué observar al ejecutar: funciona, pero cada opción del menú lleva cout, validación y SQL a la vez.
// Salida determinista: SI (depende solo de lo que se teclee y de lo que ya hubiera en tareas.db)
// Compilar con: cualquier nivel
// =====================================================================
// Material original: "Solución guiada" del Tema 3 (Python en Jupyter), bucle
// while True con break en la opción 4. Sustituido por la bandera 'seguir'
// (norma: sin break). Sin entrada (fin de fichero) el menú sale solo.
// Pregunta para clase: si mañana cambiamos la consola por una web, o SQLite
// por otro sistema, ¿cuántas líneas de este main hay que tocar?
#include <iostream>    // cout, cin
#include <string>      // string, getline
#include <cstdlib>     // atoi
#include <sqlite3.h>   // funciones sqlite3_* de SQLite

using namespace std;

int main() {
    sqlite3* conexion = nullptr;             // puntero a la conexión; open lo rellena
    sqlite3_open("tareas.db", &conexion);    // &conexion: dirección del puntero
    // Los tres nullptr: sin función para filas, sin dato extra y sin mensaje de error.
    sqlite3_exec(conexion, "CREATE TABLE IF NOT EXISTS tareas ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, tarea TEXT NOT NULL)", nullptr, nullptr, nullptr);

    bool seguir = true;                      // bandera: la opción 4 la pone a false y el while termina
    while (seguir) {
        cout << "\n1. Mostrar tareas\n2. Agregar tarea\n3. Eliminar tarea\n4. Salir\n"
                  << "Seleccione una opcion: ";
        string opcion;
        // getline lee una línea entera; si ya no queda entrada, la condición es falsa y el ! la vuelve cierta.
        if (!getline(cin, opcion)) {
            opcion = "4";   // no queda entrada: salimos como si pulsara 4
        }

        if (opcion == "1") {
            sqlite3_stmt* sentencia = nullptr;   // puntero a la sentencia compilada
            // prepare_v2 compila el SQL. -1: hasta el final del texto. &sentencia: SQLite rellena el puntero.
            sqlite3_prepare_v2(conexion, "SELECT tarea FROM tareas ORDER BY id", -1, &sentencia, nullptr);
            int numero = 1;
            cout << "Tareas pendientes:\n";
            while (sqlite3_step(sentencia) == SQLITE_ROW) {   // una vuelta por cada fila que devuelve SQLite
                // Cast (const char*): sqlite3_column_text da unsigned char* y cout lo quiere como texto normal.
                cout << numero << ". " << (const char*) sqlite3_column_text(sentencia, 0) << "\n";
                numero++;
            }
            sqlite3_finalize(sentencia);         // libera la sentencia
        } else if (opcion == "2") {
            cout << "Ingrese la nueva tarea: ";
            string texto;
            getline(cin, texto);
            if (texto.empty()) {                       // regla de negocio... en mitad del menú
                cout << "La tarea no puede estar vacia\n";
            } else {
                sqlite3_stmt* sentencia = nullptr;
                sqlite3_prepare_v2(conexion, "INSERT INTO tareas (tarea) VALUES (?)", -1, &sentencia, nullptr);
                // El texto va al hueco 1. SQLITE_TRANSIENT: SQLite se guarda su propia copia.
                sqlite3_bind_text(sentencia, 1, texto.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(sentencia);         // ejecuta el INSERT
                sqlite3_finalize(sentencia);
                cout << "Se ha agregado la tarea: '" << texto << "'\n";
            }
        } else if (opcion == "3") {
            cout << "Ingrese el numero de tarea a eliminar: ";
            string linea;
            getline(cin, linea);
            int numero = atoi(linea.c_str());     // atoi devuelve 0 si no es un número
            // La tarea número N (como la ve el usuario) es la fila N-1 ordenada por id.
            // OFFSET ? se salta esas filas y LIMIT 1 se queda con la siguiente.
            sqlite3_stmt* sentencia = nullptr;
            sqlite3_prepare_v2(conexion, "DELETE FROM tareas WHERE id = "
                               "(SELECT id FROM tareas ORDER BY id LIMIT 1 OFFSET ?)", -1, &sentencia, nullptr);
            sqlite3_bind_int(sentencia, 1, numero - 1);   // bind_int: pone un entero en el hueco 1
            sqlite3_step(sentencia);
            sqlite3_finalize(sentencia);
            // sqlite3_changes: cuántas filas cambió la última sentencia. 1 = se ha borrado de verdad.
            if (numero >= 1 && sqlite3_changes(conexion) == 1) {
                cout << "Tarea " << numero << " eliminada\n";
            } else {
                cout << "Indice invalido\n";
            }
        } else if (opcion == "4") {
            cout << "Hasta luego!\n";
            seguir = false;                      // el while no dará más vueltas (sin break)
        } else {
            cout << "Opcion no valida. Intente nuevamente.\n";
        }
    }
    sqlite3_close(conexion);   // cierra la base de datos
    return 0;                  // 0 = el programa ha terminado bien
}
