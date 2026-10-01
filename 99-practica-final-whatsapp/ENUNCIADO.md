# Práctica final: mensajería instantánea concurrente en C++ (un "WhatsApp" de consola)

**Programación Concurrente y Distribuida · Universidad Europea de Madrid**

| Dato | Valor |
|---|---|
| Asignatura | Programación Concurrente y Distribuida (2º curso) |
| Unidades implicadas | Hilos, candados (mutex), semáforos, sockets TCP, servidores concurrentes, N-capas con SQLite |
| Modalidad | En grupo (3 o 4 personas; grupos de 2 solo con autorización del profesor) |
| Lenguaje y entorno | C++20, CMake, CLion, Linux (GCC o Clang) |
| Entrega | Campus virtual, antes de las vacaciones de Navidad: **[FECHA A FIJAR]** a las 23:59 |
| Defensa oral | Individual, en las sesiones posteriores a la entrega: **[FECHAS A FIJAR]** |
| Evaluación | Tres tramos acumulativos: hasta 5, hasta 7 y hasta 10 puntos (ver sección 7 y RUBRICA.md) |
| Continuidad | El proyecto se reutiliza como punto de partida en **Proyectos de Informática II** |

**LEED ESTE DOCUMENTO HASTA EL FINAL ANTES DE ESCRIBIR UNA SOLA LÍNEA DE CÓDIGO.** Casi todas las dudas que os van a surgir en diciembre están resueltas aquí.

---

## 1. Presentación

Durante el curso habéis visto por separado todas las piezas de un sistema concurrente y distribuido: hilos y `join()`, condiciones de carrera, candados (mutex), semáforos, sockets TCP, servidores con un hilo por conexión y aplicaciones en capas con SQLite.

La práctica final consiste en juntar todas esas piezas en un único programa que funcione de verdad: un sistema de mensajería instantánea al estilo de WhatsApp, pero en consola. Habrá un **servidor** al que se conectan muchos **clientes** a la vez. Los usuarios se registran, inician sesión, se mandan mensajes entre ellos, crean grupos y consultan su historial. Todo lo que ocurre queda guardado en una base de datos, de forma que si el servidor se apaga y se vuelve a encender, los usuarios y sus conversaciones siguen ahí.

Lo difícil de esta práctica no es la cantidad de código, sino que **no podéis predecir en qué orden van a ocurrir las cosas**. Mientras Ana escribe un mensaje a Luis, Luis puede estar desconectándose, Marta puede estar creando un grupo en el que están los dos y el servidor puede estar guardando en la base de datos un mensaje anterior. Vuestro programa tiene que ser correcto en **todas** esas situaciones, no solo en la que os salió al probarlo una vez. Ese es exactamente el salto de la programación lineal a la concurrente que lleváis todo el cuatrimestre trabajando.

## 2. Objetivos de aprendizaje

Al terminar la práctica, cada miembro del grupo debe ser capaz de:

1. Diseñar un servidor TCP concurrente en C++ con un hilo por conexión, y justificar cuándo se crea y cuándo se une (`join`) cada hilo.
2. Identificar **todos** los recursos compartidos de un programa real (estructuras en memoria, sockets, base de datos, fichero de registro, consola) y protegerlos con el mecanismo adecuado.
3. Utilizar un semáforo contador (`counting_semaphore`) para limitar el número de conexiones simultáneas, y explicar qué ocurre cuando se alcanza el límite.
4. Definir e implementar un protocolo de aplicación de texto sobre TCP, teniendo en cuenta que TCP es un flujo de bytes y no un sistema de mensajes.
5. Organizar el servidor en tres capas (presentación, lógica de negocio y acceso a datos) y acceder a SQLite con consultas preparadas.
6. Detectar y explicar condiciones de carrera, interbloqueos e inanición en su propio código.
7. Medir el comportamiento del sistema bajo carga con muchos clientes concurrentes.
8. Trabajar con herramientas de IA de forma responsable: entendiendo, adaptando y defendiendo el código que la IA propone.

## 3. Qué hay que construir

El sistema tiene, como mínimo, estos programas:

| Programa | Descripción |
|---|---|
| `servidor` | Escucha en un puerto TCP, acepta conexiones y atiende a cada cliente en su propio hilo. Contiene la lógica del sistema y la base de datos. |
| `cliente` | Programa de consola que se conecta al servidor, permite al usuario escribir órdenes y muestra en tiempo real los mensajes que le llegan. |
| `lanzador_clientes` | Programa de pruebas que simula muchos clientes a la vez (obligatorio en el tramo 3, recomendable desde el principio). |

Se ejecutan en varias terminales de la misma máquina (servidor en una, un cliente por terminal) o en varias máquinas de la misma red. El comportamiento debe ser el mismo en ambos casos.

## 4. Arquitectura obligatoria

### 4.1. El servidor

- Es un servidor **TCP** con sockets POSIX (`sys/socket.h`, `netinet/in.h`, `arpa/inet.h`), igual que los de las carpetas 12 y 13 del curso.
- El hilo principal crea el socket de escucha y ejecuta el bucle de aceptación. Por cada conexión aceptada se lanza **un hilo** que atiende a ese cliente hasta que se desconecta.
- Los hilos de atención se guardan (por ejemplo, en un `vector<thread>`) para poder unirlos con `join()` en el cierre. **No se permite `detach()`.**
- El puerto se define como constante en el código o se lee de un fichero de configuración; el programa debe arrancar sin argumentos.
- El servidor debe poder apagarse de forma ordenada (sección 6.4). Un servidor que solo se puede parar con Ctrl+C no cumple el tramo 1.

