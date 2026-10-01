// =====================================================================
// Tema 14.5  |  El mismo gestor de tareas separado en tres capas
// En este programa he repartido el gestor de tareas del 04 en tres capas: funciones presentacion_*, negocio_* y datos_*, cada una con su trabajo.
// Requisitos previos: 04_tareas_todo_mezclado
// Qué observar al ejecutar: se comporta igual que el 04; lo que cambia es el código, no la salida.
// Salida determinista: SI (depende solo de lo que se teclee y de lo que ya hubiera en tareas.db)
// Compilar con: cualquier nivel
// =====================================================================
// Regla de oro: cada capa habla solo con la de al lado.
//   PRESENTACIÓN  -> cout y cin. Nunca SQL.
//   NEGOCIO       -> reglas (no vacía, máximo 60 letras, número válido). Ni SQL ni cout.
//   DATOS         -> SQL. Nunca imprime.
// Material original: Tema 3 (Python, while True + break). Bandera 'seguir' (norma: sin break).
#include <iostream>    // cout, cin
#include <string>      // string, getline
#include <vector>      // vector
#include <cstdlib>     // atoi
#include <sqlite3.h>   // funciones sqlite3_* de SQLite

using namespace std;

// ======================= CAPA DE ACCESO A DATOS =======================
sqlite3* conexion_bd = nullptr;   // solo la tocan las funciones datos_*

void datos_abrir() {
    sqlite3_open("tareas.db", &conexion_bd);   // &conexion_bd: open rellena el puntero global
    // Crea la tabla si no existe. Los nullptr: sin filas y sin mensaje de error.
    sqlite3_exec(conexion_bd, "CREATE TABLE IF NOT EXISTS tareas ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, tarea TEXT NOT NULL)", nullptr, nullptr, nullptr);
}

void datos_cerrar() {
    sqlite3_close(conexion_bd);   // cierra la base de datos
}

// const string&: recibe el texto sin copiarlo y sin poder cambiarlo.
void datos_insertar(const string& texto) {
    sqlite3_stmt* sentencia = nullptr;   // puntero a la sentencia compilada
    sqlite3_prepare_v2(conexion_bd, "INSERT INTO tareas (tarea) VALUES (?)", -1, &sentencia, nullptr);
    sqlite3_bind_text(sentencia, 1, texto.c_str(), -1, SQLITE_TRANSIENT);   // el texto va al hueco 1; SQLite se copia el texto
    sqlite3_step(sentencia);             // ejecuta el INSERT
    sqlite3_finalize(sentencia);         // libera la sentencia
}

// Rellena dos vectores paralelos: ids[i] es el id de la tarea textos[i].
// vector<int>& y vector<string>&: son los vectores del que llama, así se rellenan "hacia fuera".
void datos_leer_todas(vector<int>& ids, vector<string>& textos) {
    sqlite3_stmt* sentencia = nullptr;
    sqlite3_prepare_v2(conexion_bd, "SELECT id, tarea FROM tareas ORDER BY id", -1, &sentencia, nullptr);
    while (sqlite3_step(sentencia) == SQLITE_ROW) {        // una vuelta por fila
        ids.push_back(sqlite3_column_int(sentencia, 0));   // push_back: añade al final del vector
        // Cast (const char*): SQLite da unsigned char* y el vector guarda string.
        textos.push_back((const char*) sqlite3_column_text(sentencia, 1));
    }
    sqlite3_finalize(sentencia);
}

void datos_borrar(int id) {
    sqlite3_stmt* sentencia = nullptr;
    sqlite3_prepare_v2(conexion_bd, "DELETE FROM tareas WHERE id = ?", -1, &sentencia, nullptr);
    sqlite3_bind_int(sentencia, 1, id);   // bind_int: pone un entero en el hueco 1
    sqlite3_step(sentencia);
    sqlite3_finalize(sentencia);
}

// ====================== CAPA DE LÓGICA DE NEGOCIO ======================
// Devuelven true/false y un mensaje; decidir CÓMO se enseña es de la presentación.
// string& mensaje (sin const): la función escribe ahí el mensaje para quien la llama.
bool negocio_agregar_tarea(const string& texto, string& mensaje) {
    bool valida = !texto.empty() && texto.size() <= 60;   // no vacía Y como mucho 60 caracteres (size: longitud)
    if (valida) {
        datos_insertar(texto);
        mensaje = "Se ha agregado la tarea: '" + texto + "'";
    } else {
        mensaje = "La tarea debe tener entre 1 y 60 caracteres";
    }
    return valida;
}

void negocio_listar_tareas(vector<string>& textos) {
    vector<int> ids;               // el usuario no ve ids, ve números 1, 2, 3...
    datos_leer_todas(ids, textos);
}

bool negocio_eliminar_tarea(int numero, string& mensaje) {
    vector<int> ids;
    vector<string> textos;
    datos_leer_todas(ids, textos);
    // Cast (int): size() devuelve un número sin signo. Válido si está entre 1 y el número de tareas.
    bool valido = numero >= 1 && numero <= (int) ids.size();
    if (valido) {
        datos_borrar(ids[numero - 1]);  // traduce "número en pantalla" a "id en la tabla"
        mensaje = "Se ha eliminado la tarea: '" + textos[numero - 1] + "'";
    } else {
        mensaje = "Indice invalido";
    }
    return valido;
}

// ========================= CAPA DE PRESENTACIÓN =========================
// Devuelve false si ya no hay entrada (fin de fichero).
bool presentacion_preguntar(const string& pregunta, string& respuesta) {
    cout << pregunta;
    return (bool) getline(cin, respuesta);   // getline lee una línea; convertido a bool, false si no queda entrada
}

void presentacion_mostrar_tareas() {
    vector<string> textos;
    negocio_listar_tareas(textos);   // al volver, textos ya viene relleno
    cout << "Tareas pendientes:\n";
    for (int i = 0; i < (int) textos.size(); i++) {
        cout << i + 1 << ". " << textos[i] << "\n";   // i + 1: el usuario cuenta desde 1, el vector desde 0
    }
}

int main() {
    datos_abrir();   // en un programa mayor lo haría negocio_iniciar(); aquí basta
    bool seguir = true;                // bandera: la opción 4 la pone a false y el while termina
    while (seguir) {
        string opcion, texto, mensaje;
        cout << "\n1. Mostrar tareas\n2. Agregar tarea\n3. Eliminar tarea\n4. Salir\n";
        if (!presentacion_preguntar("Seleccione una opcion: ", opcion)) {
            opcion = "4";              // no queda entrada: salimos como con la opción 4
        }
        if (opcion == "1") {
            presentacion_mostrar_tareas();
        } else if (opcion == "2") {
            presentacion_preguntar("Ingrese la nueva tarea: ", texto);
            negocio_agregar_tarea(texto, mensaje);   // mensaje vuelve relleno por referencia
            cout << mensaje << "\n";
        } else if (opcion == "3") {
            presentacion_preguntar("Ingrese el numero de tarea a eliminar: ", texto);
            negocio_eliminar_tarea(atoi(texto.c_str()), mensaje);   // atoi: texto a int (0 si no es un número)
            cout << mensaje << "\n";
        } else if (opcion == "4") {
            cout << "Hasta luego!\n";
            seguir = false;            // el while no dará más vueltas (sin break)
        } else {
            cout << "Opcion no valida. Intente nuevamente.\n";
        }
    }
    datos_cerrar();
    return 0;   // 0 = el programa ha terminado bien
}
