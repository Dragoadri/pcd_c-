# Mapa de trazabilidad con el material heredado

Este documento relaciona cada carpeta y fichero del curso con la transparencia o unidad del material original de la que sale, y lista qué contenido heredado se ha descartado y por qué.

## Fuentes

### Fuentes leídas

| Clave | Documento | Qué es |
|---|---|---|
| **T1** | Tema 1 - Conceptos de programación (PDF, 30 diapositivas) | Transparencias presenciales. Diagramas de "Conceptos de partida", ejemplo lineal y con hilos de la carrera de tortugas en Python, definiciones, ejercicios |
| **T2** | Tema 2 - Mecanismos de sincronización y de comunicación (PDF, 21) | Transparencias presenciales. Problemas (carrera, interbloqueo, lectura y escritura, productor y consumidor, recursos compartidos), ejemplos Python con `threading.Lock`, `Semaphore` y `queue.Queue`, cinco ejercicios |
| **T3** | Tema 3 - Desarrollo de Aplicaciones en N-capas (PDF, 14) | Transparencias presenciales. Tres capas, solución guiada de gestión de tareas con SQLite en Jupyter, ejercicio de agenda de contactos |
| **T4** | Tema 4 - GIL, Threading y Multiprocessing (PDF, 42) | Transparencias presenciales. Programa, proceso e hilo; funciones de espera y de conteo con hilos y procesos en Python; `fork`, cascada, copia de variables; fork frente a spawn; `subprocess` |
| **UA1_T01** | Introducción a la programación concurrente (pptx) | Versión ya reescrita en C++ (curso 2026/27): concurrencia frente a paralelismo, orígenes, arquitecturas, Bernstein, fork/join, seguridad y vivacidad |
| **UA1_T02** | Procesos e hilos (pptx) | Versión ya en C++: proceso frente a hilo, estados, niveles usuario y núcleo, `thread`, `jthread`, errores frecuentes |
| **UA1_T03** | Conceptos básicos (pptx) | Interfoliación, atomicidad, corrección, seguridad, vivacidad, equidad, notación at() |
| **UA1_T04** | Exclusión mutua (pptx) | Sección crítica, requisitos, cuatro intentos, Peterson, `mutex` con `lock_guard` |
| **A0** | Actividad 0: Entorno de trabajo (PDF) | Instalación de CLion, comprobación de la cadena de herramientas, programa de información del sistema |
| **Nuevo** | (sin fuente heredada) | Material añadido para cubrir la secuencia didáctica obligatoria o las trampas técnicas de C++ |

### Fuentes que no estaban disponibles

En el equipo no se encontraron los ficheros originales del campus online `GRINGCIBERM203_UAx_Txx`, ni sus transcripciones, ni las actividades antiguas (**Actividad 1 Hilos Java, Actividad 2 Sockets Java, Actividad 3 Semáforos, Actividad 4 Hilos Python**), ni **UA4_T01**. Los `UA1_T01` a `UA1_T04` que sí había son versiones ya reescritas en C++, no los originales con la parte final en Java, las siete condiciones para N procesos o Test&Set. Donde este mapa habla de ese material, se basa en la descripción del encargo, no en haberlo leído.

## Trazabilidad por fichero

### 00-entorno-clion-cmake

| Fichero | Origen |
|---|---|
| 01_hola_mundo | A0, apartado 1.3 y 1.4 (salida, compilar sin IDE) |
| 02_informacion_del_sistema | A0, Anexo A (versión solo Linux, sin `#if` de sistema operativo) |
| 03_perfil_debug_frente_a_release | Nuevo (prepara la trampa de la optimización de 04) |
| GUION.md | A0, apartados 1.1 a 1.4 y Anexo B (problemas frecuentes) |

### 01-repaso-cpp

| Fichero | Origen |
|---|---|
| 01 a 05 (tipos, aritmética, condicionales, bucles) | Nuevo, repaso básico; coincide en parte con el `repaso.cpp` propio del profesor (`../00_fundamentos/repaso.cpp`) |
| 06 a 14 | Nuevo, siguiendo la lista obligatoria del repaso para concurrencia |
| 15_carrera_tortugas_lineal | T1, "Ejemplo Python lineal" (carrera de tortugas sin hilos) |

