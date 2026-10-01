# Simulacro del Parcial 1: Programación Concurrente y Distribuida

**Contenidos:** creación de hilos, candados (mutex) y semáforos. Las barreras no entran, pero se aceptan si se usan bien.
**Duración:** 90 minutos.
**Puntuación:** 10 puntos (teoría 2, trazas 2, detección de errores 3, programación 3).
**Norma de código:** prohibidos `break`, `continue`, `switch` y definir clases o structs propios. Los candados se toman y se sueltan con `lock()` y `unlock()`. Todo hilo creado se une con `join()`. Un código que no cumpla la norma puntúa como máximo la mitad de su apartado.

En todos los fragmentos se suponen incluidas las cabeceras necesarias (`<iostream>`, `<thread>`, `<mutex>`, `<semaphore>`, `<string>`, `<chrono>`, `<functional>`).

---

## Parte 1. Teoría (2 puntos, 0,2 cada pregunta)

Responde en dos o tres líneas como máximo.

1. ¿Qué es la **interfoliación** de dos hilos? ¿Por qué decimos que un programa concurrente es correcto solo si lo son **todas** sus interfoliaciones?
2. `contador++` ocupa una sola línea. ¿Por qué no es una operación **atómica**?
3. Define **condición de carrera** y di qué dos requisitos tienen que darse sobre un dato para que exista.
4. ¿Qué es una **sección crítica**? ¿Por qué conviene que sea lo más corta posible?
5. Explica la diferencia entre una propiedad de **seguridad** y una de **vivacidad**, con un ejemplo de cada una.
6. Diferencia entre **interbloqueo** e **inanición**.
7. ¿Por qué `thread t(funcion, contador);` no permite que el hilo modifique la variable `contador` de `main`? ¿Cómo se arregla?
8. ¿Qué ocurre si un objeto `thread` se destruye sin haber hecho `join()`?
9. Di dos diferencias entre un `mutex` y un `binary_semaphore`.
10. ¿Por qué un ejemplo con una condición de carrera puede dar el resultado "correcto" al compilarlo con `-O2`? ¿Significa eso que el programa está bien?

---

## Parte 2. ¿Qué puede imprimir? (2 puntos)

### 2.1 (0,6 puntos)

```cpp
void escribir(char letra) {
    for (int i = 0; i < 2; i++) {
        cout << letra;
    }
}

int main() {
    thread t1(escribir, 'A');
    thread t2(escribir, 'B');
    t1.join();
    t2.join();
    cout << "\n";
    return 0;
}
```

a) Escribe **todas** las salidas posibles.
b) ¿Puede imprimir `ABBB`? ¿Y `BBAA`? Justifícalo.

### 2.2 (0,8 puntos)

Dos hilos ejecutan a la vez esta función sobre la misma variable `x`, que empieza valiendo 0. Cada línea numerada es un paso indivisible.

```cpp
void incrementar(int& x) {
    int t = x;      // paso 1: leer
    t = t + 1;      // paso 2: sumar
    x = t;          // paso 3: escribir
}
```

a) ¿Qué valores finales puede tener `x`? ¿Puede valer 0? ¿Puede valer 3?
b) ¿Cuántas interfoliaciones distintas hay de los 6 pasos?
c) ¿En cuántas de ellas `x` termina valiendo 2? Escribe una de ellas como tabla de pasos (hilo, paso, valor de `x`).

### 2.3 (0,6 puntos)

```cpp
counting_semaphore<2> sala(2);
mutex mtx_consola;

void visitar(int id) {
    sala.acquire();
    mtx_consola.lock();
    cout << "entra " << id << "\n";
    mtx_consola.unlock();

    this_thread::sleep_for(chrono::milliseconds(100));

    mtx_consola.lock();
    cout << "sale " << id << "\n";
    mtx_consola.unlock();
    sala.release();
}
// main crea tres hilos con visitar(1), visitar(2) y visitar(3) y los une con join.
```

Contesta sí o no y justifica:
a) ¿Pueden aparecer `entra 1`, `entra 2` y `entra 3` seguidas, sin ningún `sale` entre ellas?
b) ¿Puede ser `entra 3` la primera línea?
c) ¿Puede aparecer `sale 2` antes que `entra 2`?

---

## Parte 3. Detección de errores (3 puntos, 0,6 cada fragmento)

Para cada fragmento: **qué ocurre al ejecutarlo** (o al compilarlo), **por qué** y **cómo se corrige** respetando la norma.

### 3.1

```cpp
void sumar(int total, int cantidad) {
    total = total + cantidad;
}

int main() {
    int total = 0;
    thread t(sumar, total, 5);
    t.join();
    cout << "Total: " << total << "\n";   // se espera 5
    return 0;
}
```

### 3.2

```cpp
void saludar() {
    cout << "hola desde el hilo\n";
}

int main() {
    thread t(saludar);
    cout << "fin del main\n";
    return 0;
}
```

### 3.3

