// =====================================================================
// Tema 14.1  |  SQLite desde C++: abrir, crear tabla, insertar y consultar
// En este programa he abierto tareas_demo.db con SQLite, he creado la tabla tareas, he insertado dos filas y las he leído con prepare y step.
// Requisitos previos: 01-repaso-cpp (punteros y paso por referencia)
// Qué observar al ejecutar: la carpeta donde se crea tareas_demo.db, el id que asigna SQLite y las filas leídas.
// Salida determinista: SI (la tabla se vacía al empezar; solo cambia la ruta de trabajo)
// Compilar con: cualquier nivel (enlaza con la biblioteca sqlite3_amalgamacion)
// =====================================================================
// SQLite no es un servidor: es una biblioteca que lee y escribe UN fichero.
// La base de datos entera vive en "tareas_demo.db", en la carpeta de trabajo.
#include <iostream>     // cout
#include <string>       // string
#include <filesystem>   // filesystem::current_path, para ver en qué carpeta estamos
#include <sqlite3.h>    // todas las funciones sqlite3_* (SQLite está escrito en C)

using namespace std;

// Ejecuta SQL que no devuelve filas (CREATE, INSERT, DELETE). true si fue bien.
// sqlite3* es un puntero a la conexión abierta; const string& recibe el texto sin copiarlo y sin poder cambiarlo.
bool ejecutar_sql(sqlite3* conexion, const string& sql) {
    char* mensaje_error = nullptr;   // nullptr: aún no apunta a nada; si hay error, SQLite lo apunta a su texto
    // sql.c_str() da el texto como char*, que es lo que pide una biblioteca de C.
    // Los dos nullptr: no queremos recibir filas. &mensaje_error: dirección del puntero, para que SQLite lo rellene.
    int codigo = sqlite3_exec(conexion, sql.c_str(), nullptr, nullptr, &mensaje_error);
    if (codigo != SQLITE_OK) {       // SQLITE_OK (vale 0) significa que todo fue bien
        cout << "Error SQL: " << mensaje_error << "\n";
        sqlite3_free(mensaje_error);   // el texto lo reservó SQLite: lo libera SQLite
    }
    return codigo == SQLITE_OK;      // devuelve el resultado de la comparación: true o false
}

int main() {
    // sqlite3_libversion: versión de SQLite. current_path: carpeta donde aparecerá tareas_demo.db.
    cout << "SQLite " << sqlite3_libversion() << ", carpeta de trabajo: "
              << filesystem::current_path() << "\n";

    // 1. ABRIR. Solo manejamos un PUNTERO a la conexión. sqlite3_open necesita
    //    la dirección de ese puntero (&conexion) para poder rellenarlo.
    sqlite3* conexion = nullptr;
    if (sqlite3_open("tareas_demo.db", &conexion) != SQLITE_OK) {   // si el fichero no existe, lo crea
        cout << "No se pudo abrir: " << sqlite3_errmsg(conexion) << "\n";   // errmsg: texto del último error
    }

    // 2. SQL SIN RESULTADOS. IF NOT EXISTS: se puede ejecutar mil veces.
    //    Dos textos entre comillas seguidos se pegan en uno solo.
    //    AUTOINCREMENT: el id no lo damos nosotros, lo pone SQLite.
    ejecutar_sql(conexion, "CREATE TABLE IF NOT EXISTS tareas ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, texto TEXT NOT NULL)");
    ejecutar_sql(conexion, "DELETE FROM tareas");   // solo para que la demo sea repetible
    ejecutar_sql(conexion, "INSERT INTO tareas (texto) VALUES ('Estudiar hilos')");
    ejecutar_sql(conexion, "INSERT INTO tareas (texto) VALUES ('Repasar mutex')");
    // sqlite3_last_insert_rowid: el id que SQLite puso a la última fila insertada.
    cout << "Ultimo id asignado: " << sqlite3_last_insert_rowid(conexion) << "\n";
    ejecutar_sql(conexion, "INSERT INTO tareas (prioridad) VALUES (3)");  // error provocado: no existe la columna prioridad

    // 3. CONSULTAR: prepare compila el SQL, step avanza una fila cada vez.
    sqlite3_stmt* sentencia = nullptr;   // puntero a una sentencia ya compilada, lista para ejecutar
    // -1: el SQL llega hasta el final del texto. &sentencia: SQLite rellena el puntero.
    // El último nullptr: no nos interesa si sobra SQL detrás.
    sqlite3_prepare_v2(conexion, "SELECT id, texto FROM tareas ORDER BY id", -1, &sentencia, nullptr);
    // step devuelve SQLITE_ROW mientras quedan filas: el bucle se controla con su condición.
    while (sqlite3_step(sentencia) == SQLITE_ROW) {
        int id = sqlite3_column_int(sentencia, 0);                      // columnas desde 0
        // Cast (const char*): SQLite devuelve unsigned char* y string necesita const char*.
        string texto = (const char*) sqlite3_column_text(sentencia, 1);  // unsigned char* a texto
        cout << "  [" << id << "] " << texto << "\n";
    }
    sqlite3_finalize(sentencia);   // liberar la sentencia; olvidarlo deja la tabla bloqueada

    // 4. CERRAR siempre, incluso si falló al abrir.
    sqlite3_close(conexion);       // cierra la conexión y suelta el fichero
    return 0;   // 0 = el programa ha terminado bien
}