### 02-fundamentos-multiprogramacion

| Fichero | Origen |
|---|---|
| 01_automata_frente_a_ordenador | T1, "Conceptos de partida" (diagrama CPU, ALU, UC, memoria, E/S); UA1_T03, atomicidad (`contador++` son tres instrucciones) |
| 02_estados_de_un_proceso | T1, "Conceptos de partida" (diagrama inactivo, preparado, ejecución, suspendido); UA1_T02, diapositivas 10 y 11 |
| 03_planificador_y_cambio_de_contexto | T1, "Conceptos de partida" (Gantt P1 20 ms, P2 10 ms, P3 5 ms) |
| 04_cpu_y_espera_de_entrada_salida | T1, "Conceptos de partida" (programas A y B con uso de CPU y espera) |
| 05_demo_un_nucleo_frente_a_varios | UA1_T01, diapositivas 5 a 8 (concurrencia frente a paralelismo); T4, función de conteo |
| GUION.md | T4 (programa, proceso, hilo); UA1_T01 (orígenes, analogía de la cocina, arquitecturas); UA1_T02 (estados del hilo, niveles usuario, núcleo y combinado, que sirven tal cual) |

### 03-hilos-creacion

Sustituye la parte final en Java de UA1_T02 original (herencia de `Thread` frente a `Runnable`) por creación de hilos en C++.

| Fichero | Origen |
|---|---|
| 01_primer_hilo | UA1_T02, diapositiva 15; UA1_T01, diapositiva 14 (fork/join) |
| 02_dos_hilos_y_tiempo | T4, "función de espera" secuencial y con hilos |
| 03_hilo_con_argumentos | UA1_T02, diapositiva 15 |
| 04_trampa_1_los_argumentos_se_copian | UA1_T02, diapositivas 15 y 17 ("los argumentos se copian") |
| 05_trampa_1_solucion_std_ref | UA1_T02, diapositiva 15 |
| 06_muchos_hilos_con_vector | UA1_T01, diapositivas 13 y 14 (cobegin/coend traducido a vector de hilos) |
| 07_el_orden_no_esta_garantizado | UA1_T01, diapositivas 16 y 19 (no determinismo) |
| 08_carrera_tortugas_con_hilos | T1, "Ejemplo Python" con `threading` |

### 04-hilos-usados-mal

| Fichero | Origen |
|---|---|
| 01_cout_entremezclado | Nuevo (trampa 5); en T1 la salida de las tortugas ya aparece mezclada en la captura |
| 02_contador_compartido_roto | UA1_T03, diapositiva 7; T2, "Condiciones de carrera" |
| 03_trampa_1_sin_std_ref_parece_que_funciona | Nuevo (trampa 1 en contexto de carrera) |
| 04_transferencias_bancarias_rotas | UA1_T03, diapositiva 8 (analogía del cajero) |
| 05_esperar_con_sleep_parece_funcionar / 06_esperar_con_sleep_falla | Nuevo |
| 07_espera_activa_satura_la_cpu | UA1_T04 (espera activa de los intentos) |
| 08_referencia_colgante | UA1_T02, diapositivas 15 y 17 (capturar por referencia lo que se destruye); trampa 4 |
| 09_la_optimizacion_esconde_la_carrera | Nuevo (trampa 2) |

### 05-conceptos-y-exclusion-mutua

| Fichero | Origen |
|---|---|
| 01_interfoliacion_enumerar_mezclas | UA1_T03, diapositiva 6 (barajar dos mazos, multinomial) |
| 02_atomicidad_incremento_en_tres_pasos | UA1_T03, diapositivas 7 y 8 |
| 03_seguridad_invariante_roto | UA1_T03, diapositivas 10 y 11; UA1_T01, diapositiva 17 |
| 04_vivacidad_inanicion_simulada | UA1_T03, diapositivas 12 y 13 (supermercado). Única mención del curso al sinónimo antiguo "equitatividad" |
| 05_intento2_banderas_viola_exclusion | UA1_T04, diapositiva 10 (trampa 3: `atomic` con `seq_cst`) |
| 06_intento3_interbloqueo | UA1_T04, diapositiva 11 |
| 07_peterson_correcto | UA1_T04, diapositiva 15 |
| LECTURA_cuatro_intentos.md | UA1_T04, diapositivas 5 a 15; UA1_T03, diapositiva 14 (at(), invariantes, reducción al absurdo); resumen de las siete condiciones y Test&Set del original |
| GUION.md | UA1_T01, diapositivas 11 y 12 (Bernstein) y 18 (ascensor); T2, catálogo de problemas |

