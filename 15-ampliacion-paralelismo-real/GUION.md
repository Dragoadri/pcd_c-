# Guion de clase: 15 ampliación, paralelismo real

**Duración estimada:** 110 minutos (una sesión)
**Entra en examen:** No (ampliación). Sustituye al Tema 4 original sobre el GIL de Python.

## Objetivo de la sesión

Cerrar el círculo que se abrió en la carpeta 02: allí se vio que con un solo núcleo no hay paralelismo real. Aquí se mide que en C++ los hilos **sí** se ejecutan a la vez en varios núcleos, hasta dónde escala, qué lo frena (núcleos físicos, falso compartido), una forma más cómoda de devolver resultados (`async`) y la alternativa de usar procesos con `fork()`.

## Antes de empezar (preparación del profesor)

* Compilar con el perfil que se vaya a usar. Los ejemplos 01 y 02 se compilan siempre a `-O2` (lo fija su `CMakeLists.txt`), así que da igual Debug o Release.
* Cerrar programas pesados en el ordenador del aula: las medidas de tiempo se estropean con carga.
* Saber cuántos núcleos físicos tiene la máquina del aula (`lscpu`, líneas *Core(s) per socket* y *Thread(s) per core*): `hardware_concurrency` devuelve hilos hardware, no núcleos.
* Los ejemplos 04 a 06 usan `fork()`: **no compilan en Windows** (MinGW). En el aula, Linux o WSL.
* Tener a mano `NOTA_GIL_PYTHON.md` para el final.

## Pizarra

1. Recuperar el dibujo de la carpeta 02 (una CPU, procesos turnándose) y al lado dibujar cuatro CPU con un hilo cada una. Encima: "concurrencia" y "paralelismo".
2. Una tabla vacía `hilos | segundos | aceleración` que se rellena con los números que salgan en directo.
3. Un rectángulo de 64 bytes partido en 8 casillas (`long long`) con 8 hilos apuntando cada uno a la suya: "no comparten dato, sí comparten línea de caché".
4. Dos columnas **Hilos** y **Procesos**: memoria compartida o copiada; `join` o `waitpid`; hacen falta mutex o hace falta comunicarse.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 1 | `01_suma_paralela_speedup.cpp` | 25 | Trabajo de CPU puro. Partir el rango en trozos; cada hilo acumula en local y escribe una vez en su casilla (Bernstein, sin mutex). Rellenar la tabla de la pizarra. Por qué no llega a x16. |
| 2 | `02_falso_compartido.cpp` | 15 | Sin carrera y aun así lento. Caché de 64 bytes. La solución buena es la de siempre: variable local. |
| 3 | `03_async_y_future.cpp` | 20 | Devolver un valor sin `ref`. `get()` hace de `join`. La trampa de no guardar el future. `async` frente a `deferred` con los identificadores de hilo. |
| 4 | `04_fork_primer_proceso.cpp` | 15 | Port del Tema 4. fork devuelve dos veces. El búfer de `cout` se copia: `flush` antes de cada fork. |
| 5 | `05_fork_variables_se_copian.cpp` | 15 | Port del Tema 4 "copia de variables III". El hijo ve 21, el padre 20; con un hilo, 21. |
| 6 | `06_fork_esperar_hijos_waitpid.cpp` | 12 | Varios hijos con un bucle sin `break`. `waitpid` como `join`. Zombis. Código de salida. |
| | `NOTA_GIL_PYTHON.md` | 8 | Por qué en Python el mismo reparto no escalaba. Lectura en voz alta de la tabla de la cocina. |

## Preguntas a la clase antes de ejecutar

* **01**: "Con 2 hilos, ¿cuánto tardará respecto a 1?" La mitad, más o menos. "¿Y con 64 hilos en un ordenador de 16?" Muchos dirán un dieciseisavo de 1/64. No mejora: no hay más núcleos donde ejecutarlos.
* **01**: "¿Por qué no hace falta un mutex si todos escriben en `resultados`?" Cada uno escribe una casilla distinta y nadie lee la de otro hasta después del `join`.
* **02**: "Tres versiones hacen exactamente el mismo cálculo y ninguna tiene carrera. ¿Tardarán lo mismo?" No: la de casillas contiguas es la más lenta (medido: unas 2 veces más).
* **03**: "Si escribo `async(launch::async, sumar_rango, 0LL, N);` sin guardarlo en una variable, ¿se ejecuta en paralelo?" No: el future temporal se destruye en esa línea y su destructor espera.
* **03**: "¿En qué hilo se ejecuta la tarea `deferred`?" En el de `main`, dentro de `get()`.
* **04**: "¿Cuántas veces sale la línea 'fork me ha devuelto'?" Dos. "¿Y si quito los `flush` y ejecuto `./04_fork_primer_proceso > salida.txt`?" La línea 'Antes del fork' aparece dos veces en el fichero: estaba en el búfer copiado.
* **04 (cascada, del Tema 4)**: "Si hago dos `fork()` seguidos y detrás imprimo una línea, ¿cuántas líneas salen?" Cuatro: 2 procesos tras el primero, 4 tras el segundo. Se puede probar añadiendo un segundo fork en clase.
* **05**: "El hijo suma uno a la global. ¿Qué imprime el padre?" 20. Con el hilo, 21.
* **06**: "Los hijos terminan en orden 2, 1, 0. ¿En qué orden los recoge el padre?" 0, 1, 2: `waitpid(hijos[i])` espera a uno concreto.

## Errores típicos de los alumnos

* Pensar que más hilos siempre es más rápido. Pasado el número de núcleos solo se añade coste.
* Medir en perfil Debug y sacar conclusiones de rendimiento. Por eso 01 y 02 fuerzan `-O2`.
* Confundir `hardware_concurrency` con núcleos físicos.
* Acumular dentro del bucle en `resultados[i]` en lugar de en una variable local (falso compartido sin saberlo).
* Llamar dos veces a `get()` sobre el mismo future: tras la primera el future ya no es válido (`valid()` devuelve false) y la segunda es un error (con GCC lanza `future_error`).
* En fork: olvidar que el hijo sigue ejecutando el código que hay detrás, incluido el bucle que crea hijos. Sin la bandera `numero_hijo`, 3 vueltas crean 7 procesos en lugar de 3.
* No esperar a los hijos: la terminal devuelve el prompt y luego aparecen líneas sueltas, y quedan zombis mientras el padre vive.
* Intentar compilar fork en Windows con MinGW.

## Cierre y enlace con la siguiente carpeta

Idea para llevarse: **los hilos comparten memoria y hay que protegerla; los procesos no la comparten y hay que comunicarlos**. Comunicar procesos, en la misma máquina o en otra, es justo lo que hace la carpeta 16 (RPC y reparto de trabajo entre procesos a través de sockets), también de ampliación.
