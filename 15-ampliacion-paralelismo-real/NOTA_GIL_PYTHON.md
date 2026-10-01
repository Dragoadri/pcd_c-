# Nota de una página: por qué el mismo código en Python no escala igual (el GIL)

**Contexto.** El Tema 4 heredado medía "contar hasta 200 millones" dos veces, primero seguido y luego con dos hilos, en Python. Con hilos no tardaba la mitad: tardaba lo mismo o más (26 s frente a 52 s en las transparencias). En C++ el mismo experimento con `thread` sí reparte el trabajo entre núcleos (ejemplo `01_suma_paralela_speedup.cpp`).

**Medido en esta máquina** (i7-11800H, contar hasta 50 millones dos veces):

| Versión | Secuencial | 2 hilos | 2 procesos |
|---|---|---|---|
| Python 3.12 (`threading` / `multiprocessing`) | 3,2 a 3,5 s | 2,6 a 2,8 s | 1,4 a 1,6 s |
| C++20 a -O0 (`thread`) | 0,040 s | 0,020 s | |

En Python moderno los hilos ya no empeoran tanto como en las transparencias (aquellas cifras eran de versiones antiguas del intérprete), pero siguen **sin dividir el tiempo por dos**. Los procesos sí.

**Qué es el GIL, dicho bien.** El *Global Interpreter Lock* es un candado dentro del intérprete CPython que solo deja ejecutar código Python a **un hilo a la vez**. Existe para proteger el estado interno del intérprete, sobre todo el contador de referencias de cada objeto con el que se decide cuándo liberar memoria.

Dos correcciones al material original:

* El GIL **no** es "un módulo que gestiona las referencias a las variables": es un candado que protege al intérprete mientras hace esa gestión.
* El GIL **no** sustituye a los mutex del programador. Garantiza que el intérprete no se rompa, no que tu programa sea correcto: `contador += 1` desde dos hilos sigue siendo leer, sumar y escribir, y el intérprete puede cambiar de hilo entre medias. En Python también hacen falta `Lock` y `Semaphore`, exactamente igual que en C++.

**Por qué C++ no lo tiene.** En C++ no hay intérprete: el compilador genera código máquina y cada `thread` es un hilo del sistema operativo que el planificador puede poner en otro núcleo. Nada impide que dos hilos ejecuten a la vez. Por eso escala, y por eso **toda** la protección (mutex, semáforos, atómicos) es responsabilidad nuestra.

**Las salidas en Python.**

* Trabajo que espera (E/S, red, `sleep`): el hilo suelta el GIL mientras espera, así que `threading` sí ayuda. Es el caso "función de espera" del Tema 4: 20 s en secuencial, 10 s con hilos.
* Trabajo de CPU: `multiprocessing`. Cada proceso tiene su propio intérprete y su propio GIL; a cambio no comparten memoria y hay que comunicarlos, igual que con `fork()` en C++ (ejemplos 04 a 06).
* Python 3.13 incluye una compilación experimental **sin GIL** (*free-threaded*, PEP 703). No es la instalación por defecto, y precisamente al quitar el GIL aparecen en Python los mismos problemas de sincronización que estudiamos en C++.

**La cocina del Tema 4, resumida.**

| Modelo | Cocina | Cuándo conviene |
|---|---|---|
| Secuencial | Una cocina, un cocinero, diez platos | Sin prisa, o trabajo imposible de dividir |
| Hilos (en Python) | Una cocina, diez cocineros, un solo fogón a la vez (el GIL) | Trabajo con esperas: E/S, red |
| Hilos (en C++) | Una cocina, diez cocineros, todos los fogones | Trabajo de CPU y de E/S, pero hay que coordinar quién coge cada sartén |
| Procesos | Diez cocinas, un cocinero en cada una | Trabajo de CPU aislado; cuesta más memoria y comunicarse |
