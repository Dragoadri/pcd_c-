# Lectura: los cuatro intentos de exclusión mutua

**Material de lectura. No entra en examen.** Resume la unidad UA1_T04. Sirve para entender por qué existen los candados (mutex) y los semáforos: cada intento arregla el fallo del anterior e introduce uno nuevo. Los intentos 2 y 3 y el algoritmo de Peterson tienen programa en esta carpeta (`05_`, `06_` y `07_`); los intentos 1 y 4 y Test&Set solo se leen.

## 1. El problema

- **Recurso crítico:** el que no admite uso simultáneo (una variable compartida, un fichero, una impresora, una conexión).
- **Sección crítica (SC):** el fragmento de código que usa ese recurso. Solo un hilo puede estar dentro en cada momento.
- **Preprotocolo (sección de entrada):** el hilo pide permiso. Aquí vive todo el algoritmo.
- **Postprotocolo (sección de salida):** el hilo avisa de que ha salido. Olvidarlo bloquea a todos para siempre.

Analogía del baño del avión: un solo baño (recurso crítico), un cartel de "ocupado" que se gira al entrar (preprotocolo) y al salir (postprotocolo). Si alguien sale sin girar el cartel, no vuelve a entrar nadie. Todo el tema consiste en diseñar el cartel: quién lo mira, quién lo gira y en qué orden.

La sección crítica debe ser lo más corta posible: mientras uno está dentro, los demás están parados.

## 2. Requisitos de una solución correcta

1. **Exclusión mutua:** nunca hay dos hilos a la vez en la sección crítica (seguridad).
2. **Progreso:** si no hay nadie dentro y alguien quiere entrar, la decisión no se pospone indefinidamente (vivacidad).
3. **Espera limitada:** un hilo que quiere entrar solo puede ser adelantado un número finito de veces (sin inanición).
4. **Sin suposiciones:** nada sobre el número de núcleos ni sobre la velocidad relativa de los hilos.

Hay que cumplir los cuatro a la vez. El material original de la asignatura generaliza estos requisitos a siete condiciones para N procesos; para dos procesos se reducen a estas cuatro.

## 3. Notación para razonar

- `at(Li)`: el hilo está a punto de ejecutar la instrucción `Li`. Así se habla de "dónde está" cada hilo dentro de un estado.
- **Invariante:** afirmación cierta en todos los estados alcanzables. Se demuestra por inducción: es cierta al inicio y ninguna transición la puede romper.
- **Reducción al absurdo:** se supone el estado prohibido, por ejemplo `at(SC1) y at(SC2)`, y se llega a una contradicción con los invariantes.
- **Vivacidad:** se razona sobre ejecuciones completas: si un hilo espera, hay que demostrar que la condición que lo bloquea acaba cambiando (bajo una hipótesis de equidad del planificador).

## 4. Primer intento: turno estricto

```
turno compartido, inicialmente 1
Proceso P1                     Proceso P2
loop                           loop
  L1: while turno != 1: {}       M1: while turno != 2: {}
  L2: SC1                        M2: SC2
  L3: turno := 2                 M3: turno := 1
  L4: resto1                     M4: resto2
end                            end
```

- **Exclusión mutua: SÍ.** Invariante: `at(L2) implica turno == 1` y `at(M2) implica turno == 2`. Si estuvieran los dos dentro, `turno` valdría 1 y 2 a la vez: absurdo.
- **Progreso: NO.** Obliga a alternar. Si P1 termina o no quiere volver a entrar, P2 entra una vez más y se queda esperando para siempre. Además el hilo rápido va al ritmo del lento.
- **Fallo:** acopla los dos hilos. Un hilo que no participa impide entrar al otro.

## 5. Segundo intento: mirar antes de señalizar

```
c1, c2 = 1 (libre), 0 (dentro)
Proceso P1                     Proceso P2
loop                           loop
  L1: while c2 == 0: {}          M1: while c1 == 0: {}
  L2: c1 := 0                    M2: c2 := 0
  L3: SC1                        M3: SC2
  L4: c1 := 1                    M4: c2 := 1
end                            end
```

- Elimina la alternancia: cada hilo mira la bandera del otro.
- **Exclusión mutua: NO.** Traza que lo demuestra (basta una, es un fallo de seguridad): P1 ejecuta L1 y ve `c2 == 1`; antes de L2, P2 ejecuta M1 y ve `c1 == 1`; P1 hace L2 y entra; P2 hace M2 y entra. Estado `at(L3) y at(M3)`: prohibido.
- **Fallo:** entre comprobar (L1) y señalizar (L2) hay una ventana. Es el fallo más grave de los cuatro. Programa: `05_intento2_banderas_viola_exclusion.cpp`.

## 6. Tercer intento: señalizar primero

```
Proceso P1                     Proceso P2
loop                           loop
  L1: c1 := 0                    M1: c2 := 0
  L2: while c2 == 0: {}          M2: while c1 == 0: {}
  L3: SC1                        M3: SC2
  L4: c1 := 1                    M4: c2 := 1
end                            end
```

