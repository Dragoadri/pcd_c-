# Guion de clase: 10 fundamentos de redes

**Duración estimada:** 110 minutos (1 sesión)
**Entra en examen:** Parcial 2 (vocabulario: IP, puerto, DNS, localhost, TCP frente a UDP, htons)

## Objetivo de la sesión

Que el alumno sepa qué necesita un programa para hablar con otro (IP y puerto), qué es `127.0.0.1`, para qué sirve el DNS y por qué existen `htons` e `inet_pton`, que va a teclear en todos los ejemplos de sockets. El peso de la sesión está en la pizarra y en `APUNTES_REDES.md`; el código son tres consultas cortas al sistema.

## Antes de empezar (preparación del profesor)

- Tener abierto un terminal con letra grande para `hostname`, `ip -4 a`, `getent hosts`, `ss -tulpn`.
- Comprobar si `nc` (netcat) está instalado en los equipos del aula.
- Llevar pensada la analogía de la ciudad: edificio, dirección postal, buzón, guía telefónica, portería.
- Aviso de entorno: los sockets POSIX de este bloque solo compilan en Linux (o WSL, o macOS). Los alumnos con Windows y MinGW necesitan WSL o los equipos del laboratorio.

## Pizarra

1. Dibujo de dos edificios (dos ordenadores) con varios buzones numerados cada uno. Escribo al lado: **IP = dirección del edificio, puerto = buzón**.
2. Dentro de un edificio, un buzón con el cartel "servidor 5200" y una persona mirándolo. Desde el otro edificio, alguien que escribe a "edificio X, buzón 5200": el cliente.
3. Un edificio con un bucle que sale y vuelve a él: `127.0.0.1 = localhost = hablar conmigo mismo`.
4. Una guía telefónica con una flecha: `www.universidadeuropea.com -> 34.98.96.131` (DNS).
5. Una urbanización con portería: IPs privadas `192.168.x.x` dentro, una IP pública fuera (NAT). Frase: "desde fuera no se puede llamar a un piso concreto".
6. Tabla de dos columnas **cartas (UDP)** y **llamada (TCP)**: conexión, llega todo, llega en orden. Solo la idea; el detalle es de las carpetas 11 y 12.
7. El número 5000 escrito en dos cajitas de byte: `13 88` y `88 13`. Palabras: little endian (mi PC), big endian (la red), `htons`.

## Secuencia de ficheros

| Orden | Fichero | Min | Qué cuento mientras lo tecleo |
|---|---|---|---|
| 0 | Pizarra y `APUNTES_REDES.md` apartados 1 a 7 | 45 | Analogía de la ciudad, IP, máscara, localhost, puerto, DNS, NAT, cliente/servidor. Intercalo comandos `hostname`, `ip -4 a`, `ping -c 3 127.0.0.1`. |
| 1 | `01_mis_direcciones_ip.cpp` | 15 | `gethostname` y la lista enlazada de `getifaddrs`. Recorrido con `while` hasta `nullptr` (repaso de punteros). Comparar con `ip -4 a`. |
| 2 | `02_resolver_nombre_dns.cpp` | 15 | `getaddrinfo`: nombre a IP. Un nombre puede tener varias IPs. Qué pasa sin red: no se cuelga, informa. Comparar con `getent hosts`. |
| 3 | `03_ip_y_puerto_en_binario.cpp` | 20 | `inet_pton` y `htons`. Leer bytes en memoria con un puntero a `unsigned char`. El error de olvidar `htons`. |
| 4 | `APUNTES_REDES.md` apartados 8, 10 y 11 | 15 | Tabla TCP/UDP, `ss -tulpn` para ver puertos abiertos, tabla de llamadas POSIX que vendrán. |

## Preguntas a la clase antes de ejecutar

- **01:** "¿Cuántas IPs creéis que tiene vuestro portátil?" Esperado: una. Realidad: al menos dos (`lo` y la wifi), y más si hay Docker o máquinas virtuales.
- **01:** "¿Tendrá mi portátil la misma IP que el vuestro?" `127.0.0.1` sí, todos; la de la wifi no.
- **02:** "¿Qué IP saldrá para localhost?" `127.0.0.1`. "¿Y si desconecto la wifi?" localhost sigue funcionando; el nombre de Internet da error pero el programa no se rompe.
- **02:** "¿Por qué puede salir más de una IP para un nombre?" Varios servidores reparten la carga.
- **03:** "Si guardo 5000 en un `uint16_t`, ¿qué byte va primero en memoria?" Casi nadie lo sabe: el bajo (136) en un PC. "¿Y en la red?" El alto.
- **03:** "Si el servidor olvida `htons` y el cliente no, ¿se encuentran?" No: uno está en 5000 y otro en 34835.

## Errores típicos de los alumnos

- Confundir IP con puerto, o creer que el puerto identifica al ordenador.
- Pensar que `127.0.0.1` es "la IP de mi ordenador en Internet" y darla a un compañero para que se conecte.
- Dar a un compañero la IP de Docker (`172.17.0.1`) en vez de la de la wifi.
- Olvidar `htons` al rellenar el puerto, o imprimir el puerto de un cliente sin `ntohs`.
- No comprobar el valor de retorno de las llamadas POSIX (todas devuelven `-1` si fallan).
- Olvidar `freeifaddrs` o `freeaddrinfo` (la memoria la reservó el sistema).
- Esperar poder conectar desde casa al servidor de un compañero sin saber qué es NAT.

## Cierre y enlace con la siguiente carpeta

Resumen en una frase: "para hablar con un programa necesito su IP y su puerto, ambos en orden de red". En la carpeta 11 enviamos los primeros datos de verdad con UDP, las cartas: veremos que se pueden enviar sin que nadie escuche y sin recibir error.
