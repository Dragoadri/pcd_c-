# Guion de clase: 99 práctica final (mensajería instantánea concurrente)

**Duración estimada:** 50 minutos de presentación, 3 sesiones de seguimiento de 30 minutos y las defensas (unos 15 minutos por estudiante)
**Entra en examen:** No (es la práctica final; se evalúa con RUBRICA.md)

## Objetivo de la sesión

Que los grupos salgan de la presentación sabiendo qué tienen que construir, qué se evalúa en cada tramo, que la norma de código se aplica igual que en clase y que la defensa oral individual es lo que decide si la nota cuenta.

## Antes de empezar (preparación del profesor)

- Fijar en ENUNCIADO.md y en el campus las fechas marcadas como **[FECHA A FIJAR]** (entrega antes de Navidad y defensas) y la normativa de entrega tardía.
- Tener preparado un servidor concurrente de la carpeta 13 para enseñar "esto es el tramo 1 en pequeño".
- Preparar la hoja de grupos (3 o 4 personas) en el campus.
- Recomendado: comprobar en los ordenadores del laboratorio que CLion compila sockets POSIX, porque parte de la clase trabaja en Windows o macOS (ver aviso en la sección 8 del enunciado).

## Pizarra

1. Dibujo de la arquitectura: un rectángulo "servidor" con tres franjas (presentación, negocio, datos) y un cilindro "whatsapp.db" debajo; a la izquierda, tres clientes con dos flechas cada uno (hilo de envío, hilo receptor).
2. Dentro del servidor, el hilo principal con el bucle de aceptación y una flecha "un hilo por conexión" hacia varios hilos de atención.
3. Una lista titulada "¿Qué comparten?": usuarios conectados, sockets, base de datos, registro, bandera de apagado. Se deja en la pizarra toda la sesión.
4. Los tres tramos como escalera: 5, 7, 10, con la frase "no se sube un escalón sin terminar el anterior".
5. La norma en una esquina: sin break, sin continue, sin switch, sin clases ni struct, lock y unlock a mano, todo hilo con join.

## Secuencia de la presentación

| Orden | Contenido | Min | Qué cuento |
|---|---|---|---|
| 1 | Demostración de un servidor concurrente de la carpeta 13 con tres terminales | 5 | "Esto ya lo sabéis hacer; la práctica es convertirlo en un sistema de verdad" |
| 2 | Secciones 3 y 4 del enunciado: programas y arquitectura | 10 | Qué es cada capa en un servidor; por qué la presentación aquí es el protocolo |
| 3 | Sección 5: protocolo y ejemplo de diálogo | 10 | Por qué líneas (TCP es un flujo), respuestas frente a notificaciones asíncronas |
| 4 | Sección 6: requisitos de concurrencia | 10 | Recorrer la lista de la pizarra; la regla "comprobar y actuar en la misma sección crítica" |
| 5 | Sección 7 y RUBRICA.md: tramos y penalizaciones | 5 | Tramo 1 impecable antes que tramo 2 a medias |
| 6 | Sección 9: uso de IA y defensa | 5 | La IA genera break, clases y lock_guard; la defensa es individual |
| 7 | Formación de grupos y dudas | 5 | Entorno Linux de todos los miembros esta misma semana |

## Preguntas a la clase antes de terminar la presentación

- **"Si Ana y Luis hacen LOGIN con el mismo usuario en el mismo instante, ¿qué puede pasar si la comprobación y el alta están en dos secciones críticas?"** Respuesta esperada: los dos comprueban que no está conectado y los dos entran.
- **"¿Por qué el cliente necesita dos hilos?"** Respuesta esperada: porque `getline` bloquea y los mensajes deben aparecer sin pulsar nada.
- **"¿Qué pasa si leemos una orden con un único `recv`?"** Respuesta esperada: pueden llegar dos órdenes juntas o media orden.
- **"¿Cómo termina un hilo que está bloqueado en `recv` cuando apagamos el servidor?"** Respuesta esperada: cerrando su socket con `shutdown`, que hace que `recv` devuelva.

## Sesiones de seguimiento sugeridas