- **Exclusión mutua: SÍ.** Invariantes: `c1 == 0` si P1 está en L2, L3 o L4; `c2 == 0` si P2 está en M2, M3 o M4. Suponemos `at(L3) y at(M3)`. P1 pasó L2 viendo `c2 == 1`, luego P2 aún no había hecho M1; entonces P2 hizo M1 después y en M2 tuvo que ver `c1 == 0` (P1 ya estaba en L3), así que no pudo pasar a M3. Contradicción: `not(at(SC1) y at(SC2))`.
- **Progreso: NO.** Traza: P1 hace L1 (`c1 := 0`), P2 hace M1 (`c2 := 0`). Los dos esperan en su bucle una condición que solo el otro puede cambiar: **interbloqueo** (deadlock). Es un fallo de vivacidad: nadie entra, nadie sale.
- Programa: `06_intento3_interbloqueo.cpp` (la espera se acota para que el programa termine).

## 7. Cuarto intento: procesos generosos

```
Proceso P1
loop
  L1: c1 := 0
  L2: while c2 == 0:
  L3:     c1 := 1        (renuncio un momento)
  L4:     esperar
  L5:     c1 := 0        (lo vuelvo a intentar)
  L6: SC1
  L7: c1 := 1
end
```

- Arregla el interbloqueo: cada hilo cede si ve al otro intentándolo.
- **Fallo: livelock.** Si los dos ceden a la vez, esperan lo mismo y vuelven a intentarlo a la vez, repiten el ciclo sin fin: están activos, gastan CPU y ninguno progresa. También cabe la **inanición**: uno puede quedar repitiendo el ciclo mientras el otro entra una y otra vez.

## 8. Interbloqueo, livelock e inanición

| Fallo | Qué pasa | Analogía |
|---|---|---|
| Interbloqueo | Todos parados esperando algo que solo otro puede dar. No gastan CPU (o gastan esperando en vano). | Dos coches en un cruce estrecho, cada uno esperando a que arranque el otro. |
| Livelock | Todos se mueven, cambian de estado, gastan CPU, y nadie avanza. | Dos personas en un pasillo que se apartan hacia el mismo lado una y otra vez. |
| Inanición | El sistema progresa (otros entran), pero un hilo concreto no entra nunca. | En la puerta del cine siempre se cuelan los grupos y tú te quedas el último. |

Los tres son fallos de **vivacidad**. La solución a la inanición es la **espera limitada**: un portero que apunta el orden de llegada.

## 9. Algoritmo de Peterson (la solución)

```
quiere[0], quiere[1] = falso;  turno
entrar(yo):
  otro := 1 - yo
  quiere[yo] := verdadero        (señalizo mi intención)
  turno := otro                  (cedo la preferencia)
  while quiere[otro] y turno == otro: {}
salir(yo):
  quiere[yo] := falso
```

- Banderas para la intención, turno para desempatar. Ceder el turno rompe la simetría: evita el interbloqueo del intento 3 y el livelock del intento 4.
- **Exclusión mutua (reducción al absurdo):** supongamos los dos dentro. Sea P1 el último que escribió `turno` (puso `turno := 2`). Cuando P1 comprobó la condición del bucle, P2 ya tenía `quiere[2]` a verdadero (lo pone antes de escribir `turno`) y nadie volvió a tocar `turno`, así que P1 vio `quiere[2]` y `turno == 2` y no pudo salir del bucle. Contradicción.
- **Progreso y espera limitada:** si P1 espera, es porque `turno == 2` y `quiere[2]`; cuando P2 salga y vuelva a intentarlo, pondrá `turno := 1` y P1 entrará. Nadie es adelantado más de una vez.
- Cumple los cuatro requisitos. El algoritmo de Dekker resuelve lo mismo con más código.
- Programa: `07_peterson_correcto.cpp`.

## 10. Test&Set: ayuda del hardware

La instrucción `Test&Set` lee y pone a verdadero una bandera en **un solo paso indivisible** que da el procesador:

```
testandset(cerrojo): devuelve el valor antiguo y deja cerrojo = verdadero, de forma atómica
entrar:  while testandset(cerrojo): {}
salir:   cerrojo := falso
```

Como comprobar y señalizar ocurren a la vez, desaparece la ventana del intento 2. En C++ es `atomic_flag::test_and_set()`. Garantiza exclusión mutua y progreso, pero **no espera limitada**: un hilo con mala suerte puede perder siempre la carrera (inanición). Y sigue siendo espera activa.

## 11. Por qué en C++ real se usa mutex

Todos los algoritmos anteriores tienen dos problemas prácticos:

1. **Espera activa:** el hilo que espera da vueltas gastando CPU. Con un solo núcleo es aún peor: gasta su rodaja entera sin que el otro pueda avanzar.
2. **La teoría supone memoria secuencialmente consistente.** Con variables `int` normales, el compilador puede sacar la lectura de la bandera fuera del bucle de espera (el programa se cuelga para siempre) o reordenar escrituras, y el procesador puede ejecutar instrucciones fuera de orden. Por eso los programas de esta carpeta usan `atomic` con `memory_order_seq_cst`: es pedir explícitamente la garantía que la teoría daba por hecha. El hardware real no la da gratis.

`mutex` resuelve la exclusión mutua **sin espera activa**: el hilo que no puede entrar se bloquea y el sistema operativo lo despierta cuando el candado queda libre. Peterson explica el problema; `mutex` lo resuelve. Es el contenido de la carpeta `06-mutex`.