### 4.2. El cliente

- Es un programa de consola que se conecta al servidor por TCP.
- Tiene, como mínimo, **dos hilos**: el hilo principal lee lo que escribe el usuario y lo envía; un **hilo receptor** lee continuamente del socket y muestra por pantalla todo lo que llega (respuestas y notificaciones de mensajes nuevos). Sin este segundo hilo, un usuario solo vería los mensajes que le mandan cuando pulse Enter, y eso no es mensajería instantánea.
- La consola es un recurso compartido entre los dos hilos: la escritura en pantalla debe estar protegida para que las líneas no se mezclen.
- Basta con que el usuario escriba las órdenes del protocolo; una interfaz amigable (por ejemplo `/msg ana hola`) se valora como mejora en el tramo 3.

### 4.3. Tres capas en el servidor

El servidor se organiza en tres capas, cada una en sus propios ficheros `.cpp` y `.hpp`, que se comunican **mediante llamadas a funciones**:

| Capa | Responsabilidad | Ejemplos de funciones |
|---|---|---|
| Presentación | Traduce el protocolo: lee líneas del socket, las separa en campos, llama a la lógica de negocio y construye la respuesta de texto. Es la única capa que conoce el formato del protocolo. | `procesar_linea`, `enviar_linea`, `separar_campos` |
| Lógica de negocio | Aplica las reglas del sistema: quién puede enviar a quién, qué pasa si el destinatario no está conectado, quién pertenece a un grupo. No sabe nada de sockets ni de SQL. | `registrar_usuario`, `iniciar_sesion`, `enviar_mensaje`, `crear_grupo` |
| Acceso a datos | Es la única capa que habla con SQLite: abre la base de datos, crea las tablas y ejecuta consultas preparadas. | `datos_insertar_usuario`, `datos_guardar_mensaje`, `datos_leer_historial` |

Aclaración importante: **las capas no se separan con sockets entre ellas.** La única comunicación por red es la de cliente y servidor. Dentro del servidor, la capa de presentación llama a funciones de negocio y la de negocio llama a funciones de datos, todo en el mismo proceso. Tampoco se permite que la capa de presentación ejecute SQL directamente, ni que la de datos escriba en un socket.

### 4.4. Persistencia con SQLite

- La base de datos es un fichero SQLite (por ejemplo, `whatsapp.db`) que crea el propio servidor si no existe.
- SQLite se incluye **dentro del proyecto** con la amalgamación (`sqlite3.c` y `sqlite3.h`), igual que en la carpeta 14 del curso, y se enlaza desde CMake. Quien descargue el proyecto no debe tener que instalar nada.
- Todas las consultas que incluyan datos introducidos por el usuario se hacen con **consultas preparadas** (`sqlite3_prepare_v2` y `sqlite3_bind_*`). Construir SQL concatenando cadenas está prohibido desde el tramo 2.
- El acceso a la base de datos desde varios hilos debe estar pensado y documentado (sección 6.2).

### 4.5. Estructura de carpetas de la entrega

```
grupo_XX_whatsapp/
  CMakeLists.txt
  README.md                  manual de compilación y ejecución
  sqlite/                    sqlite3.c y sqlite3.h
  servidor/
    servidor_main.cpp
    capa_presentacion.cpp / .hpp
    capa_negocio.cpp / .hpp
    capa_datos.cpp / .hpp
  cliente/
    cliente_main.cpp
  pruebas/
    lanzador_clientes.cpp
    resultados/              medidas de las pruebas de carga
  docs/
    memoria.pdf
    anexo_ia.pdf
    capturas/
```

Podéis añadir ficheros (por ejemplo, `servidor/registro_actividad.cpp`), pero se deben respetar las carpetas y la separación en capas. La carpeta `99-practica-final-whatsapp` del curso incluye un `CMakeLists.txt` de referencia.

## 5. Protocolo de aplicación

### 5.1. Reglas generales

TCP no transmite mensajes, transmite un **flujo de bytes** (lo visteis en la carpeta 12): si el cliente envía dos órdenes seguidas, el servidor puede recibirlas juntas en un solo `recv`, o recibir media orden en un `recv` y la otra media en el siguiente. Por eso el protocolo se basa en **líneas**:

1. Cada mensaje del protocolo es una línea de texto terminada en `\n`.
2. Los campos se separan con punto y coma (`;`).
3. El **último campo** de una orden se lleva el resto de la línea, incluidos posibles `;`. Así el texto de un mensaje puede contener punto y coma sin romper nada.
4. Longitud máxima de una línea: 1024 bytes. Una línea más larga se responde con `ERROR;LINEA_LARGA` y se descarta.
5. Nombres de usuario y de grupo: entre 3 y 16 caracteres, solo letras sin tildes, dígitos y guion bajo.
6. Contraseñas: entre 4 y 64 caracteres imprimibles, sin `;`.
7. Las respuestas a una orden empiezan siempre por `OK` o `ERROR`. Todo lo que empiece por otra palabra es una **notificación asíncrona** (algo que el servidor envía por iniciativa propia, por ejemplo un mensaje nuevo).
8. Antes de iniciar sesión solo se aceptan `REGISTRO`, `LOGIN` y `SALIR`. Cualquier otra orden se responde con `ERROR;NO_AUTENTICADO`.

