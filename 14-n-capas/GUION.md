# Guion de clase: 14 aplicaciones en N capas

**Duración estimada:** 150 minutos (una sesión completa y media de otra, sesiones de ~110 min)
**Entra en examen:** Parcial 2

## Objetivo de la sesión

Que el alumno separe un programa en tres capas (presentación, lógica de negocio y acceso a datos) y guarde datos de verdad en SQLite desde C++. De paso, dos ideas que conectan con el resto de la asignatura: la inyección SQL (seguridad) y que una base de datos es también un recurso compartido con su propia sección crítica.

Importante: en esta asignatura las capas son **funciones dentro del mismo programa**. No separamos front y back con sockets entre ellos; eso sería un sistema distribuido y se deja para la práctica final y Proyectos II.

## Antes de empezar (preparación del profesor)

* Compilar la carpeta una vez antes de clase: la primera compilación de `sqlite3.c` tarda unos segundos y es mejor no esperar delante de todos.
* Tener leído el `README.md` de esta carpeta (por qué SQLite viene dentro del proyecto).
* Borrar los `.db` que queden de ensayos (`tareas.db`, `tareas_demo.db`, `usuarios_juguete.db`, `hilos.db`) para empezar limpio.
* Opcional: abrir el panel *Database* de CLion para enseñar las tablas visualmente.

## Pizarra

1. Tres cajas apiladas: **Presentación** (arriba), **Lógica de negocio** (medio), **Acceso a datos** (abajo), y debajo un cilindro **SQLite (fichero .db)**. Flechas solo entre cajas vecinas. Tachar con una X una flecha de Presentación directa al cilindro.
2. Al lado de cada caja, lo que puede hacer: `cout/cin` | `reglas: no vacía, 60 letras, número válido` | `SQL`. Y lo que NO puede hacer cada una.
3. Las cuatro llamadas de SQLite en columna: `open`, `exec` (sin resultados), `prepare / step / column / finalize` (con resultados), `close`.
4. Para el ejemplo 02, escribir la consulta con el hueco y rellenarla a mano con `' OR '1'='1` delante de la clase, subrayando dónde se cierra la comilla.
5. Para el ejemplo 06, un cilindro con un candado y cuatro hilos haciendo cola: "SQLite ya tiene su propia sección crítica: el fichero entero".

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | `README.md` | 10 | SQLite no viene con C++. Amalgamación, `add_library` y dónde aparece el `.db`. |
| 1 | `01_sqlite_crear_insertar_consultar.cpp` | 25 | El puntero a la conexión y por qué `open` recibe `&conexion`. `exec` para lo que no devuelve filas; `prepare/step` para lo que sí. El bucle `while (step == SQLITE_ROW)` se controla solo con su condición. Error provocado de columna inexistente. |
| 2 | `02_inyeccion_sql_concatenando.cpp` | 15 | Login construido pegando texto. Leer en voz alta el SQL que imprime. En un grado de Ciberseguridad esto no es anécdota. |
| 3 | `03_consulta_preparada_con_bind.cpp` | 15 | Mismo login con `?` y `bind`. El dato nunca forma parte de la sentencia. Regla: con datos del usuario, siempre `bind`. |
| 4 | `04_tareas_todo_mezclado.cpp` | 15 | Port de la solución guiada del Tema 3 sin capas. Funciona. Preguntar qué habría que tocar para cambiar la consola por una web. `while True + break` sustituido por la bandera `seguir`. |
| 5 | `05_tareas_en_tres_capas.cpp` | 35 | Teclear de abajo arriba: datos, negocio, presentación. Hacer notar que `main` ya no contiene ni una línea de SQL y que la capa de datos nunca imprime. Vectores paralelos `ids` y `textos` en lugar de structs (norma). |
| 6 | `06_hilos_escriben_en_sqlite.cpp` | 25 | Enlace con concurrencia: una conexión por hilo, SQLite deja un solo escritor a la vez, `database is locked` y `busy_timeout`. Cada hilo escribe sus fallos en su casilla (Bernstein). |
| | Cierre y ejercicios | 10 | Ver abajo. |

## Preguntas a la clase antes de ejecutar

