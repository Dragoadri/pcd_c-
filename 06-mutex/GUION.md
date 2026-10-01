# Guion de clase: 06 mutex

**Duración estimada:** 110 minutos (1 sesión)
**Entra en examen:** Parcial 1

## Objetivo de la sesión

Que los alumnos protejan una sección crítica con `mutex`, tomando y soltando el candado **a mano** con `mtx.lock()` y `mtx.unlock()`, y que sepan reconocer los cuatro errores clásicos: no proteger, proteger demasiado, olvidar soltar y tomar dos candados en orden opuesto. Todo con problemas que ya vieron fallar en la carpeta 04 y con el vocabulario de la carpeta 05.

## Antes de empezar (preparación del profesor)

- Compilar en perfil Debug. Los ficheros 01, 02, 08 y 09 van siempre a `-O0` por CMake (con `-O2` la carrera del 01 desaparece y el contador sale bien por casualidad).
- Tener a mano la carpeta 04 compilada por si hay que relanzar el `cout` entremezclado.
- Dejar escrita en una esquina de la pizarra la norma: "candado a mano: `lock()` ... `unlock()`; sin `lock_guard`".

## Pizarra

1. **El candado es el cartel del baño** (analogía de la carpeta 05). Dibujo de tres cajas en vertical: `mtx.lock()` (preprotocolo), sección crítica, `mtx.unlock()` (postprotocolo). Al lado: "si está ocupado, el hilo se BLOQUEA y no gasta CPU" (diferencia con Peterson).
2. **Regla 1: un único candado compartido.** Dibujar dos hilos apuntando al MISMO `mtx`. Si cada hilo tiene su propio candado, no protege nada. Por eso se pasa con `ref`, igual que el contador.
3. **Regla 2: sección crítica mínima.** Dos barras de tiempo de 4 hilos: en una se solapan (trabajo fuera), en otra van en fila (trabajo dentro).
4. **Regla 3: todo camino que toma el candado lo suelta.** Un `if` con dos ramas y un `unlock()` en cada una.
5. **Regla 4: orden global de candados.** Ciclo dibujado: Hilo A tiene mutex1 y quiere mutex2; Hilo B tiene mutex2 y quiere mutex1. Flechas en círculo. Debajo: "siempre mutex1 antes que mutex2".
6. **Comprobar y actuar van juntos.** `if (disponibles > 0) { disponibles--; }` con una llave grande que abarca las dos líneas bajo el candado.
7. **Lo que verán fuera de clase** (al final, escrito pequeño): `lock_guard` y `scoped_lock` son el atajo RAII que suelta el candado solo al salir del ámbito. Aquí no se usan: queremos ver dónde empieza y dónde acaba la sección crítica.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 1 | `01_contador_sin_mutex.cpp` | 8 | Es el contador de la carpeta 04 con medida de tiempo. "¿Os acordáis de cuánto salía?". Ejecutar dos veces. |
| 2 | `02_contador_con_mutex.cpp` | 12 | Añadir `#include <mutex>`, el parámetro `mutex& mtx` y `ref(mtx)`. Marcar con comentarios dónde empieza y dónde acaba la sección crítica. Comparar tiempos: correcto pero mucho más lento. |
| 3 | (pizarra 1 a 3) | 8 | El cartel del baño, un único candado, sección mínima. |
| 4 | `03_seccion_critica_demasiado_grande.cpp` | 12 | Preparar un pedido es trabajo local: fuera del candado. Si lo metemos dentro, cuatro hilos tardan como uno. Los dos resultados son correctos: el fallo es de rendimiento, no de corrección. |
| 5 | `04_cout_con_mutex.cpp` | 8 | La consola es un recurso compartido. Función `imprimir()` con `mtx_consola`: la usaremos el resto del curso. Las líneas salen enteras, el orden sigue siendo libre. |
| 6 | `05_olvidar_unlock.cpp` | 12 | Un camino del `if` no suelta. Explicar por qué usamos `timed_mutex` y `try_lock_for` (para que el programa no se cuelgue en clase). Descomentar el `unlock()` olvidado y borrar el `unlock()` de `main`: el ingreso ya entra. |
| 7 | (pizarra 4 y 5) | 7 | Todo camino suelta. Ciclo de espera entre dos candados. |
| 8 | `06_interbloqueo_dos_mutex.cpp` | 15 | Port del ejemplo de dos mutex del Tema 2. Cada hilo tiene su primer candado y espera el del otro. El `sleep` de 100 ms asegura que los dos llegan a tener el suyo. Los dos detectan el interbloqueo. Recordar de la carpeta 05: es un fallo de vivacidad. |
| 9 | `07_interbloqueo_resuelto_mismo_orden.cpp` | 8 | Un solo cambio en `main`: el mismo orden para los dos. Ya no hace falta `timed_mutex`. |
| 10 | `08_taquilla_cine_sin_mutex.cpp` | 10 | Ejercicio 4 del Tema 2. "Comprobar y después actuar". El `sleep` de 50 microsegundos simula imprimir la entrada. Se venden más de 100. |
| 11 | `09_taquilla_cine_con_mutex.cpp` | 10 | El candado abarca comprobar y restar, y se suelta en las dos ramas. Salen exactamente 100. Cierre con la pizarra 6 y 7. |