### 5.2. Órdenes del cliente al servidor

| Orden | Formato | Respuesta correcta | Errores posibles | Tramo |
|---|---|---|---|---|
| Registro | `REGISTRO;usuario;clave` | `OK;REGISTRO;usuario` | `USUARIO_EXISTE`, `FORMATO` | 1 |
| Inicio de sesión | `LOGIN;usuario;clave` | `OK;LOGIN;usuario` | `CREDENCIALES`, `YA_CONECTADO`, `BLOQUEADO` (tramo 3) | 1 |
| Lista de conectados | `LISTA` | `OK;LISTA;ana,luis,marta` | `NO_AUTENTICADO` | 1 |
| Mensaje privado | `MSG;destino;texto` | `OK;MSG;id` | `NO_EXISTE`, `NO_CONECTADO` (solo tramo 1), `FORMATO` | 1 |
| Crear grupo | `GRUPO_CREAR;nombre` | `OK;GRUPO_CREAR;nombre` | `GRUPO_EXISTE`, `FORMATO` | 2 |
| Unirse a grupo | `GRUPO_UNIR;nombre` | `OK;GRUPO_UNIR;nombre` | `NO_EXISTE`, `YA_MIEMBRO` | 2 |
| Mensaje a grupo | `MSG_GRUPO;grupo;texto` | `OK;MSG_GRUPO;id` | `NO_EXISTE`, `NO_MIEMBRO` | 2 |
| Historial | `HISTORIAL;usuario_o_grupo;n` | `OK;HISTORIAL;k` seguido de `k` líneas `HIST;...` y una línea `FIN;HISTORIAL` | `NO_EXISTE`, `FORMATO` | 2 |
| Estado de un usuario | `ESTADO;usuario` | `OK;ESTADO;usuario;EN_LINEA` o `OK;ESTADO;usuario;DESCONECTADO;fecha` | `NO_EXISTE` | 3 |
| Confirmar lectura | `LEIDO;id` | `OK;LEIDO;id` | `NO_EXISTE` | 3 |
| Salir | `SALIR` | `OK;SALIR` y el servidor cierra la conexión | | 1 |

Las respuestas de error tienen siempre la forma `ERROR;CODIGO`, opcionalmente seguida de un texto explicativo: `ERROR;NO_EXISTE;el usuario pepe no existe`.

### 5.3. Notificaciones del servidor al cliente

| Notificación | Formato | Cuándo se envía | Tramo |
|---|---|---|---|
| Mensaje privado | `DE;origen;id;fecha;texto` | Al destinatario, en cuanto el servidor procesa el `MSG` (o al conectarse, si estaba desconectado) | 1 (fecha desde el tramo 2) |
| Mensaje de grupo | `DE_GRUPO;grupo;origen;id;fecha;texto` | A todos los miembros conectados del grupo, salvo al autor | 2 |
| Línea de historial | `HIST;id;fecha;origen;texto` | Dentro de la respuesta a `HISTORIAL` | 2 |
| Entregado | `ENTREGADO;id` | Al autor, cuando el mensaje llega al cliente del destinatario | 3 |
| Leído | `LEIDO;id;usuario` | Al autor, cuando el destinatario confirma la lectura | 3 |
| Aviso | `AVISO;texto` | Mensajes informativos del servidor (por ejemplo, "el servidor se va a apagar") | 1 |

La fecha se envía en formato `AAAA-MM-DD HH:MM:SS`.

### 5.4. Ejemplo de diálogo

Las líneas que empiezan por `C>` las envía el cliente de Ana; las que empiezan por `S>` las envía el servidor a Ana. En paralelo, Luis está conectado desde otra terminal.

```
C> REGISTRO;ana;gatito22
S> OK;REGISTRO;ana
C> LOGIN;ana;gatito22
S> OK;LOGIN;ana
S> DE;luis;41;2026-12-02 17:58:10;te dejé un mensaje mientras no estabas
C> LISTA
S> OK;LISTA;ana,luis
C> MSG;luis;hola; ¿nos vemos a las 6?
S> OK;MSG;42
S> ENTREGADO;42
S> DE;luis;43;2026-12-02 18:01:12;vale, en la biblioteca
C> LEIDO;43
S> OK;LEIDO;43
S> LEIDO;42;luis
C> MSG;pepe;hola
S> ERROR;NO_EXISTE;el usuario pepe no existe
C> SALIR
S> OK;SALIR
```

Fijaos en tres detalles. El texto `hola; ¿nos vemos a las 6?` contiene un punto y coma y llega entero porque es el último campo. La notificación `DE;luis;43;...` llega **entre** la respuesta a una orden y la siguiente orden de Ana: el cliente no puede suponer que después de enviar una orden lo siguiente que recibe es la respuesta a esa orden. Y el mensaje 41 se envió cuando Ana no estaba conectada y se le entrega nada más iniciar sesión (tramo 2).

### 5.5. Extender el protocolo

