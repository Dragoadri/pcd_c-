# Guion de clase: 02 fundamentos de multiprogramación

**Duración estimada:** 105 minutos (1 sesión de 2 horas)
**Entra en examen:** Parcial 1, solo como vocabulario (concurrencia frente a paralelismo, proceso, hilo, estados). Los detalles de planificación son de Sistemas Operativos.

## Objetivo de la sesión

Que entiendan, sin entrar en Sistemas Operativos, **por qué** varias tareas pueden avanzar "a la vez" en un ordenador y **por qué nadie puede predecir** cuándo va a ejecutarse cada una. Es la base del salto de ejecución lineal a ejecución concurrente.

Esta sesión es un resumen y una previa de Sistemas Operativos: nada de algoritmos de planificación en detalle, memoria virtual ni virtualización.

## Antes de empezar (preparación del profesor)

- Compilar la demo `05_demo_un_nucleo_frente_a_varios` y probar en el terminal del aula `taskset -c 0 ./build/bin/05_demo_un_nucleo_frente_a_varios` (en portátiles con Windows no hay `taskset`: hacer la demo desde el equipo del profesor).
- Tener a mano las figuras del Tema 1 presencial ("Conceptos de partida"): CPU con ALU y UC, Gantt de P1, P2, P3, programas A y B con uso de CPU y espera, diagrama de estados, proceso con un hilo frente a proceso con varios hilos.

## Pizarra

Orden de lo que se escribe, de arriba abajo:

1. **Definiciones** (del Tema 4 presencial y UA1_T02):
   - *Programa*: fichero ejecutable con instrucciones; está en disco y no hace nada.
   - *Proceso*: programa cargado en memoria y en ejecución, con su propia zona de memoria y sus recursos. Es la empresa con su oficina.
   - *Hilo*: unidad mínima de ejecución dentro de un proceso. Tiene su pila, sus registros y su contador de programa; comparte con los demás hilos la memoria del proceso. Son los empleados que comparten la oficina. Por eso crear un hilo es barato (proceso ligero) y por eso pueden pisarse.
2. **Von Neumann**: caja CPU (ALU + UC) y caja Memoria (instrucciones y datos juntos), flechas de datos, direcciones y control. Debajo, el ciclo **captar, decodificar, ejecutar**. Frase: *un autómata hace siempre lo mismo; un ordenador cambia de tarea cambiando lo que hay en memoria.*
3. **Orígenes**, en tres líneas: años 60 multiprogramación (la CPU era carísima y esperaba a la E/S); años 70 tiempo compartido (muchos usuarios, rodajas de tiempo); desde 2005 el muro de la frecuencia (no se pueden subir más los GHz por consumo y calor, se añaden núcleos, y un programa secuencial ya no va más rápido solo por cambiar de ordenador).
4. **Diagrama de estados**: NUEVO, PREPARADO, EJECUCIÓN, BLOQUEADO, TERMINADO con las flechas crear, planificación, expropiado, espera E/S, ocurre suceso, terminar. Rodear con rojo la flecha BLOQUEADO → PREPARADO: *desbloquear no es ejecutar*.
5. **Los mismos estados para un hilo** (UA1_T02): creación, bloqueo, desbloqueo y terminación. Analogía de la sala de espera del centro de salud: coges número (creación), te llaman (ejecución), te mandan a radiografía (bloqueo), vuelves y **esperas otra vez** (desbloqueo, vuelta a la cola).
6. **Niveles de hilos** (UA1_T02), tabla de tres filas:
   - Nivel de usuario: los gestiona una biblioteca, el núcleo ve un solo proceso. Cambio muy rápido, pero una llamada bloqueante para a todos y no aprovechan varios núcleos.
   - Nivel de núcleo: el sistema operativo conoce y planifica cada hilo; aprovechan varios núcleos; el cambio de contexto es más caro.
   - Combinado (M:N): varios hilos de usuario sobre unos pocos de núcleo; flexible y difícil.
   - Nota: en Linux cada `thread` es un hilo de núcleo (modelo 1:1).