### 06-mutex

| Fichero | Origen |
|---|---|
| 01_contador_sin_mutex / 02_contador_con_mutex | T2, "Mutex (cerraduras)" (100 hilos con `threading.Lock`); UA1_T04, diapositiva 16 (sin `lock_guard`, por la norma) |
| 03_seccion_critica_demasiado_grande | UA1_T04, diapositiva 5 ("la sección crítica debe ser lo más corta posible"); UA1_T01, diapositiva 16 (coste de coordinación) |
| 04_cout_con_mutex | Nuevo (resuelve 04/01) |
| 05_olvidar_unlock | UA1_T04, diapositiva 7 (el cartel del baño que nadie gira) |
| 06_interbloqueo_dos_mutex / 07_interbloqueo_resuelto_mismo_orden | T2, "Bloqueos y Deadlocks" (mutex1 y mutex2 con `acquire(timeout=2)`, portado a `timed_mutex::try_lock_for`) |
| 08_taquilla_cine_sin_mutex / 09_taquilla_cine_con_mutex | T2, ejercicio 4 (taquilla de cine) |

### 07-semaforos

| Fichero | Origen |
|---|---|
| 01_restaurante_sin_limite / 02_restaurante_con_counting_semaphore | T2, ejercicio 2 (restaurante con mesas limitadas) y "Semáforos" |
| 03_binary_semaphore_como_candado | T2, "Semáforos" (semáforo usado como cerradura) |
| 04_condition_variable_basica | T2, índice ("Variables de condición", sin desarrollar en el original) |
| 05_semaforo_artesanal | Nuevo (exigido: un semáforo es un contador protegido); UA1_T04, nota "el cartel pasa a ser un contador" |
| 06_productor_consumidor_sin_sincronizar / 07_productor_consumidor_con_semaforos | T2, "Productor-Consumidor" (sustituye `queue.Queue` y su `while True`) |
| 08_filosofos_con_semaforo_camarero | Nuevo (problema clásico) |

La Actividad 3 Semáforos original no estaba disponible.

### 08-barreras

| Fichero | Origen |
|---|---|
| 01 a 05 | T2, índice ("Barreras", sin desarrollar en el original); el resto es nuevo. La clase invertida y la sesión de cierre están diseñadas desde cero en el GUION |

### 09-repaso-parcial-1

| Fichero | Origen |
|---|---|
| 01_enunciado / 02_solucion_impresoras_compartidas | Nuevo (tipo T2, gestión de recursos compartidos) |
| 03_enunciado / 04_solucion_cuenta_bancaria_cajeros | UA1_T03, analogía del cajero; T2, condiciones de carrera |
| 05_enunciado / 06_solucion_tortugas_con_podio | T1, tortugas; T2, ejercicio 1 (solo un ganador) |
| SIMULACRO_PARCIAL_1.md | Nuevo, con el vocabulario de UA1_T01, UA1_T03 y UA1_T04 |

### 10-fundamentos-de-redes

| Fichero | Origen |
|---|---|
| 01_mis_direcciones_ip, 02_resolver_nombre_dns, 03_ip_y_puerto_en_binario, APUNTES_REDES.md | Nuevo. Contexto de UA1_T01, diapositiva 8 (arquitectura distribuida) y T2 ("Comunicación remota", "Comunicación en sistemas distribuidos") |

### 11-sockets-udp, 12-sockets-tcp, 13-servidores-concurrentes