El protocolo es una **propuesta**. Podéis añadir órdenes y notificaciones (por ejemplo `GRUPO_SALIR`, `GRUPO_MIEMBROS`, `ESCRIBIENDO`) o cambiar detalles justificados, siempre que:

- se mantengan las reglas generales de la sección 5.1;
- el protocolo completo esté documentado en la memoria con el mismo formato de tabla;
- el profesor pueda probar el servidor con un cliente sencillo que escriba las órdenes a mano (por ejemplo, `nc localhost 5555`).

## 6. Requisitos de concurrencia

Esta sección es el corazón de la práctica. Se evalúa en la memoria, en las pruebas y, sobre todo, en la defensa oral.

### 6.1. Hilos del servidor

| Hilo | Cuántos | Qué hace | Cuándo termina |
|---|---|---|---|
| Principal | 1 | Inicializa la base de datos, crea el socket de escucha, acepta conexiones y lanza hilos de atención | Cuando se activa la bandera de apagado; después une a todos los hilos |
| Atención | Uno por conexión | Lee líneas del socket de su cliente, llama a la capa de presentación y envía respuestas | Cuando el cliente envía `SALIR`, se cierra la conexión o se activa el apagado |
| Consola del servidor (recomendado) | 1 | Lee órdenes del administrador por teclado (`apagar`, `conectados`) | Al leer `apagar` o al llegar al final de la entrada |

### 6.2. Recursos compartidos y su protección

Cada grupo debe incluir en la memoria una **tabla de recursos compartidos** completa (plantilla en el anexo A). Como mínimo aparecerán estos recursos:

| Recurso compartido | Quién lo usa | Riesgo si no se protege | Protección mínima exigida |
|---|---|---|---|
| Tabla de usuarios conectados (nombre de usuario y socket de cada conexión) | Todos los hilos de atención | Dos hilos inician sesión con el mismo usuario a la vez; se envía un mensaje a un socket ya cerrado | `mutex` propio, secciones críticas cortas |
| Escritura en el socket de un cliente | Su hilo de atención y los hilos de otros usuarios que le envían mensajes | Dos líneas se mezclan byte a byte y el cliente recibe basura | Un mutex por conexión, o un mutex global de envío justificado |
| Contador o generador de identificadores de mensaje | Todos los hilos que envían mensajes | Dos mensajes con el mismo id | Mutex o `atomic` (en el tramo 2, el id lo genera SQLite) |
| Base de datos SQLite | Todos los hilos de atención | Errores `SQLITE_BUSY`, lecturas incoherentes | Una conexión compartida protegida con un mutex en la capa de datos, o una conexión por hilo con `sqlite3_busy_timeout`; hay que justificar la elección |
| Fichero de registro de actividad | Todos los hilos | Líneas mezcladas o cortadas | Mutex del registro (tramo 3) |
| Bandera de apagado | Hilo principal, hilo de consola, hilos de atención | Un hilo no se entera de que debe terminar | `atomic<bool>` |
| Consola del cliente | Hilo principal y receptor del cliente | Líneas mezcladas en pantalla | Mutex de consola |

Recordad que la norma de la asignatura no permite definir `struct` ni clases, así que la información de cada conexión se guarda en **vectores paralelos** (por ejemplo, `vector<int> sockets_conexion` y `vector<string> usuario_conexion`, con la misma posición para la misma conexión) o en `map` de la librería estándar. Un `mutex` no se puede copiar ni mover, así que un vector de mutex debe crearse con su tamaño definitivo desde el principio (por ejemplo, `vector<mutex> mtx_envio(MAX_CONEXIONES);`) y no volver a cambiar de tamaño.

Reglas de oro que se comprobarán en la defensa:

1. **Nunca se hace una operación lenta con un candado tomado** si se puede evitar: ni `recv`, ni esperas, ni consultas largas. Mantener un candado mientras se espera a que un cliente escriba bloquea a todo el servidor.
2. **Orden fijo de adquisición.** Si alguna función necesita dos candados a la vez, hay que documentar el orden y respetarlo en todo el código para evitar interbloqueos (deadlock).
3. **Todo `lock()` tiene su `unlock()` en todos los caminos**, incluidos los de error. Un `return` en mitad de una sección crítica es el fallo más típico.
4. **Comprobar y actuar dentro de la misma sección crítica.** "Miro si luis está conectado" y "le envío el mensaje" no pueden estar en dos secciones críticas separadas, porque entre medias luis puede desconectarse.

### 6.3. Límite de conexiones simultáneas

- El servidor admite como máximo `MAX_CONEXIONES` clientes a la vez (por ejemplo, 20; debe ser una constante fácil de cambiar).
- El límite se implementa con un `counting_semaphore`: se adquiere un permiso antes de lanzar el hilo de atención y se libera **siempre** cuando esa conexión termina, también si termina por error.
- Cuando no hay permisos disponibles, el servidor debe hacer algo **definido y documentado**: esperar un tiempo máximo con `try_acquire_for` y, si no hay hueco, responder `ERROR;SERVIDOR_LLENO` y cerrar esa conexión. Lo que no se admite es que el cliente se quede esperando para siempre sin saber qué pasa.
- En el tramo 1 el límite puede implementarse con un contador protegido por mutex; el semáforo es obligatorio desde el tramo 2.

### 6.4. Cierre ordenado