```cpp
mutex mtx_taquilla;
int entradas = 10;

bool comprar() {
    mtx_taquilla.lock();
    if (entradas == 0) {
        return false;
    }
    entradas--;
    mtx_taquilla.unlock();
    return true;
}
```

### 3.4

```cpp
mutex mtx_plazas;
int plazas = 1;

void reservar(int id) {
    if (plazas > 0) {
        mtx_plazas.lock();
        plazas--;
        mtx_plazas.unlock();
        cout << "Reserva confirmada para " << id << "\n";
    }
}
```

### 3.5

```cpp
counting_semaphore<5> huecos(5);
counting_semaphore<5> elementos(0);
mutex mtx_bufer;

void productor(int valor) {
    mtx_bufer.lock();
    huecos.acquire();
    poner_en_bufer(valor);
    mtx_bufer.unlock();
    elementos.release();
}
```

---

## Parte 4. Programación (3 puntos)

Un aparcamiento tiene **3 plazas**. Llegan **10 coches**, y cada coche aparca **2 veces**: entra, se queda 50 ms y sale.

Escribe el programa completo en C++20 de forma que:

1. Cada coche sea un hilo que ejecuta la función `coche(int id)`.
2. Nunca haya más de 3 coches dentro a la vez.
3. Se lleve una variable compartida `coches_dentro` y otra `maximo_dentro`, sin condiciones de carrera.
4. Cada vez que un coche entra o sale se imprima una línea completa (sin mezclarse con otras) con su id y los coches que hay dentro.
5. Al terminar, `main` imprima el máximo de coches que llegó a haber dentro y el número total de estancias (debe ser 20).
6. Se cumpla la norma de la asignatura.

---
---

# SOLUCIONARIO Y CRITERIOS DE CORRECCIÓN

## Parte 1

1. Interfoliación: el orden concreto en que se intercalan las instrucciones atómicas de los hilos; dentro de cada hilo se respeta su orden. Como no controlamos qué interfoliación ocurre, basta que una sea incorrecta para que el programa pueda fallar.
2. Son tres pasos de máquina: leer el valor, sumarle 1 y escribirlo. Otro hilo puede ejecutarse entre ellos.
3. Varios hilos acceden al mismo dato a la vez y el resultado depende del orden. Requisitos: el dato es **compartido** y **al menos uno escribe** (condiciones de Bernstein).
4. El fragmento de código que accede a un recurso compartido y que solo puede ejecutar un hilo a la vez. Corta, porque mientras un hilo está dentro los demás esperan: una sección crítica larga convierte el programa en secuencial.
5. Seguridad: "nunca pasa algo malo" (nunca hay dos hilos en la sección crítica). Vivacidad: "algo bueno acaba pasando" (un hilo que quiere entrar acaba entrando).
6. Interbloqueo: un grupo de hilos se espera mutuamente y **ninguno** avanza. Inanición: el sistema avanza, pero **un hilo concreto** nunca consigue el recurso.
7. `thread` copia los argumentos: el hilo trabaja sobre una copia. Se corrige declarando el parámetro como `int&` y pasando `ref(contador)`.
8. El destructor llama a `terminate()` y el programa aborta.
9. El mutex tiene dueño (solo quien hace `lock()` puede hacer `unlock()`); el semáforo no, y cualquier hilo puede hacer `release()`. El semáforo puede empezar a 0 y servir de señal entre hilos; el mutex no.
10. El optimizador puede convertir el bucle en una única suma y la ventana de la carrera desaparece. El programa sigue siendo incorrecto: es comportamiento indefinido y puede fallar con otro compilador, otra máquina u otra carga.

**Criterio:** 0,2 si la idea es correcta y usa el vocabulario adecuado; 0,1 si es incompleta; 0 si confunde conceptos (por ejemplo, interbloqueo con inanición).

## Parte 2

**2.1** a) Las 6 mezclas de `AA` con `BB` que conservan el orden interno: `AABB`, `ABAB`, `ABBA`, `BAAB`, `BABA`, `BBAA`. b) `ABBB` no: solo hay dos `B`. `BBAA` sí: t2 puede ejecutarse entero antes que t1.
**Criterio:** 0,4 las seis salidas (0,2 si faltan una o dos), 0,2 el apartado b.

**2.2** a) Solo 1 o 2. No puede valer 0 (cada hilo escribe al menos una vez un valor mayor o igual que 1) ni 3 (solo hay dos incrementos). b) 20 (combinaciones de 6 elementos tomados de 3 en 3). c) En 2: cuando un hilo hace sus tres pasos completos antes de que el otro lea (A1 A2 A3 B1 B2 B3, x = 1 tras A3 y x = 2 tras B3; o la simétrica). En las otras 18 los dos leen 0 y se pierde un incremento.
**Criterio:** 0,3 apartado a, 0,2 apartado b, 0,3 apartado c con la tabla.

