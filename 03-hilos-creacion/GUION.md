# Guion de clase: 03 hilos, creación

**Duración estimada:** 110 minutos (1 sesión)
**Entra en examen:** Parcial 1 (creación de hilos)

## Objetivo de la sesión

Que el alumno sea capaz de lanzar funciones en hilos con `thread`, pasarles argumentos, guardarlos en un `vector` y esperarlos a todos con `join`. Y, sobre todo, que se lleve dos ideas: el orden de ejecución no está garantizado, y `thread` copia los argumentos (trampa 1).

## Antes de empezar (preparación del profesor)

- Proyecto abierto en CLion con el perfil Debug. Comprobar que `01_primer_hilo` compila y enlaza (si falla con `undefined reference to pthread_create`, falta `Threads::Threads` en el CMake raíz; aquí ya está).
- Tener a mano la diapositiva 7 de UA1_T02 (qué es propio de cada hilo y qué se comparte) y la 14 de UA1_T01 (fork/join).
- Terminal integrado de CLion abierto para lanzar los ejecutables varias veces seguidas (`./cmake-build-debug/bin/07_el_orden_no_esta_garantizado`).

## Pizarra

1. **Proceso y hilos** (analogía de UA1_T02: la empresa y los empleados). Un rectángulo grande "proceso" con: código, variables globales, montículo (compartido). Dentro, tres columnas "hilo": cada una con su pila, su contador de programa y sus variables locales.
2. **Línea de tiempo**: arriba "secuencial", una barra A de 2 s y a continuación una barra B de 2 s (total 4 s). Abajo "con hilos", las barras A y B una encima de la otra (total 2 s).
3. **fork / join**: escribir en dos columnas:
   - `thread h(funcion, args);` = fork (el hilo arranca YA)
   - `h.join();` = espero a que termine
   - Regla de la asignatura: **todo hilo que se crea se une con join**.
4. **TRAMPA 1** en un recuadro rojo que se queda toda la sesión: `thread COPIA los argumentos. Para compartir: ref(variable) o &variable`.
5. Al final, esquema cobegin/coend: bucle que crea, bucle que une.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | (pizarra 1 a 3) | 15 | Proceso frente a hilo, qué comparten. Crear un hilo es barato, crear un proceso no. |
| 1 | `01_primer_hilo.cpp` | 12 | El nombre de la función va sin paréntesis. El hilo empieza al construirse. Comentar el join en directo y ejecutar: el programa aborta. |
| 2 | `02_dos_hilos_y_tiempo.cpp` | 12 | Port del Tema 4 (función de espera). Mismo trabajo, la mitad de tiempo, porque las esperas se solapan. |
| 3 | `03_hilo_con_argumentos.cpp` | 8 | Los argumentos van detrás de la función, en orden. Una misma función, dos hilos distintos. |
| 4 | `04_trampa_1_los_argumentos_se_copian.cpp` | 15 | Pizarra 4. Descomentar el caso A y compilar delante de ellos: enseñar a buscar la línea útil entre el ruido de plantillas. Luego el caso B: compila y main ve 0. |
| 5 | `05_trampa_1_solucion_std_ref.cpp` | 10 | `ref` y puntero: las dos formas de compartir. Advertir que compartir abre la puerta a la carpeta 04. |
| 6 | `06_muchos_hilos_con_vector.cpp` | 12 | Pizarra 5. `push_back(thread(...))`. Dos bucles separados. `this_thread::get_id()`: el hilo principal ya existía (Actividad 0). |
| 7 | `07_el_orden_no_esta_garantizado.cpp` | 8 | Ejecutarlo tres veces seguidas desde el terminal. Todos hacen el mismo trabajo y aun así el orden cambia. |
| 8 | `08_carrera_tortugas_con_hilos.cpp` | 13 | Port del ejemplo del Tema 1. Generador aleatorio local por hilo. Ejercicio en vivo: pasar a 5 tortugas cambiando una constante. |
| 9 | Cierre | 5 | Ver apartado final. |

## Preguntas a la clase antes de ejecutar