El servidor debe poder apagarse sin perder datos ni dejar hilos colgados:

1. Algo activa la bandera de apagado (orden `apagar` en la consola del servidor, o un número máximo de conexiones atendidas en modo pruebas).
2. El bucle de aceptación deja de aceptar. Para que `accept` no se quede bloqueado para siempre, el socket de escucha debe tener un tiempo de espera (`SO_RCVTIMEO`), de forma que el bucle compruebe periódicamente la bandera, exactamente como en los servidores de la carpeta 13.
3. Se avisa a los clientes conectados (`AVISO;el servidor se va a apagar`) y se cierran sus sockets con `shutdown`, lo que despierta a los hilos que estaban esperando en `recv`.
4. El hilo principal hace `join()` de **todos** los hilos que ha creado.
5. Se cierra la base de datos y el fichero de registro.

Un servidor que termina con "terminate called without an active exception" (un `thread` destruido sin `join`) o que necesita Ctrl+C para acabar **no cumple** el requisito de cierre ordenado.

### 6.5. Hilos del cliente

- El hilo receptor termina cuando `recv` devuelve 0 (el servidor cerró) o un error, y activa una bandera compartida para que el hilo principal sepa que la conexión ha terminado.
- El hilo principal termina cuando el usuario escribe la orden de salida, cuando se llega al final de la entrada estándar o cuando detecta que la conexión ha terminado. Como `getline` es bloqueante, se admite que el cliente pida pulsar Enter para salir tras una desconexión del servidor.
- Antes de salir, el hilo principal une al receptor con `join()`.

### 6.6. Prohibiciones y trampas conocidas

- **Prohibido coordinar hilos con `sleep_for`.** "Espero 100 milisegundos y así seguro que el otro hilo ya ha terminado" es un error, aunque funcione en vuestra máquina.
- **Prohibida la espera activa** (bucles que comprueban una condición sin parar consumiendo CPU). Para esperar a que ocurra algo se usan `join`, semáforos o variables de condición.
- **Ningún bucle sin condición de parada.** `while (true)` no está permitido: todo bucle termina por su condición (banderas booleanas).
- **Escritura en sockets cerrados.** Si un cliente se desconecta de golpe y el servidor le escribe, el sistema envía la señal `SIGPIPE`, que por defecto mata el proceso entero. Usad `send(..., MSG_NOSIGNAL)` y comprobad el valor devuelto.
- **Envíos parciales.** `send` puede enviar menos bytes de los pedidos: hay que repetir hasta enviar la línea completa.
- **Argumentos de los hilos.** `thread` copia sus argumentos: para compartir hace falta `ref`, y lo compartido debe vivir más que el hilo (carpetas 03 y 04).

## 7. Funcionalidades por tramos

La nota funcional se organiza en **tres tramos acumulativos**. Un tramo solo puntúa si el anterior está **completo**, es decir, si cumple **todos sus requisitos obligatorios** (marcados con **O**). Los requisitos no obligatorios suman puntos dentro de su tramo. El detalle de puntos está en RUBRICA.md.

### 7.1. Tramo 1: servidor concurrente funcional (hasta 5 puntos)

| Código | Requisito | Tipo |
|---|---|---|
| T1.1 | El proyecto compila con CMake desde cero en Linux, sin avisos graves, con `-Wall -Wextra` | O |
| T1.2 | El servidor atiende al menos 5 clientes conectados a la vez, cada uno en su propio hilo | O |
| T1.3 | Registro e inicio de sesión de usuarios, guardados en memoria (se pierden al apagar) | O |
| T1.4 | No se permite iniciar sesión dos veces a la vez con el mismo usuario (comprobado dentro de una sección crítica) | O |
| T1.5 | `LISTA` devuelve los usuarios conectados | O |
| T1.6 | Mensajes privados a usuarios conectados, que aparecen en el destinatario sin que tenga que pulsar nada (hilo receptor en el cliente) | O |
| T1.7 | Todas las estructuras compartidas del servidor están protegidas con candados tomados y soltados a mano | O |
| T1.8 | Protocolo por líneas que funciona aunque lleguen varias órdenes juntas o una orden partida | O |
| T1.9 | Cierre ordenado del servidor con `join` de todos los hilos (sección 6.4) | O |
| T1.10 | Desconexión brusca de un cliente (cerrar su terminal) sin que el servidor se caiga ni quede el usuario como conectado | O |
| T1.11 | Respuestas de error correctas para órdenes mal formadas y usuarios inexistentes | Puntuable |
| T1.12 | Memoria con manual de ejecución, protocolo y primera tabla de recursos compartidos | Puntuable |

### 7.2. Tramo 2: persistencia y grupos (hasta 7 puntos)

| Código | Requisito | Tipo |
|---|---|---|
| T2.1 | Base de datos SQLite con la amalgamación dentro del proyecto; usuarios y mensajes persisten tras reiniciar el servidor | O |
| T2.2 | Servidor organizado en tres capas en ficheros separados, sin SQL fuera de la capa de datos ni sockets fuera de la de presentación | O |
| T2.3 | Todas las consultas con datos del usuario son consultas preparadas | O |
| T2.4 | Mensajes a usuarios desconectados: se guardan y se entregan en orden al iniciar sesión | O |
| T2.5 | Límite de conexiones simultáneas con `counting_semaphore` y comportamiento definido al llegar al límite | O |
| T2.6 | Acceso concurrente a la base de datos protegido y justificado en la memoria | O |
| T2.7 | Grupos: crear, unirse y enviar mensajes a todos los miembros conectados (y diferidos para los desconectados) | Puntuable |
| T2.8 | `HISTORIAL` de una conversación privada o de grupo, con fecha de cada mensaje | Puntuable |
| T2.9 | Diagrama de la base de datos y tabla de recursos compartidos actualizada en la memoria | Puntuable |