| Fichero | Origen |
|---|---|
| 11: 01 a 04 (eco UDP, datagramas sin garantías) | Nuevo; sustituye a la Actividad 2 Sockets Java (no disponible) |
| 12: 01 a 06 (eco TCP, flujo por líneas, servidor secuencial) | Nuevo; sustituye a la Actividad 2 Sockets Java |
| 13: 01_servidor_un_hilo_por_conexion, 02_lanzador_de_clientes_concurrentes | UA1_T01, diapositiva 10 ("un servidor secuencial atiende a un cliente y bloquea a los demás") |
| 13: 03 y 04 (visitas sin y con candado), 05_servidor_limite_con_semaforo | T2, "Gestión de recursos compartidos", aplicado a un servidor |
| 13: 06_servidor_chat_difusion / 07_cliente_chat | Nuevo (preludio de la práctica final) |

### 14-n-capas

| Fichero | Origen |
|---|---|
| 01_sqlite_crear_insertar_consultar | T3, "Solución guiada" (crear tabla y conexión) |
| 02_inyeccion_sql_concatenando / 03_consulta_preparada_con_bind | Nuevo (grado de Ciberseguridad); T3, capa de acceso a datos ("seguridad de datos") |
| 04_tareas_todo_mezclado | T3, "Diseño de aplicaciones en N-capas" y "Solución guiada", sin separar capas |
| 05_tareas_en_tres_capas | T3, capas de presentación, lógica de negocio y acceso a datos |
| 06_hilos_escriben_en_sqlite | Nuevo (enlaza N-capas con concurrencia) |
| README.md, `sqlite/` | Nuevo (trampa 7: amalgamación oficial SQLite 3.53.4 dentro del proyecto) |
| GUION.md | T3, ejercicio 1 (agenda de contactos, propuesto como ejercicio) |

### 15-ampliacion-paralelismo-real

| Fichero | Origen |
|---|---|
| 01_suma_paralela_speedup | T4, "función de conteo" con hilos; UA1_T01, diapositiva 10 (ganancia acotada) |
| 02_falso_compartido | Nuevo |
| 03_async_y_future | Nuevo (exigido en la ampliación) |
| 04_fork_primer_proceso | T4, "Cómo usar procesos" (`os.fork`) |
| 05_fork_variables_se_copian | T4, "Copia de variables (y III): valores traspasados" |
| 06_fork_esperar_hijos_waitpid | T4, `fork` y `join` de procesos |
| NOTA_GIL_PYTHON.md | T4, "¿qué es el GIL?", comparativa de tiempos y tabla de la cocina (con la definición del GIL corregida) |
| GUION.md | T4, "Copia de variables (y I): ejecución en cascada" (como pregunta) |

### 16-ampliacion-rpc-distribucion

| Fichero | Origen |
|---|---|
| 01_servidor_rpc_calculadora / 02_cliente_rpc_calculadora | T2, "Mecanismos de comunicación: RPC" (solo citado en el índice del original) |
| 03_maestro_reparte_trabajo / 04_trabajador_remoto | Nuevo (gestión distribuida de procesos) |

### 99-practica-final-whatsapp

| Fichero | Origen |
|---|---|
| ENUNCIADO.md, RUBRICA.md, GUION.md | Nuevo. Formato inspirado en A0 (ficha, detalles de la entrega, lista de comprobación) |

## Material heredado descartado y por qué

### Descartado por no pertenecer a la asignatura

- **Hadoop y virtualización** del material online (según el encargo; no disponible en disco): no son concurrencia ni distribución a este nivel. Virtualización y planificación se imparten en Sistemas Operativos.
- **UA4_T01** (según el encargo): programación funcional en Python con map, filter, lambda, y de paso R y Scala. No trata de concurrencia.
- **T1, "Conceptos de partida"**: la clasificación de sistemas operativos (monoprogramados, por lotes, tiempo real, monousuario y multiusuario) y la cadena `init`, `getty`, `login`, `sh` con fork y exec. Invaden Sistemas Operativos. También los detalles de algoritmos de planificación y memoria virtual.
- **T3**: la capa de presentación como HTML, CSS y JavaScript, y la mención a ORM. El encargo pide N-capas sin separar front y back con sockets entre ellos.

### Descartado porque es específico de Python o de Java