## Preguntas a la clase antes de ejecutar

- **01:** "¿Cuánto saldrá esta vez?" Respuesta: menos de 2 000 000 y distinto cada vez.
- **02:** "¿Saldrá 2 000 000? ¿Tardará lo mismo, más o menos?" Casi todos aciertan el total; pocos esperan que tarde unas cien veces más. Respuesta: correcto siempre, y mucho más lento porque cada `lock`/`unlock` cuesta y los hilos se pelean por el candado.
- **02 (variante):** "¿Y si cada hilo creara su propio `mutex` dentro de la función?" Respuesta: no protegería nada, cada uno tendría su cartel.
- **03:** "Cuatro hilos, cien pedidos de 2 ms. ¿Cuánto tarda cada versión?" Respuesta: unos 50 ms la mínima y unos 200 ms la grande: la grande es un programa secuencial disfrazado.
- **04:** "¿El candado hace que los hilos escriban en orden 0, 1, 2, 3?" Respuesta: no, solo que cada línea salga entera.
- **05:** "¿Qué le pasa al ingreso después de un retiro rechazado?" Respuesta: no puede entrar nunca; sin `try_lock_for` se quedaría colgado.
- **06:** "Cada hilo necesita los dos candados. ¿Terminarán su trabajo?" Respuesta: ninguno: interbloqueo.
- **07:** "Si los dos piden primero mutex1, ¿puede haber ciclo?" Respuesta: no; uno espera en el primer candado sin tener nada del otro.
- **08:** "Hay 100 entradas y ocho taquillas. ¿Cuántas se venden?" Respuesta: más de 100 y `disponibles` en negativo.
- **09 (antes de teclear):** "¿Basta con poner el candado solo alrededor de `disponibles--`?" Respuesta: no, la comprobación quedaría fuera y se seguiría vendiendo de más.

## Errores típicos de los alumnos

- Declarar el `mutex` como variable local dentro de la función del hilo: cada hilo tiene su propio candado y no se protege nada.
- Pasar el candado al hilo sin `ref`: no compila (`mutex` no se puede copiar) y el mensaje de error es largo y confuso. Leer la primera línea que menciona `mutex`.
- Olvidar el `unlock()` en una rama del `if`, o salir de la función con `return` antes del `unlock()`.
- Hacer `unlock()` dos veces, o `unlock()` desde un hilo que no tomó el candado: comportamiento indefinido.
- Tomar el mismo `mutex` dos veces seguidas desde el mismo hilo: se bloquea contra sí mismo.
- Meter `sleep`, entrada/salida o cálculos largos dentro de la sección crítica.
- Proteger solo la escritura y dejar la lectura o la comprobación fuera (taquilla).
- Tomar varios candados en órdenes distintos en funciones distintas.
- Pensar que con candado "va en paralelo igual": la parte protegida va en fila.

## Notas para el profesor

- `atomic<int>` es una alternativa sin candado para un contador simple (`contador++` pasa a ser atómico). No sirve para comprobar y actuar: `if (entradas > 0) entradas--;` sigue fallando aunque cada operación sea atómica, porque la combinación no lo es. Merece una frase en clase, sin fichero.
- Problema de lectores y escritores (Tema 2): con un solo `mutex` los lectores se esperan entre sí aunque no haga falta. Existe `shared_mutex`, fuera del temario.
- Las diapositivas UA1_T04 dicen "usa siempre `lock_guard`, nunca `lock()`/`unlock()` a mano". En esta asignatura se hace a propósito al revés hasta que sepan ver la sección crítica; conviene decirlo si algún alumno ha leído las diapositivas.

## Cierre y enlace con la siguiente carpeta

Resumen en la pizarra: un candado compartido, sección mínima, todo camino suelta, orden global, comprobar y actuar juntos. Pregunta de salida: "¿Y si en vez de uno quiero dejar entrar a cuatro a la vez, por ejemplo cuatro mesas de un restaurante?". El mutex solo deja pasar a uno: eso lo resuelve el semáforo, que es un contador protegido (carpeta `07-semaforos`).