### 7.3. Tramo 3: sistema robusto, medido y seguro (hasta 10 puntos)

| Código | Requisito | Tipo |
|---|---|---|
| T3.1 | Confirmaciones de entrega (`ENTREGADO`) y de lectura (`LEIDO`), también para mensajes diferidos | O |
| T3.2 | Estados de usuario: en línea o fecha y hora de última conexión (`ESTADO`) | O |
| T3.3 | Programa `lanzador_clientes` que lanza N clientes concurrentes (hilos) que inician sesión y envían M mensajes cada uno | O |
| T3.4 | Pruebas de carga documentadas: al menos tres tamaños (por ejemplo 10, 50 y 100 clientes), tiempo total, mensajes por segundo y comprobación de que **no se pierde ningún mensaje** (el número de mensajes en la base de datos coincide con el esperado) | O |
| T3.5 | Registro de actividad en fichero (conexiones, inicios de sesión, errores), con fecha e identificador de hilo, protegido frente a escrituras concurrentes | Puntuable |
| T3.6 | Contraseñas no almacenadas en claro: resumen (hash) con sal aleatoria por usuario. Sin librerías externas: implementación propia de un algoritmo reconocido (por ejemplo SHA-256 siguiendo su especificación) o, si se usa algo más débil, justificación honesta de sus limitaciones (se puntúa menos). `hash` no es criptográfico y no cuenta como solución | Puntuable |
| T3.7 | Validación de todas las entradas (longitudes, caracteres, número de campos) en la capa de presentación y en la de negocio | Puntuable |
| T3.8 | Límite de intentos de inicio de sesión fallidos por usuario (bloqueo temporal) con su estado compartido protegido | Puntuable |
| T3.9 | Mejora de libre elección documentada (cliente con menú amigable, colores ANSI, órdenes de administración, mensajes de "escribiendo...", eliminación de mensajes, lectura con variables de condición, etc.) | Puntuable |

## 8. Norma de código obligatoria

La práctica sigue la **misma norma que todo el código de la asignatura**. No hay excepciones por tratarse de un proyecto grande:

1. **Prohibidos `break`, `continue` y `goto`.** Los bucles se controlan con su condición y con banderas booleanas.
2. **Prohibido `switch`**, porque en C++ exige `break`. Se usa `if / else if / else`.
3. **Prohibido definir clases, `struct`, `union` o `enum class` propios.** El código es procedimental, con funciones libres. Sí se pueden usar los tipos de la librería estándar (`string`, `vector`, `map`, `thread`, `mutex`, `counting_semaphore`, `condition_variable`, `atomic`) y los tipos de POSIX y SQLite (`sockaddr_in`, `sqlite3*`), escritos sin la palabra `struct`.
4. **Candados tomados y soltados a mano** con `mtx.lock()` y `mtx.unlock()`. Prohibidos `lock_guard` y `scoped_lock`. La única excepción es `unique_lock` cuando lo exige `condition_variable::wait`, con un comentario que lo explique.
5. **Todo hilo creado se une con `join()`.** Prohibidos `detach()` y `jthread`.
6. **C++20, CMake y Linux** (GCC o Clang). Solo librería estándar, POSIX y SQLite. Nada de Boost, Asio ni otras dependencias.
7. **Todo en español**: nombres de variables y funciones, comentarios y mensajes.
8. **Ningún programa se cuelga**: todos los bucles tienen condición de parada y los bloqueos tienen salida (tiempos de espera, apagado ordenado).
9. **`using namespace std;`** debajo de los `#include` de cada fichero, como en todos los ejemplos de clase, sin escribir `std::` en el código. Cuidado con la llamada POSIX `bind` del servidor: se escribe `::bind`, porque con el `using` el compilador elegiría el `bind` de la librería estándar y el programa compilaría sin enlazar el puerto.

Los incumplimientos se penalizan según RUBRICA.md. Como referencia: cada aparición de una construcción prohibida resta, y un número elevado impide dar por completo el tramo 1.

Quien trabaje en Windows o en macOS debe usar **WSL2**, una máquina virtual con Linux o los ordenadores del laboratorio: los sockets POSIX no compilan con el MinGW que trae CLion en Windows. Hacedlo desde la primera semana, no la última.

## 9. Uso de inteligencia artificial

**El uso de herramientas de IA está permitido y se da por hecho.** Este enunciado está escrito sabiendo que las vais a usar para generar código, buscar errores y redactar parte de la memoria. Lo que se evalúa no es si habéis escrito cada línea a mano, sino si **entendéis y controláis** lo que entregáis.

Condiciones:

