# Guion de clase: 05 conceptos y exclusión mutua

**Duración estimada:** 110 minutos (1 sesión), más la lectura de los intentos en casa
**Entra en examen:** Parcial 1 el vocabulario (ficheros 01 a 04). Los intentos (05 a 07 y `LECTURA_cuatro_intentos.md`) son lectura, no examen.

## Objetivo de la sesión

Ponerle nombre a lo que los alumnos ya han visto fallar en pantalla en la carpeta 04: el orden cambia en cada ejecución (**interfoliación**), `contador++` pierde incrementos (**atomicidad**), las transferencias descuadran el total (**seguridad**, invariante roto) y hay hilos que nunca consiguen su turno (**vivacidad**, **equidad**, **inanición**). Sin este vocabulario no se puede discutir el mutex ni el semáforo.

Regla de la sesión: primero se recuerda el fallo de la carpeta 04, después se da el nombre, después se escribe la definición en la pizarra. Nunca al revés.

## Antes de empezar (preparación del profesor)

- Tener compilada la carpeta 04 para relanzar `contador compartido` y `transferencias` si hace falta refrescar.
- Compilar esta carpeta en perfil Debug: los ficheros 02, 03 y 07 van a `-O0` por CMake.
- Si el aula tiene máquinas virtuales de 1 núcleo: el 02 falla igual (el `yield()` fuerza la ventana), el 03 puede dar menos estados prohibidos.

## Pizarra

Escribir en este orden, dejando cada bloque a la vista toda la sesión:

1. **Interfoliación.** Dos columnas: `A1 A2 A3` y `B1 B2 B3`. Debajo, dos mezclas distintas. Fórmula `(n1 + n2)! / (n1! · n2!)` y el número 20. Frase: "correcto = correcto en TODAS las mezclas".
2. **Atomicidad.** `contador++` descompuesto en `cargar / sumar / almacenar`. Tabla de traza con dos hilos donde los dos leen 41 y los dos escriben 42. Frase: "atómico = indivisible frente a los demás hilos, no una sola línea".
3. **Abstracción.** Tres líneas: estado = valores de variables + posición de cada hilo; no suponemos velocidades; el planificador es un adversario.
4. **Condiciones de Bernstein** (UA1_T01, sin programa, se resuelve en la pizarra). `L(S)` conjunto de lectura, `E(S)` conjunto de escritura. S1 y S2 pueden ir a la vez si `L(S1) ∩ E(S2) = ∅`, `E(S1) ∩ L(S2) = ∅` y `E(S1) ∩ E(S2) = ∅`. Ejemplos para la clase: `a = x + y` con `b = z * 2` (sí); `a = x + y` con `x = a - 1` (no, dependencia de flujo y antidependencia); `a = x + y` con `a = 7` (no, dependencia de salida). Analogía de los dos cocineros: cada uno con su tabla (sí); uno corta la cebolla que el otro necesita (flujo); los dos echan sal al mismo puchero (salida); uno lee la receta mientras otro la reescribe (antidependencia).
5. **Corrección.** Parcial (si termina, termina bien) y total (además termina). Invariante: afirmación cierta en todos los estados.
6. **Dos columnas: SEGURIDAD | VIVACIDAD.**
   - Seguridad: "nunca ocurre algo malo". Exclusión mutua, ausencia de interbloqueo, invariantes intactos. Contraejemplo: una traza finita.
   - Vivacidad: "algo bueno acaba ocurriendo". Progreso, ausencia de inanición. Contraejemplo: una traza infinita.
   - Analogía del ascensor (UA1_T01): las puertas nunca se abren entre plantas (seguridad); si pulsas, acaba llegando (vivacidad); un ascensor apagado es segurísimo y no sirve (seguridad sin vivacidad); que no suba siempre al mismo vecino (equidad).