**2.3** a) No: el semáforo solo tiene 2 permisos; el tercero no puede imprimir `entra` hasta que alguien libere, y se libera después de imprimir `sale`. b) Sí: el orden de arranque de los hilos no está garantizado. c) No: dentro de un mismo hilo `entra` va antes que `sale`.
**Criterio:** 0,2 cada apartado con justificación; 0,1 si solo dice sí o no.

## Parte 3

**3.1** Imprime `Total: 0`. Dos errores: el parámetro es por valor y además `thread` copia los argumentos. Si solo se cambia a `int& total`, no compila (el hilo intenta pasar una copia temporal a una referencia). Corrección: `void sumar(int& total, int cantidad)` y `thread t(sumar, ref(total), 5);`.

**3.2** Aborta: al terminar `main` se destruye `t` sin `join()` y se llama a `terminate()`. Además "hola desde el hilo" puede no llegar a salir. Corrección: `t.join();` antes de `return 0;`.

**3.3** Si `entradas` vale 0, la función retorna con el mutex cogido y el siguiente comprador se bloquea para siempre (vivacidad). Corrección con un único punto de salida:

```cpp
bool comprar() {
    bool comprada = false;
    mtx_taquilla.lock();
    if (entradas > 0) {
        entradas--;
        comprada = true;
    }
    mtx_taquilla.unlock();
    return comprada;
}
```

**3.4** La comprobación `plazas > 0` está fuera del mutex. Dos hilos pueden ver `plazas == 1`, entrar los dos y dejar `plazas` en -1: dos reservas para una plaza (seguridad). Comprobar y modificar es una única sección crítica:

```cpp
void reservar(int id) {
    bool reservada = false;
    mtx_plazas.lock();
    if (plazas > 0) {
        plazas--;
        reservada = true;
    }
    mtx_plazas.unlock();
    if (reservada) {
        cout << ("Reserva confirmada para " + to_string(id) + "\n");
    }
}
```

(También vale imprimir con un mutex de consola; lo importante es la sección crítica.)

**3.5** Interbloqueo cuando el búfer está lleno: el productor coge el mutex y se duerme en `huecos.acquire()`; ningún consumidor puede entrar al búfer para dejar un hueco porque necesita ese mutex. Corrección: primero `huecos.acquire()` y después `mtx_bufer.lock()`.

**Criterio por fragmento:** 0,2 qué ocurre, 0,2 por qué, 0,2 corrección que cumpla la norma.

## Parte 4. Solución modelo

```cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <string>
#include <thread>
#include <vector>

using namespace std;

const int PLAZAS = 3;
const int COCHES = 10;
const int VECES = 2;

counting_semaphore<PLAZAS> plazas_libres(PLAZAS);
mutex mtx_aparcamiento;
int coches_dentro = 0;
int maximo_dentro = 0;
int estancias = 0;
mutex mtx_consola;

void imprimir(const string& texto) {
    mtx_consola.lock();
    cout << texto << "\n";
    mtx_consola.unlock();
}

void coche(int id) {
    for (int vez = 0; vez < VECES; vez++) {
        plazas_libres.acquire();

        mtx_aparcamiento.lock();
        coches_dentro++;
        estancias++;
        if (coches_dentro > maximo_dentro) {
            maximo_dentro = coches_dentro;
        }
        int dentro = coches_dentro;
        mtx_aparcamiento.unlock();
        imprimir("Entra coche " + to_string(id) + ", dentro: " + to_string(dentro));

        this_thread::sleep_for(chrono::milliseconds(50));

        mtx_aparcamiento.lock();
        coches_dentro--;
        dentro = coches_dentro;
        mtx_aparcamiento.unlock();
        imprimir("Sale coche " + to_string(id) + ", dentro: " + to_string(dentro));

        plazas_libres.release();
    }
}

int main() {
    vector<thread> coches;
    for (int i = 0; i < COCHES; i++) {
        coches.push_back(thread(coche, i));
    }
    for (int i = 0; i < COCHES; i++) {
        coches[i].join();
    }
    cout << "Maximo dentro: " << maximo_dentro << ", estancias: " << estancias << "\n";
    return 0;
}
```

**Criterios (3 puntos):**

| Criterio | Puntos |
|---|---|
| Crea 10 hilos y los une todos con `join` | 0,5 |
| Semáforo contador de 3 con `acquire` antes de entrar y `release` después de salir, en todos los caminos | 0,8 |
| `coches_dentro`, `maximo_dentro` y `estancias` protegidos con mutex; el valor que se imprime se copia dentro de la sección crítica | 0,8 |
| Salida por consola sin mezclarse (mutex de consola o línea completa en una sola escritura) | 0,3 |
| Resultado final correcto (máximo como mucho 3 y 20 estancias) | 0,3 |
| Cumple la norma (sin `break`, sin clases, `lock` y `unlock` a mano) | 0,3 |

Penalizaciones: usar un mutex en lugar del semáforo para limitar las plazas (solo deja un coche dentro) resta 0,5; poner el `sleep` dentro de la sección crítica del mutex resta 0,3 (convierte el aparcamiento en secuencial); leer `coches_dentro` fuera del mutex para imprimirlo resta 0,2.