1. **Anexo obligatorio de uso de IA** (`docs/anexo_ia.pdf`) con: herramientas usadas, las conversaciones o instrucciones (prompts) más relevantes, y una tabla que indique qué ficheros o funciones se generaron con IA, cuáles se modificaron después y cuáles se escribieron sin IA (plantilla en el anexo B).
2. **Adaptar a la norma es vuestro trabajo.** Las herramientas de IA generan por defecto código con `break`, clases, `struct`, `lock_guard`, lambdas y `detach()`, porque es lo más habitual fuera de esta asignatura. Pedidle expresamente que cumpla la norma y revisad el resultado: "lo generó la IA" no es una justificación para ningún incumplimiento.
3. **La IA se equivoca a menudo en concurrencia** (esperas con `sleep`, secciones críticas que no cubren comprobación y acción, hilos sin unir). Cada decisión de concurrencia se razona en la memoria con vuestras palabras.
4. **Defensa oral individual.** Cada miembro del grupo defiende la práctica por separado. El profesor puede elegir **cualquier fichero y cualquier línea** y pedir que se explique, que se modifique en el momento, o que se prediga qué pasa en una situación concurrente concreta ("¿qué ocurre si luis se desconecta justo después de que el servidor compruebe que está conectado y antes de enviarle el mensaje?").
5. **Sin defensa superada, la nota no cuenta** para ese estudiante, aunque el programa funcione perfectamente.
6. Presentar como propio código generado sin declararlo en el anexo se considera plagio y se aplica la normativa académica de la universidad.

## 10. Plan de trabajo sugerido

La práctica se presenta al comenzar el bloque de redes. Este es un plan orientativo; las fechas concretas las fija el profesor en el campus.

| Semana | Temas del curso en clase | Trabajo sugerido en la práctica |
|---|---|---|
| 1 | 10 Fundamentos de redes | Formar grupo, leer el enunciado entero, preparar el entorno Linux de todos los miembros, crear el repositorio y el `CMakeLists.txt` |
| 2 | 11 Sockets UDP | Diseñar en papel: hilos del servidor y del cliente, recursos compartidos (primera versión de la tabla), estructura de ficheros |
| 3 | 12 Sockets TCP | Cliente y servidor de eco por líneas con un solo cliente; funciones para leer y enviar líneas completas |
| 4 | 13 Servidores concurrentes | Hilo por conexión, registro e inicio de sesión en memoria, `LISTA`, `MSG`, hilo receptor en el cliente |
| 5 | 13 (continuación) | Cierre ordenado, desconexiones bruscas, revisión de secciones críticas. **Objetivo: tramo 1 completo** |
| 6 | 14 N-capas con SQLite | Separar en tres capas, integrar la amalgamación, usuarios y mensajes persistentes, consultas preparadas |
| 7 | 14 (continuación) | Mensajes diferidos, semáforo de conexiones, grupos, historial. **Objetivo: tramo 2 completo** |
| 8 | Repaso | Confirmaciones, estados, lanzador de clientes, pruebas de carga, seguridad, memoria y anexo de IA. Ensayo de defensa en el grupo |

Consejo: **terminad un tramo antes de empezar el siguiente.** Un tramo 2 a medias sobre un tramo 1 con fallos vale menos que un tramo 1 impecable.

## 11. Entregables y detalles de la entrega

- **Código fuente** en un único fichero `grupo_XX_whatsapp.zip` con la estructura de la sección 4.5, o enlace a un repositorio con acceso para el profesor y la etiqueta `entrega-final`. No se incluyen carpetas de compilación (`build`, `cmake-build-debug`) ni la base de datos.
- **Memoria técnica** (`docs/memoria.pdf`), con portada, índice y estas secciones:
  1. Resumen de lo implementado y tramo al que se opta, con la lista de requisitos cumplidos.
  2. Manual de compilación y ejecución paso a paso, incluido cómo lanzar el servidor y varios clientes en dos o más terminales.
  3. Arquitectura: diagrama de las tres capas con las funciones principales de cada una.
  4. Diagrama de hilos del servidor y del cliente: quién crea cada hilo, qué hace y quién lo une.
  5. Protocolo completo en tablas.
  6. Tabla de recursos compartidos y su protección (anexo A), con el orden de adquisición de candados si hay más de uno.
  7. Diseño de la base de datos (tablas, columnas y relaciones), desde el tramo 2.
  8. Pruebas: casos probados, capturas y, en el tramo 3, resultados de las pruebas de carga con tablas o gráficas.
  9. Problemas de concurrencia encontrados durante el desarrollo y cómo se resolvieron (esta sección se valora mucho).
  10. Reparto de trabajo entre los miembros.
- **Anexo de uso de IA** (`docs/anexo_ia.pdf`).
- **Vídeo breve opcional** (máximo 5 minutos) mostrando el sistema con varios clientes a la vez.

Condiciones de entrega:

- **El proyecto debe compilar y ejecutarse en Linux siguiendo el manual.** Si no compila, la práctica se evalúa con la misma nota que NO ENTREGADA.
- La entrega la sube un único miembro del grupo; en el nombre del zip y en la portada de la memoria aparecen todos los miembros.
- Las capturas son de pantalla completa y deben verse las terminales implicadas.
- La entrega fuera de plazo se rige por la normativa de la asignatura **[A FIJAR]**.

## 12. Evaluación

