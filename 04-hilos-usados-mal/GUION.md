# Guion de clase: 04 hilos usados mal

**Duración estimada:** 110 minutos (1 sesión)
**Entra en examen:** Parcial 1 (reconocer los síntomas; el vocabulario formal llega en 05)

## Objetivo de la sesión

Enseñar el problema antes que la solución. Los alumnos ven fallar en pantalla programas con hilos que parecen correctos: salida mezclada, sumas que se pierden, dinero que aparece y desaparece, coordinación por tiempos que falla, CPU quemada esperando y memoria que ya no existe. **No se dan nombres ni teoría todavía**: se anotan los síntomas en la pizarra y la carpeta 05 les pone nombre.

Regla de la sesión: **antes de ejecutar cada ejemplo, la clase predice el resultado** (a mano alzada o escrito en un papel). El choque entre la predicción lineal y lo que sale es el aprendizaje.

## Antes de empezar (preparación del profesor)

- Todos los ejemplos de esta carpeta se compilan a `-O0` aunque CLion esté en Release (función `pcd_ejemplo_sin_optimizar` del CMake). La excepción es `09`, a `-O2` a propósito.
- Terminal abierto en `cmake-build-debug/bin` para lanzar un mismo ejemplo varias veces seguidas.
- Tener `htop` (o `top`) instalado para el ejemplo 07.
- Cifras medidas en la máquina de preparación (16 núcleos, GCC 13.3), para contrastar con las del aula:
  - `02`: 50 de 50 experimentos fallan, totales entre 1 000 710 y 1 380 151 de 2 000 000.
  - `02` con un solo núcleo (`taskset -c 0 ./02_contador_compartido_roto`): fallan 14 de 100, casi siempre con 1 000 000 exacto.
  - `04`: 20 de 20 descuadran, total entre 4 099 y 6 647 (debería ser 5 000).
  - `09` (`-O2`): 249 de 250 dan 2 000 000 y una dio 1 000 000.
  - Si en el aula algún portátil es una máquina virtual con 1 CPU, el `02` puede salir bien varias veces: es el ejemplo perfecto de "que no falle no significa que esté bien".

## Pizarra

Una tabla de dos columnas que se va rellenando durante la sesión, **sin nombres técnicos**:

| Síntoma que hemos visto | Ejemplo |
|---|---|
| El texto de los hilos sale mezclado | 01 |
| Una suma pierde incrementos y cada vez da distinto | 02 |
| Sin ref "funciona", pero porque no comparten nada | 03 |
| El dinero total del banco cambia | 04 |
| Esperar "un rato" funciona hasta que el otro tarda más | 05, 06 |
| Esperar preguntando sin parar gasta una CPU entera | 07 |
| Un hilo lee una variable que ya murió | 08 |
| Optimizando, el fallo casi desaparece (pero sigue ahí) | 09 |

Al lado, en el ejemplo 02, dibujar la traza de `contador++` como tres pasos (leer, sumar 1, escribir) de dos hilos en columnas, con un intercalado en el que ambos leen 41 y ambos escriben 42. Solo el dibujo; la palabra "atomicidad" se reserva para 05.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 1 | `01_cout_entremezclado.cpp` | 8 | Nada de teoría: se ejecuta y se mira. Cada `<<` es una escritura suelta. |
| 2 | `02_contador_compartido_roto.cpp` | 15 | Predicción, ejecución, dibujo de la traza en la pizarra. Ejecutar tres veces: nunca da lo mismo. Luego con `taskset -c 0`. |
| 3 | `03_trampa_1_sin_std_ref_parece_que_funciona.cpp` | 10 | La trampa 1 vuelta del revés: sin `ref` los hilos cuentan perfecto porque cada uno tiene su copia. Si en un examen o práctica "no hay carrera", mirad primero si comparten algo. |
| 4 | `04_transferencias_bancarias_rotas.cpp` | 12 | Un caso con datos de verdad. Mover dinero entre cuentas no puede cambiar el total, y cambia. |
| 5 | `05_esperar_con_sleep_parece_funcionar.cpp` | 7 | El camarero adivina cuánto tarda la cocina. Todo sale bien. |
| 6 | `06_esperar_con_sleep_falla.cpp` | 8 | Un solo cambio: la cocina tarda un tiempo variable. Platos crudos. |
| 7 | `07_espera_activa_satura_la_cpu.cpp` | 12 | Lanzarlo con `htop` a la vista: un núcleo al 100 % durante 3 s. Comparar el tiempo de CPU con el de join. |
| 8 | `08_referencia_colgante.cpp` | 15 | Dibujar la pila: marco de `lanzar_hilo` con `dato_local`, se borra al retornar, `calcular_otra_cosa` ocupa el mismo sitio. Comportamiento indefinido. |
| 9 | `09_la_optimizacion_esconde_la_carrera.cpp` | 12 | Mismo código que 02. Enseñar la línea del CMakeLists que lo compila a `-O2`. Contar el caso real de 1 000 000 medido al preparar la clase. |
| 10 | Cierre con la tabla | 11 | Repasar la tabla de síntomas. Anunciar que la próxima sesión les pone nombre. |