7. **Equidad e inanición.** Equidad débil y fuerte en una línea cada una. La equidad la pone el planificador, no tu código. Inanición: el sistema progresa pero un hilo concreto no entra nunca.
8. **Sección crítica.** Dibujo del baño del avión: preprotocolo (girar el cartel), sección crítica, postprotocolo (volver a girarlo). Cuatro requisitos: exclusión mutua, progreso, espera limitada, sin suposiciones.
9. **Interbloqueo | livelock | inanición** en tres columnas con las analogías del cruce, del pasillo y de la puerta del cine.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | (pizarra 1 a 3) | 15 | Recordar la carpeta 04: "¿por qué salía distinto cada vez?". Barajar dos mazos. |
| 1 | `01_interfoliacion_enumerar_mezclas.cpp` | 12 | No hay hilos: enumeramos lo que el planificador PODRÍA hacer. La recursión elige en cada paso qué hilo avanza. Dentro de cada hilo el orden se respeta. |
| 2 | `02_atomicidad_incremento_en_tres_pasos.cpp` | 12 | Escribimos a mano los tres pasos de `contador++`. El `yield()` agranda la ventana a propósito. Analogía del cajero: dos personas miran el saldo antes de que la otra reste. |
| 3 | (pizarra 4 y 5) | 12 | Bernstein en la pizarra con los cocineros. Corrección e invariante. |
| 4 | `03_seguridad_invariante_roto.cpp` | 15 | Invariante `saldo_a + saldo_b == 2000`. El observador lo mira sin parar. Un solo estado prohibido basta para decir "inseguro". Explicar por qué `terminado` es `atomic<bool>`: no queremos mezclar dos problemas. |
| 5 | (pizarra 6 y 7) | 10 | Ascensor. Seguridad frente a vivacidad. Equidad. |
| 6 | `04_vivacidad_inanicion_simulada.cpp` | 12 | Supermercado con una caja. Se cumplen seguridad y vivacidad y aun así el carro lleno no paga nunca: inanición. Cambiar la política a orden de llegada lo arregla (espera limitada). |
| 7 | (pizarra 8 y 9) | 10 | Sección crítica y sus cuatro requisitos. Tres fallos de vivacidad. |
| 8 | `05_intento2_...`, `06_intento3_...`, `07_peterson_...` | 12 | **No se teclean.** Se ejecutan ya escritos, se enseña el pseudocódigo en la lectura y se deja como tarea leer `LECTURA_cuatro_intentos.md`. Contar la trampa 3: por qué `atomic` con `memory_order_seq_cst`. |

## Preguntas a la clase antes de ejecutar

- **01:** "Dos hilos de tres instrucciones cada uno. ¿Cuántos órdenes posibles hay?" Suelen decir 6 o 9. Respuesta: 20. "¿Y con tres hilos de cinco?" Respuesta: más de 750 000. Conclusión: no se prueban, se razonan.
- **02:** "Cada hilo suma 10 000. ¿Qué total sale?" Muchos dirán 20 000 porque "cada uno hace lo suyo". Respuesta: cerca de 10 000. "¿Por qué casi exactamente la mitad?" Porque con el `yield()` los dos leen casi siempre el mismo valor.
- **03:** "¿Puede el observador ver una suma distinta de 2000 aunque el programa estuviera bien escrito sin hilos?" Respuesta: sin hilos no hay nadie mirando entre las dos líneas; con hilos, sí. "¿Y el total final cuadrará?" Respuesta: tampoco, se pierden actualizaciones como en el 02.
- **04:** "La caja atiende a alguien en cada turno y nunca a dos a la vez. ¿Está bien?" Respuesta: es segura y tiene vivacidad, pero no es equitativa: inanición.
- **05:** "¿Pueden entrar los dos si cada uno mira antes la bandera del otro?" Respuesta: sí, entre mirar y señalizar hay una ventana.
- **06:** "Y si señalizo primero y luego miro, ¿qué puede pasar?" Respuesta: que los dos señalicen y se esperen para siempre: interbloqueo.
- **07:** "El contador es un `int` normal a `-O0`, como en la carpeta 04. ¿Saldrá bien?" Respuesta: sí, lo protege Peterson. "¿Entonces por qué no lo usamos siempre?" Espera activa y necesidad de `atomic`: mañana, `mutex`.

## Errores típicos de los alumnos

- Creer que con un solo núcleo no hay carreras. Hay que insistir: la carrera existe igual, solo que se manifiesta menos (en la carpeta 04 lo vimos con `taskset -c 0`).
- Confundir atómico con "rápido" o con "una sola línea de código".
- Pensar que si el programa ha funcionado diez veces es correcto. Probar no demuestra corrección: solo demuestra que esa vez no falló.
- Confundir inanición con interbloqueo. En la inanición el sistema progresa; en el interbloqueo no progresa nadie.
- Pensar que un programa parado "está bien porque no rompe nada". Cumple toda la seguridad y ninguna vivacidad.
- Decir que el intento 3 viola la exclusión mutua. No la viola: falla la vivacidad.
- Usar "equidad" e "inanición" como sinónimos: la equidad es la propiedad, la inanición es el fallo cuando falta.

## Cierre y enlace con la siguiente carpeta

Resumen en seis palabras en la pizarra: interfoliación, atomicidad, invariante, seguridad, vivacidad, equidad. Frase de salida: "Peterson explica el problema, `mutex` lo resuelve". La carpeta `06-mutex` empieza relanzando el contador roto y arreglándolo con `mtx.lock()` y `mtx.unlock()`.