| Sesión | Momento | Qué se revisa con cada grupo |
|---|---|---|
| Seguimiento 1 | Tras la carpeta 12 | Entorno Linux de todos, repositorio, diseño en papel de hilos y primera tabla de recursos compartidos |
| Seguimiento 2 | Tras la carpeta 13 | Demostración del tramo 1 con tres clientes; apagado ordenado; búsqueda de construcciones prohibidas con `grep` |
| Seguimiento 3 | Tras la carpeta 14 | Capas separadas, consultas preparadas, mensajes diferidos; plan para el tramo 3 |

Truco para los seguimientos: pedir a un miembro al azar que explique una función escrita por otro. Es un ensayo de la defensa y detecta pronto a quien no está trabajando.

## Organización de las defensas

- Publicar un calendario con franjas de 15 minutos por estudiante; los miembros de un grupo defienden en franjas consecutivas y sin estar presentes los demás.
- Antes de la defensa, el profesor compila la entrega en limpio, pasa el `grep` de construcciones prohibidas, ejecuta la batería de pruebas de la rúbrica y anota dos o tres líneas concretas del código para preguntar.
- Estructura de cada defensa: 2 minutos de demostración dirigida por el profesor, 8 minutos de preguntas sobre código, 5 minutos de modificación en directo.
- Registrar el resultado (solvencia, dudas, no supera) con una frase que lo justifique.

## Preguntas tipo para la defensa oral

**Tramo 1**

- Enséñame dónde se crea el hilo de atención y dónde se une. ¿Qué pasa si se borra ese `join`?
- Señala todas las secciones críticas de la función que envía un mensaje privado. ¿Qué candado protege cada una?
- Luis se desconecta justo después de que el servidor compruebe que está conectado y antes de enviarle el mensaje. ¿Qué ocurre en vuestro código?
- ¿Por qué no hay `break` en vuestro bucle de lectura? ¿Cómo termina?
- Modificación: añade la orden `HORA` que devuelve la hora del servidor.

**Tramo 2**

- ¿Por qué SQLite necesita protección si ya es "thread safe"? ¿Qué habéis elegido y por qué?
- Enséñame una consulta preparada y explica qué pasaría si la construyerais concatenando el nombre de usuario.
- Hay `MAX_CONEXIONES` clientes conectados y llega uno más. Recorre el código y dime qué recibe.
- ¿En qué orden se toman el candado de usuarios conectados y el de envío? ¿Qué pasaría si una función los tomara al revés?
- Modificación: añade la orden `GRUPO_MIEMBROS` respetando las capas.

**Tramo 3**

- ¿Cómo sabéis que no se pierde ningún mensaje con 100 clientes? Enséñame la comprobación.
- ¿Por qué el mensaje `ENTREGADO` puede llegar al autor antes que la respuesta `OK;MSG`? ¿Lo gestiona vuestro cliente?
- ¿Qué aporta la sal en el hash de la contraseña? ¿Por qué `hash` no sirve?
- El registro de actividad lo escriben todos los hilos. Enséñame qué pasaría sin su candado.
- ¿Qué parte de este fichero generó la IA y qué tuvisteis que cambiar?

## Errores típicos de los alumnos

- Empezar por la base de datos y los grupos sin tener estable el tramo 1.
- Proteger la escritura de cada estructura pero no la comprobación previa (comprobar y actuar en secciones críticas distintas).
- Hacer `recv` o consultas largas con un candado tomado, lo que serializa todo el servidor.
- Olvidar el `unlock` en un camino de error con `return`.
- Usar `detach()` "porque el servidor no termina nunca", y descubrir tarde que el cierre ordenado es obligatorio.
- No usar `MSG_NOSIGNAL` y ver morir el servidor al cerrar un cliente de golpe.
- Pegar código de IA con `lock_guard`, clases y `while (true)` sin adaptarlo.
- Dejar el entorno Linux para la última semana.

## Cierre y enlace con Proyectos de Informática II

Recordar en la última sesión que el proyecto se reutiliza en Proyectos de Informática II: conviene conservar el repositorio, el lanzador de pruebas y la memoria, y tomarse en serio la separación en capas y la documentación del protocolo (sección 14 del enunciado).