* **01**: "Ejecuto el programa dos veces. ¿Cuántas tareas habrá?" Dos, porque borra la tabla al empezar. "¿Y si quito el `DELETE`?" Se acumulan de ejecución en ejecución: la base de datos sobrevive al programa, las variables no.
* **01**: "¿Dónde se crea `tareas_demo.db`?" Casi todos dirán "junto al `.cpp`". Ejecutar y leer la ruta que imprime.
* **02**: "Con la clave `' OR '1'='1`, ¿entra?" Dejar que voten. Luego leer el SQL que se ha ejecutado: `AND` se evalúa antes que `OR`, así que `'1'='1'` basta.
* **03**: "Con `bind`, ¿qué busca exactamente SQLite?" Una clave que sea literalmente el texto `' OR '1'='1`. No existe, se deniega.
* **04**: "Si el día de mañana la lista de tareas se guarda en un fichero de texto en vez de SQLite, ¿qué partes del `main` cambian?" Todas las opciones del menú.
* **05**: "Repito la pregunta con el programa en capas." Solo las funciones `datos_*`. Presentación y negocio no se enteran.
* **05**: "¿Por qué el usuario borra la tarea número 1 y no la de id 1?" Porque los ids tienen huecos cuando se borra; la traducción número a id es una regla de negocio.
* **06**: "Cuatro hilos, cincuenta inserciones cada uno, sin `busy_timeout`. ¿Cuántas filas se guardan?" Suelen decir 200. Salen entre 1 y 34 (medido). Con `busy_timeout`, 200 siempre.
* **06**: "¿Hace falta un `mutex` alrededor de cada `INSERT`?" No: cada hilo tiene su conexión y SQLite ya protege el fichero. Lo que sí protegemos con mutex es la consola.

## Errores típicos de los alumnos

* Mezclar `cin >> opcion` con `getline`: el salto de línea que deja `>>` hace que el siguiente `getline` lea una cadena vacía. Por eso en esta carpeta todo se lee con `getline`.
* Olvidar `sqlite3_finalize`: la tabla queda bloqueada y el siguiente `DELETE` o `DROP` falla con `database is locked` sin que haya hilos.
* Pensar que las tareas se han perdido cuando en realidad se ejecutó desde otra carpeta y se creó otro `.db` vacío.
* Construir SQL con `+` "porque es más corto". Es exactamente el ejemplo 02.
* Numerar los huecos de `bind` desde 0 (empiezan en 1) y las columnas de `column` desde 1 (empiezan en 0).
* En la versión en capas, meter un `cout` "de depuración" en la capa de datos o una consulta SQL en el menú: rompe la regla y hay que señalarlo en la corrección.
* Compartir una única conexión entre hilos y luego no entender por qué `sqlite3_errmsg` devuelve el error de otro hilo.
* Cambiar `project()` sin `C` en la práctica final: CMake no sabe compilar `sqlite3.c`.

## Cierre y enlace con la siguiente carpeta

Ejercicios para casa (sin solución en el repositorio):

1. **Agenda de contactos** (ejercicio del Tema 3 original): agregar un contacto con nombre, teléfono y dirección; ver la lista; eliminar un contacto eligiéndolo por su nombre de la lista. Tres capas, consultas preparadas y vectores paralelos para devolver las filas.
2. **Tres capas en tres ficheros**: partir `05_tareas_en_tres_capas.cpp` en `tareas_presentacion.cpp` (con `main`), `tareas_negocio.hpp/.cpp` y `tareas_datos.hpp/.cpp`. En los `.hpp` solo declaraciones de funciones. La presentación incluye solo `tareas_negocio.hpp` y no debe necesitar `#include <sqlite3.h>`: si lo necesita, alguna capa está mal separada. En CMake, un único ejecutable con los tres `.cpp`: `pcd_ejemplo(tareas_presentacion tareas_negocio.cpp tareas_datos.cpp)`.

Enlace: esta estructura (hilos que atienden clientes en la presentación, reglas en negocio y SQLite en datos) es el esqueleto del servidor de la práctica final (carpeta 99). Lo que falta, un servidor concurrente delante, ya se vio en la carpeta 13.
