// =====================================================================
// Tema 14.2  |  Inyección SQL: el peligro de concatenar texto del usuario
// En este programa he montado un login que pega en el SQL la clave tecleada, para ver cómo ' OR '1'='1 entra sin conocer la clave.
// Requisitos previos: 01_sqlite_crear_insertar_consultar
// Qué observar al ejecutar: la clave correcta entra, la incorrecta no, y ' OR '1'='1 entra sin conocer la clave.
// Salida determinista: SI
// Compilar con: cualquier nivel
// =====================================================================
// Base de datos de juguete, solo para clase. Nunca se guardan claves en claro.
#include <iostream>    // cout
#include <string>      // string
#include <sqlite3.h>   // funciones sqlite3_* de SQLite

using namespace std;

// MAL: la consulta se construye pegando lo que escribe el usuario.
// const string&: recibe los textos sin copiarlos y sin poder cambiarlos.
bool login_concatenando(sqlite3* conexion, const string& nombre, const string& clave) {
    // El + pega textos: lo que teclee el usuario acaba DENTRO de la sentencia SQL.
    string sql = "SELECT COUNT(*) FROM usuarios WHERE nombre = '" + nombre +
                      "' AND clave = '" + clave + "'";
    cout << "  SQL ejecutado: " << sql << "\n";

    sqlite3_stmt* sentencia = nullptr;   // puntero a la sentencia compilada
    // sql.c_str(): el texto como char*. -1: hasta el final. &sentencia: SQLite rellena el puntero.
    sqlite3_prepare_v2(conexion, sql.c_str(), -1, &sentencia, nullptr);
    int coincidencias = 0;
    if (sqlite3_step(sentencia) == SQLITE_ROW) {        // step ejecuta la consulta; COUNT(*) da siempre una fila
        coincidencias = sqlite3_column_int(sentencia, 0);   // columna 0: cuántos usuarios cumplen el WHERE
    }
    sqlite3_finalize(sentencia);         // libera la sentencia
    return coincidencias > 0;            // true si al menos un usuario coincide
}

void probar(sqlite3* conexion, const string& nombre, const string& clave) {
    cout << "Intento con clave [" << clave << "]\n";
    if (login_concatenando(conexion, nombre, clave)) {
        cout << "  -> ACCESO CONCEDIDO\n\n";
    } else {
        cout << "  -> acceso denegado\n\n";
    }
}

int main() {
    sqlite3* conexion = nullptr;
    sqlite3_open("usuarios_juguete.db", &conexion);   // &conexion: dirección del puntero, para que open lo rellene
    // Tres sentencias separadas por ';' en una sola llamada. Los nullptr: sin filas y sin mensaje de error.
    sqlite3_exec(conexion, "CREATE TABLE IF NOT EXISTS usuarios (nombre TEXT, clave TEXT);"
                           "DELETE FROM usuarios;"
                           "INSERT INTO usuarios VALUES ('admin', 'S3creta!');",
                 nullptr, nullptr, nullptr);

    probar(conexion, "admin", "S3creta!");      // legítimo
    probar(conexion, "admin", "1234");          // clave incorrecta
    // El atacante no sabe la clave. Cierra la comilla y añade una condición
    // que siempre es cierta: WHERE ... AND clave = '' OR '1'='1'
    probar(conexion, "admin", "' OR '1'='1");

    sqlite3_close(conexion);   // cierra la base de datos
    return 0;                  // 0 = el programa ha terminado bien
}