## Preguntas a la clase antes de ejecutar

- **01**: Cuatro hilos escriben 30 líneas cada uno. ¿Saldrán 120 líneas limpias? *No: aparecen líneas partidas y trozos de un hilo dentro de la línea de otro. En un terminal se rompen entre 20 y 48 líneas de 120; volcado a fichero, más.*
- **02**: Dos hilos suman un millón cada uno a la misma variable. ¿Qué imprime? *Casi nadie acierta: sale alrededor de 1 000 000 a 1 400 000, distinto en cada repetición.* ¿Y en un solo núcleo? *Casi siempre 2 000 000, pero no siempre: sigue estando mal.*
- **03**: Ahora sin `ref`. ¿Cuánto dirá cada hilo y cuánto main? *Cada hilo 1 000 000 exacto; main, 0.* ¿Por qué no hay carrera? *Porque no comparten nada.*
- **04**: Cinco cuentas de 1000 euros, solo transferencias entre ellas. ¿Cuánto dinero hay al final? *Debería ser 5 000; sale otra cosa, a veces más y a veces menos.* ¿Y si compilo a -O2? *Sigue fallando: aquí el optimizador no puede eliminar el problema.*
- **05**: La cocina tarda 300 ms y el camarero espera 500 ms. ¿Salen bien los 5 platos? *Sí, siempre.* ¿Está bien el programa? *No: funciona porque hemos adivinado el tiempo.*
- **06**: La cocina tarda entre 200 y 800 ms. ¿Cuántos platos crudos? *Alrededor de la mitad, y cambia cada vez.*
- **07**: ¿Cuánto tiempo de CPU gasta el programa esperando 3 s en el bucle? ¿Y esperando con join? *Unos 3 s de CPU con el bucle (cientos de millones de vueltas); prácticamente 0 con join, porque el sistema no le da la CPU mientras espera.*
- **08**: El hilo lee `dato_local`, que valía 42. ¿Qué lee? *Un número basura (en la preparación, entre 21 955 y 25 925 con GCC y en torno a 32 765 con Clang).* ¿Siempre basura? *No se sabe: es comportamiento indefinido.*
- **09**: Es el mismo código que 02, compilado de otra forma. ¿Qué sale? *2 000 000 casi siempre.* ¿Está arreglado? *No: dos hilos siguen escribiendo la misma variable sin protección.*

## Errores típicos de los alumnos

- Pensar que "si mi ordenador tiene un solo núcleo no hay problema" (falso: `taskset -c 0` también falla, solo que menos).
- Pensar que el fallo del 02 es "un bug del compilador" o "de C++". Es del programa.
- Probar el 02 en Release o con `-O2` en su propio CMake, ver 2 000 000 y concluir que no pasa nada. Por eso existe el 09.
- Olvidar `ref` al portar el 02 a su propio proyecto y decir que no consiguen reproducir la carrera (es el 03).
- Creer que añadir un `sleep` "arregla" una carrera o una coordinación. Solo mueve la probabilidad.
- Arreglar el 07 poniendo un `sleep` dentro del bucle de espera: gasta menos CPU pero vuelve a ser coordinar por tiempos.
- En el 08, ver un 42 en su máquina (puede pasar con otro compilador u otras opciones) y pensar que el código es correcto.

## Cierre y enlace con la siguiente carpeta

Con la tabla de síntomas completa, preguntar: "¿Qué tienen en común el 01, el 02 y el 04?" (*varios hilos tocan la misma cosa a la vez*). "¿Y el 05, 06 y 07?" (*un hilo necesita saber cuándo ha terminado otro*). La carpeta 05 da nombre a todo esto (interfoliación, atomicidad, condición de carrera, seguridad, vivacidad) y la 06 trae la primera herramienta para arreglarlo: el candado (mutex).