- **01_primer_hilo**: ¿Saldrán primero todas las líneas de main y luego las del hilo? *No: se intercalan, porque los dos avanzan a la vez.* ¿Y si borro `hilo.join()`? *El programa aborta al terminar main ("terminate called without an active exception"). Si la salida no es un terminal, a veces ni se ven las líneas anteriores, porque se pierden en el búfer.*
- **02_dos_hilos_y_tiempo**: La parte secuencial tarda 4 s. ¿Cuánto tardará la parte con hilos? *Unos 2 s: las dos esperas ocurren a la vez.* ¿Tardaría 1 s con cuatro hilos? *No, cada tarea sigue durmiendo 2 s; el total es lo que tarda la más lenta.*
- **03_hilo_con_argumentos**: ¿Quién termina antes, Ana (3 veces) o Luis (5 veces)? *Ana, pero sus líneas se intercalan con las de Luis mientras duran las dos.*
- **04_trampa_1 (caso B)**: El hilo suma 5 al contador. ¿Qué imprime main? *0. El hilo ha trabajado sobre una copia.* Suele haber mayoría que dice 5: es el momento de la sesión.
- **05_trampa_1_solucion**: ¿Qué imprimirá tras el hilo con puntero, 5 o 10? *10: los dos hilos han modificado la misma variable, uno después del otro.*
- **06_muchos_hilos_con_vector**: Los hilos se crean 0, 1, 2, 3, 4. ¿En qué orden terminan? *Al revés (4 primero), porque los de id alto duermen menos.* ¿Y los id se repiten entre ejecuciones? *No.*
- **07_el_orden**: ¿Saldrá "0 1 2 3 4 5 6 7" en cada ronda? *No, y además cada ejecución es distinta.*
- **08_tortugas**: ¿Quién ganará? *Imposible saberlo: aleatoriedad más planificación.* ¿Por qué no puedo decir "la tortuga que imprima primero la meta es la ganadora" con seguridad? *Porque llegar y escribir son dos pasos, y entre ellos otro hilo puede escribir antes (se resolverá con un candado en 06).*

## Errores típicos de los alumnos

- Escribir `thread h(saludar());` con paréntesis: llama a la función en el hilo principal y pasa su resultado (void, no compila).
- Olvidar el `join`: el programa aborta al terminar. O hacer `join` de un hilo dos veces: excepción `system_error`.
- **Hacer el join dentro del mismo bucle que crea los hilos**: `for (...) { thread h(f, i); h.join(); }`. Compila y funciona, pero es secuencial: cada hilo espera al anterior. Enseñarlo midiendo el tiempo si aparece.
- Intentar `hilos.push_back(h);` con un hilo con nombre: no compila porque un `thread` no se puede copiar. Usar `hilos.push_back(thread(f, i));`.
- Pasar una variable a una función con `int&` sin `ref` (no compila) y "arreglarlo" quitando el `&` (compila y no comparte). Es la trampa 1; en 04 veremos que además arruina la demostración de las condiciones de carrera.
- Compartir un único generador aleatorio entre todos los hilos.
- Suponer que el hilo creado primero se ejecuta primero.
- En Windows con MinGW o en macOS, olvidar el enlazado con la biblioteca de hilos si montan su propio CMake (en Linux el error es `undefined reference to pthread_create`).

## Nota sobre jthread

Las diapositivas UA1_T02 recomiendan `jthread` (C++20), que hace el join automáticamente en su destructor. En esta asignatura **no se usa**: la norma exige ver el `join` escrito, igual que se exige ver `lock()` y `unlock()`. Se menciona de palabra como el atajo RAII que encontrarán fuera, cuando ya dominen el join explícito.

## Cierre y enlace con la siguiente carpeta

Pregunta final: "En `05_trampa_1_solucion_std_ref` los dos hilos tocaban la misma variable, pero uno después del otro. ¿Qué pasará si los lanzo a la vez y cada uno suma un millón de veces?". No se responde. Es exactamente el primer ejemplo serio de la carpeta 04.