- **T4 como tema completo sobre el GIL**: se reduce a una nota de una página, porque en C++ los hilos sí se ejecutan en paralelo. Van fuera `multiprocessing.Process`, `set_start_method` fork frente a spawn y su tabla, el bloque `if __name__ == '__main__'`, la "bola extra" de `subprocess` con ssh y los enlaces a vídeos (el infame GIL, Python frente a C++, comparativa con C#, historia de Guido van Rossum).
- **T1**, la diapositiva de bibliotecas `threading` y `multiprocessing` de Python; **T2**, `with lock:`, `acquire(timeout=)` y `queue.Queue`; **T3**, Jupyter Notebook y `!pip install sqlite3`. Todo portado a su equivalente C++ o POSIX.
- **UA1_T02 original, parte final en Java** (según el encargo): herencia de `Thread` frente a interfaz `Runnable`. Sustituida por la carpeta 03.
- **Actividades 1 (Hilos Java), 2 (Sockets Java) y 4 (Hilos Python)**: no disponibles. Su contenido lo cubren las carpetas 03 y 04, de 11 a 13, y 15 respectivamente.

### Descartado o cambiado por la norma de la asignatura o por C++20

- **UA1_T01 y UA1_T02, `println`** (C++23): sustituido por `cout`, porque el estándar fijado es C++20.
- **UA1_T02, diapositiva 16, "usa `jthread` por defecto"**, y `stop_token`: la norma exige `join()` explícito. `jthread` solo se menciona en el GUION de 03 como el atajo RAII que verán fuera.
- **UA1_T04, diapositiva 16, `lock_guard`, `scoped_lock` y "nunca llames a lock() y unlock() a mano"**: contradice la norma, que exige candados a mano para ver la sección crítica. Se menciona en el GUION de 06.
- **Todos los `while True` con `break` y los bucles infinitos** del material Python (tortugas, productor y consumidor, menús de T3): reescritos con bandera o condición de parada y comentados en el fichero.
- **A0, detección de Windows y macOS con `#if`**: el curso fija Linux y evita el código condicional.

### Descartado por el recorte a lo imprescindible

- **UA1_T01**: grafos de precedencia, corrutinas de C++20, cobegin/coend como notación (solo aparece como comentario al traducirlo a vector de hilos), ley de Amdahl por su nombre y el aviso de TOCTOU. Pueden añadirse de palabra.
- **UA1_T02**: `thread_local` y el detalle de `joinable()`.
- **UA1_T03**: corrección parcial frente a total, y equidad débil frente a fuerte (en el GUION de 05 solo como mención).
- **UA1_T04**: intentos 1 y 4, Dekker y Test&Set quedan solo en `LECTURA_cuatro_intentos.md`, sin fichero ejecutable. Las siete condiciones para N procesos del original quedan resumidas en una mención.
- **T1**: ejercicios 2 y 3 (añadir tortugas y obstáculos).
- **T2**: ejercicio 3 (productor y consumidor con criterios), ejercicio 5 (administrador de tareas con cola por prioridad), "Lectura/Escritura de datos concurrentes" (solo mención en el GUION de 06), y los mecanismos de comunicación "buzón de correo" y "memoria compartida" como tema propio.
- **T3**: la versión "simulada con listas" de la aplicación de tareas y el ejercicio de la agenda de contactos como fichero (queda propuesto en el GUION de 14).
- **T4**: la ejecución en cascada de `fork` como fichero (queda como pregunta en el GUION de 15).

### Corregido respecto al original

- **T1, definición**: "la programación concurrente ejecuta múltiples tareas en paralelo, es decir, al mismo tiempo" se sustituye por la distinción de UA1_T01 (la concurrencia es propiedad del programa, el paralelismo es propiedad de la ejecución).
- **T4, el GIL**: el original dice que por el GIL "es necesario utilizar mecanismos de gestión de concurrencia" y que el multithreading "no es realmente concurrencia". La nota explica que el GIL protege el estado interno del intérprete y no sustituye a los candados del programador. Además, la cifra de "con hilos tarda el doble" es de intérpretes antiguos: medido con Python 3.12, 2 hilos tardan algo menos que el secuencial y 2 procesos la mitad.
- **Terminología**: el material antiguo llama "equitatividad" a la inanición. El curso usa siempre "inanición" para el fallo y "equidad" para la propiedad, y menciona el sinónimo una sola vez (`05-conceptos-y-exclusion-mutua/04_vivacidad_inanicion_simulada.cpp`).