7. **Gantt** de P1 (20 ms), P2 (10 ms), P3 (5 ms) en FCFS y en turno rotatorio, y la palabra *cambio de contexto* con su coste.
8. **Concurrencia frente a paralelismo** (UA1_T01), con la analogía de la cocina:
   - Concurrencia: un cocinero con tres platos que alterna entre ellos. Varias tareas activas en el mismo intervalo; existe con un solo núcleo.
   - Paralelismo: tres cocineros, cada uno con su plato y su fuego, en el mismo instante. Necesita varios núcleos.
   - Dos cocineros que van a la vez a por la misma sartén: recurso compartido, el origen de todo el temario.
   - Frase final: *todo programa paralelo es concurrente; no todo programa concurrente se ejecuta en paralelo.*

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | (pizarra 1 a 3) | 15 | Programa, proceso e hilo. Von Neumann y orígenes. |
| 1 | `01_automata_frente_a_ordenador.cpp` | 20 | La memoria es un vector con instrucciones y datos. La función `cpu` repite captar, decodificar, ejecutar. Dos programas, la misma CPU. Detenerse en el segundo programa: `contador++` son tres instrucciones (cargar, sumar, guardar). Dejarlo subrayado: volverá en el tema 04. |
| 2 | `02_estados_de_un_proceso.cpp` | 15 | Pizarra 4 a 6 mientras se teclea. La cadena de `if / else if` hace de diagrama de estados (sin `switch`). El sexto suceso se rechaza a propósito. |
| 3 | `03_planificador_y_cambio_de_contexto.cpp` | 20 | Pizarra 7. Primero FCFS, luego turno rotatorio. El vector `restante` es el "contexto" que se guarda y se restaura. Comparar espera media y número de cambios de contexto. El proceso no decide cuándo le quitan la CPU. |
| 4 | `04_cpu_y_espera_de_entrada_salida.cpp` | 20 | Figura de los programas A y B. Solo hay una CPU y aun así se tarda 90 ms en vez de 160: la ganancia viene de solapar las esperas. Eso es concurrencia sin paralelismo. |
| 5 | `05_demo_un_nucleo_frente_a_varios.cpp` | 15 | **No se teclea**, se ejecuta. Pizarra 8. Primero normal (las dos tareas a la vez tardan la mitad), después con `taskset -c 0` (tardan lo mismo que en secuencia). Anunciar que `thread` se explica en la carpeta siguiente. |

## Preguntas a la clase antes de ejecutar

- **01:** "Si cambio los datos de las direcciones 6 y 7, ¿hay que recompilar la CPU?" Respuesta: no; el programa y los datos están en memoria, la CPU es la misma.
- **01:** "¿Cuántas instrucciones de máquina hacen falta para `contador++`?" Respuesta: tres.
- **02:** "Un proceso bloqueado esperando al disco recibe el dato. ¿Pasa a ejecutarse?" Respuesta: no, pasa a preparado y espera turno.
- **03:** "¿Con qué planificador espera menos, de media, P3?" Respuesta: con turno rotatorio (10 ms frente a 30 ms en FCFS).
- **04:** "Con una sola CPU, ¿cómo puede tardar menos hacer dos programas intercalados?" Respuesta: porque mientras uno espera a la E/S el otro usa la CPU; no se ejecutan a la vez.
- **05:** "Si le quito todos los núcleos menos uno, ¿las dos tareas a la vez tardarán la mitad?" Respuesta: no; con un núcleo se alternan y tardan lo mismo que en secuencia.

## Errores típicos de los alumnos

- Creer que concurrencia y paralelismo son lo mismo, o que con un solo núcleo no puede haber problemas de concurrencia. Con un núcleo también hay interfoliación y también hay errores (se verá en el tema 04; allí se explica por qué en un núcleo el fallo es más raro de observar pero existe).
- Pensar que un proceso desbloqueado sigue ejecutándose inmediatamente.
- Confundir proceso e hilo: los procesos no comparten memoria entre sí; los hilos de un mismo proceso sí.
- Creer que más hilos siempre es más rápido. Con trabajo de CPU, el límite es el número de núcleos.
- En la simulación de estados, querer escribir un `switch`: recordar la norma, cadena `if / else if / else`.

## Cierre y enlace con la siguiente carpeta

Idea para llevarse: **el sistema operativo decide quién usa la CPU y cuándo, y el programa no se entera**. En la carpeta `03-hilos-creacion` creamos hilos de verdad con `thread`, empezando por la carrera de tortugas que en `01-repaso-cpp` era lineal.
