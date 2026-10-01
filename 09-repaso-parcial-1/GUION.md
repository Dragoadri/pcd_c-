# Guion de clase: 09 repaso del parcial 1

**Duración estimada:** 220 minutos (sesión 1 de ~110 min con los ejercicios en pareja; sesión 2 de ~110 min con el simulacro cronometrado y su corrección)
**Entra en examen:** Es el repaso del Parcial 1 (creación de hilos, candados y semáforos; barreras fuera del examen pero permitidas)

## Objetivo de la sesión

Que el alumno practique con el formato del examen: leer código concurrente, predecir qué puede imprimir, encontrar el error y corregirlo **con el mínimo cambio** y cumpliendo la norma. Los tres ejercicios en pareja cubren los tres bloques del parcial:

| Ejercicio | Bloque que repasa | Error que hay que encontrar |
|---|---|---|
| Impresoras compartidas (01, 02) | Semáforos + mutex | El semáforo limita cuántos, pero no protege la elección de impresora |
| Cuenta bancaria (03, 04) | Mutex | Condición de carrera y "comprobar y modificar" fuera de la sección crítica |
| Tortugas con podio (05, 06) | Creación de hilos + mutex | Crear y unir hilos, registrar un orden compartido, salida por consola |

## Antes de empezar (preparación del profesor)

- Repartir **solo** los ficheros `enunciado`. Las soluciones se proyectan al final de cada ejercicio.
- Imprimir `SIMULACRO_PARCIAL_1.md` **sin el solucionario** (todo lo que hay por encima de "SOLUCIONARIO Y CRITERIOS DE CORRECCIÓN") para la sesión 2.
- Comprobar que los alumnos compilan en perfil Debug: el enunciado 03 va a `-O0` en su target, pero conviene recordarlo.

## Pizarra

1. Mapa del parcial en tres columnas: **hilos** (`thread`, `join`, `ref`), **candados** (`lock`, `unlock`, sección crítica), **semáforos** (`acquire`, `release`, contador, señal).
2. Las cuatro preguntas que hay que hacerse ante cualquier ejercicio:
   1. ¿Qué datos son compartidos y quién escribe?
   2. ¿Dónde empieza y dónde acaba la sección crítica? (incluye la **comprobación**)
   3. ¿Hay que limitar cuántos hilos entran? Entonces semáforo.
   4. ¿Todos los caminos sueltan lo que cogen? (`unlock`, `release`, `join`)
3. Durante el ejercicio 01, la interfoliación de dos empleados en una tabla de dos columnas (E1, E2) con la columna central `libre[0]`.
4. Durante el ejercicio 03, la frase "comprobar y modificar es UNA sección crítica" y la cuenta de 20 interfoliaciones de la pregunta 2.2 del simulacro.

## Secuencia de ficheros

**Sesión 1**

| Orden | Fichero | Min | Qué cuento o qué hacen |
|---|---|---|---|
| 1 | Pizarra: mapa del parcial y las cuatro preguntas | 10 | Repaso rápido, sin código. |
| 2 | `01_enunciado_impresoras_compartidas.cpp` | 20 | Los alumnos, en parejas: ejecutar, explicar el fallo, corregir. Pista si se atascan a los 10 min: "el semáforo cuenta, pero ¿quién protege el vector `libre`?". |
| 3 | `02_solucion_impresoras_compartidas.cpp` | 10 | Proyectar y buscar los `CAMBIO`. Discutir el apartado c (sin semáforo hay interbloqueo). |
| 4 | `03_enunciado_cuenta_bancaria_cajeros.cpp` | 20 | En parejas. Pedir que prueben a proteger solo `saldo -= 10` y que razonen por qué no basta. |
| 5 | `04_solucion_cuenta_bancaria_cajeros.cpp` | 10 | Proyectar. Contar la anécdota de calibración: con mitad de ingresos y mitad de retiradas los errores se compensaban y la diferencia salía casi 0. |
| 6 | `05_enunciado_tortugas_con_podio.cpp` | 25 | En parejas, completar los 4 TODO. Es el más largo porque hay que escribir la creación de hilos. |
| 7 | `06_solucion_tortugas_con_podio.cpp` | 10 | Proyectar. Insistir en leer `podio.size()` dentro del mutex. |
| | Dudas | 5 | |

**Sesión 2**

| Orden | Actividad | Min |
|---|---|---|
| 1 | Simulacro individual y cronometrado (sin solucionario) | 80 |
| 2 | Intercambio de exámenes y corrección entre compañeros con el solucionario proyectado | 25 |
| 3 | Dudas finales | 5 |

(Si el grupo va justo de tiempo, el simulacro se hace en casa y la sesión 2 se dedica solo a la corrección, unos 40 min.)

## Preguntas a la clase antes de ejecutar

- **01:** "El semáforo tiene 3 permisos y hay 3 impresoras. ¿Puede haber dos empleados en la misma?" Sí: el semáforo no sabe qué impresora coge cada uno.
- **02:** "¿El mutex de impresoras tiene que englobar también los 20 ms de imprimir?" No: solo elegir y devolver. Si englobara la impresión, solo imprimiría uno a la vez.
- **03:** "¿Cuánto debería salir la diferencia?" 0. "¿Saldrá siempre el mismo número?" No, y nunca 0.
- **04:** "¿Por qué `total_ingresado` no lleva mutex?" Cada cajero escribe su propia casilla del vector: no hay dato compartido.
- **05:** "Si creamos los hilos pero no protegemos el podio, ¿se verá el error?" Casi nunca: las tortugas rara vez llegan en el mismo instante. Que no se vea no significa que no exista.
- **06:** "¿Puede haber dos `Gana la tortuga`?" No, porque la posición se decide dentro de la misma sección crítica que el `push_back`.

## Errores típicos de los alumnos

- Proteger solo la escritura y dejar la comprobación fuera (`if` antes del `lock`).
- Poner un `return` o una rama `if` que sale sin `unlock()` o sin `release()`.
- Meter el `sleep` o la impresión larga dentro del mutex: correcto pero secuencial, y en el examen se penaliza.
- Usar un mutex para limitar a N (solo deja pasar a uno) o un semáforo para proteger un vector (no impide que dos elijan lo mismo).
- Crear los hilos y unirlos dentro del mismo bucle (`push_back` y `join` seguidos): el programa pasa a ser secuencial.
- Olvidar `ref` al pasar el vector o la variable compartida al hilo.
- Leer una variable compartida fuera del mutex solo "para imprimirla".
- Usar `break` para salir del bucle de búsqueda de impresora en vez de la bandera `elegida == -1`: incumple la norma y el apartado vale la mitad.
- Dar por buena una solución porque "lo he ejecutado cinco veces y sale bien".

## Cierre y enlace con la siguiente carpeta

Tras el simulacro se cierra el primer bloque. El segundo bloque empieza en **10-fundamentos-de-redes**: los hilos vuelven a aparecer en la carpeta 13, cuando un servidor tenga que atender a varios clientes a la vez, y ahí los mutex y semáforos de este parcial protegerán las estructuras compartidas y limitarán las conexiones simultáneas.