La nota funcional se calcula por tramos (sección 7); la memoria forma parte de los requisitos puntuables de cada tramo; la defensa oral individual es condición necesaria y ajusta la nota de cada estudiante; y se aplican penalizaciones por incumplir la norma, dejar hilos sin unir, cuelgues o uso de IA no declarado. El detalle, con puntos por requisito, está en RUBRICA.md.

## 13. Preguntas frecuentes

**¿Podemos usar `lock_guard` si lo explicamos en la memoria?** No. La norma es la misma para todo el código de la asignatura. La idea es que en la defensa se vea sin ambigüedad dónde empieza y dónde acaba cada sección crítica.

**La IA me ha dado una clase `Servidor` que funciona perfectamente. ¿Tengo que reescribirla?** Sí. Convertidla en funciones libres y variables compartidas bien documentadas. Es un buen ejercicio para entender qué estado comparten realmente los hilos.

**¿Cómo guardo los datos de cada conexión sin `struct`?** Con vectores paralelos indexados por número de conexión, o con `map` de la librería estándar (por ejemplo, de nombre de usuario a socket). Documentad qué candado protege cada estructura.

**¿Una sola conexión SQLite para todo el servidor o una por hilo?** Las dos opciones son válidas si se justifican. Una conexión compartida protegida por un mutex en la capa de datos es lo más sencillo y lo recomendado. Una conexión por hilo da más concurrencia pero obliga a gestionar `SQLITE_BUSY`.

**¿Qué pasa si un cliente se conecta y no escribe nada nunca?** Su hilo queda esperando en `recv`, lo cual es correcto mientras el servidor funciona. Lo importante es que en el apagado ese hilo se despierte (con `shutdown` del socket) y se pueda unir.

**Nuestro programa funciona bien con 3 clientes pero falla a veces con 50. ¿Es grave?** Es exactamente lo que buscan las pruebas de carga: casi seguro que hay una condición de carrera. Documentadla, buscadla y corregidla. Un fallo encontrado, explicado y corregido en la memoria suma; uno escondido resta.

**Trabajo en Windows. ¿Puedo entregarla con Winsock?** No. La práctica es para Linux con sockets POSIX. Usad WSL2 (CLion puede compilar dentro de WSL) o los ordenadores del laboratorio.

**¿Podemos hacer una interfaz gráfica o web?** No: añadiría dependencias externas, que están prohibidas. Un cliente de consola cuidado basta para la máxima nota.

**Un miembro del grupo no trabaja. ¿Qué hacemos?** Comunicádselo al profesor cuanto antes, no la semana de la entrega. El reparto de trabajo de la memoria y la defensa individual permiten diferenciar notas.

## 14. Pensando en Proyectos de Informática II

Este proyecto no termina en diciembre: será el punto de partida de Proyectos de Informática II, donde se ampliará con más funcionalidades, más usuarios y probablemente otras interfaces. Algunas decisiones que tomáis ahora os ahorrarán mucho trabajo después:

1. **Respetar la separación en capas.** Si la lógica de negocio no sabe nada de sockets, mañana se podrá añadir otra forma de acceso (otra interfaz, otro protocolo) sin tocar las reglas del sistema. Si la capa de datos es la única que conoce SQL, cambiar el esquema o el motor de base de datos afectará a un solo fichero.
2. **Documentar el protocolo como un contrato.** Otro equipo, o vosotros dentro de seis meses, debe poder escribir un cliente nuevo leyendo solo la tabla del protocolo.
3. **Versionar el esquema de la base de datos.** Guardad las sentencias `CREATE TABLE` en un único sitio y apuntad los cambios; en el siguiente curso habrá que añadir columnas sin perder datos.
4. **Constantes en un solo lugar**: puerto, límite de conexiones, rutas y longitudes máximas.
5. **Pruebas reproducibles y repositorio con historial.** El lanzador de clientes servirá para comprobar que las ampliaciones no rompen nada, y los commits documentan el proceso y el trabajo de cada miembro.
6. **Tabla de recursos compartidos viva.** Toda funcionalidad nueva empezará por la misma pregunta: ¿qué comparte con lo que ya existe y cómo se protege?

---

## Anexo A. Plantilla de tabla de recursos compartidos

| Recurso | Variable(s) en el código | Hilos que lo leen | Hilos que lo escriben | Mecanismo de protección | Funciones con sección crítica | Orden respecto a otros candados |
|---|---|---|---|---|---|---|
| Usuarios conectados | `usuario_conexion`, `socket_conexion` | Atención (todos) | Atención (login y salida) | `mtx_conectados` | `iniciar_sesion`, `cerrar_sesion`, `enviar_mensaje` | Siempre antes que `mtx_envio[i]` |
| ... | ... | ... | ... | ... | ... | ... |

## Anexo B. Plantilla de declaración de uso de IA

| Fichero o función | Generado con IA (sí / no / parcial) | Herramienta | Qué se modificó después y por qué | Miembro responsable |
|---|---|---|---|---|
| `servidor/capa_datos.cpp` | parcial | ... | Se sustituyó `lock_guard` por `lock` y `unlock`; se añadieron consultas preparadas | ... |
| ... | ... | ... | ... | ... |

Añadid a continuación de la tabla las conversaciones o instrucciones más relevantes, indicando en cada una qué problema intentabais resolver y si la respuesta fue correcta, incorrecta o parcialmente correcta.
