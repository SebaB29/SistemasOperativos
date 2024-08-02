# shell

### Búsqueda en $PATH

**¿Cuáles son las diferencias entre la syscall execve(2) y la familia de wrappers proporcionados por la librería estándar de C (libc) exec(3)?**

La función execve toma la ruta (path) del binario a ejecutar, un arreglo con el comando y sus argumentos, y un arreglo con las variables de entorno. En cambio, algunas de las funciones de la familia exec (como execv, execvp, etc.) solo requieren la ruta del binario o su nombre, mientras que otras aceptan el comando y sus argumentos como un arreglo. Otra diferencia es que execve requiere un arreglo separado que contenga las variables de entorno, mientras que en la familia de wrappers algunas funciones requieren que se les pasen las variables de entorno directamente, mientras que otras utilizan la variable de entorno $PATH para determinar la ubicación del binario.
Con respecto a los errores, execve lanza un error específico si no puede ejecutar el binario por razones como falta de permisos (EACCESS), archivo inexistente, etc. La familia de wrappers manejan los errores de manera más elaborada, por ejemplo, algunas de ellas continúan buscando un binario para ejecutar si la ejecución inicial falla debido a un error como EACCESS.

**¿Puede la llamada a exec(3) fallar? ¿Cómo se comporta la implementación de la shell en ese caso?**

La llamada a exec puede fallar ya sea por falta de permisos, errores de entrada/salida o de archivo inexistente entre otros siendo estos los más comunes. La forma en la que nuestra implementación se comporta ante esas posibles fallas es imprimiendo un mensaje de error y luego se procede a la interrupción del proceso.

---

### Procesos en segundo plano

**Detallar cuál es el mecanismo utilizado para implementar procesos en segundo plano.**

El mecanismo utilizado para la implementación de los procesos en segundo plano es cuando, al recibir el “&” desde el prompt, se detecta el mismo y el comando recibido se ejecuta de forma normal, luego se permite seguir utilizando la shell a través de la entrada estándar output gracias a la syscall waitpid con el flag WNOHANG la cual permite que la shell no espere de forma bloqueante.

---

### Flujo estándar

**Investigar el significado de 2>&1, explicar cómo funciona su forma general**

El “&1” indica el descriptor de archivo de la salida estándar (stdout), la dirección “2” es el descriptor de archivo de error estándar (stderr) y la expresión “>” es el operador de redirección.
“2>&1” redirecciona los posibles errores que pueda tener un comando a la misma salida estándar (stdout).

**Mostrar qué sucede con la salida de cat out.txt en el ejemplo. Luego repetirlo, invirtiendo el orden de las redirecciones (es decir, 2>&1 >out.txt). ¿Cambió algo? Compararlo con el comportamiento en bash(1).**

- En la implementación de nuestra shell la salida de “cat out.txt” es:

```bash
$ ls -C /home /noexiste >out.txt 2>&1
$ cat out.txt
ls: no se puede acceder a ‘/noexiste’: No existe el archivo o el directorio
/home:
(nombre_de_usuario)
```

Luego al invertir el orden (>out.txt 2>&1 a 2>&1 >out.txt) se muestra el mismo resultado.

- En la implementación del bash la salida de “cat out.txt” es:

```bash
$ ls -C /home /noexiste >out.txt 2>&1
$ cat out.txt
ls: no se puede acceder a ‘/noexiste’: No existe el archivo o el directorio
/home:
(nombre_de_usuario)
```

Luego al invertir el orden (>out.txt 2>&1 a 2>&1 >out.txt), se muestra por pantalla el mensaje de error. Sin embargo, a diferencia del caso anterior, el mensaje de error no se escribió en el archivo “out.txt”. Por ende, al ejecutar el comando “cat out.txt” no muestra los errores.

```bash
$ ls -C /home /noexiste 2>&1 >out.txt
ls: no se puede acceder a ‘/noexiste’: No existe el archivo o el directorio
$ cat out.txt
/home:
(nombre_de_usuario)
```

---

### Tuberías múltiples

**Investigar qué ocurre con el exit code reportado por la shell si se ejecuta un pipe ¿Cambia en algo? ¿Qué ocurre si, en un pipe, alguno de los comandos falla? Mostrar evidencia (e.g. salidas de terminal) de este comportamiento usando bash. Comparar con su implementación.**

Si se ejecuta un pipe, por ejemplo, comando 1 | comando 2, el exit code que reporta la shell será el del último comando, sin importar si el comando 1 falla o no. Es decir, si el comando 1 falla pero el comando 2 no, el exit code reportado será cero, que representa un exit code exitoso. Por otro lado, si el comando 1 se ejecuta correctamente pero el comando 2 falla, el exit code será un número distinto de cero.

- Ejemplos de salidas de terminal si alguno de los comandos falla usando bash (vamos a estar utilizamos el comando “echo $?” para mostrar el valor de los exit code):

  - Caso en el que el comando 1 falla pero el comando 2 se ejecuta con éxito:

  ```bash
  $ prueba_error | echo hola
  hola
  prueba_error: command not found
  $ echo $?
  0
  ```

  A pesar de que el comando 1 haya dado un error el exit code del programa es 0 ya que el comando 2 se ejecutó exitosamente.

  - Caso en el que el comando 1 se ejecuta con éxito pero el comando 2 falla:

  ```bash
  $ echo hola | prueba_error
  prueba_error: command not found
  $ echo $?
  127
  ```

  Como se esperaba el exit code es un número distinto de cero que corresponde a la falla del último comando.

- Ejemplos de salidas de terminal si alguno de los comandos falla en nuestra implementación:

  - Caso en el que el comando 1 falla pero el comando 2 se ejecuta con éxito:

  ```bash
  $ prueba_error | echo hola
  hola
  $ echo $?
  0
  ```

  - Caso en el que el comando 1 se ejecuta con éxito pero el comando 2 falla:

  ```bash
  $ echo hola | prueba_error
  $ echo $?
  0
  ```

  En nuestra implementación sin importar si el último comando falla o no el exit code siempre va a ser 0.

---

### Variables de entorno temporarias

**¿Por qué es necesario hacerlo luego de la llamada a fork(2)?**

Es necesario hacerlo luego de la llamada a fork para que no esté viviendo en la shell sino en la ejecución del comando ya que deben ser variables temporales. Por lo tanto, la variable sólo le pertenece únicamente al proceso hijo durante su ejecución.

**En algunos de los wrappers de la familia de funciones de exec(3) (las que finalizan con la letra e), se les puede pasar un tercer argumento (o una lista de argumentos dependiendo del caso), con nuevas variables de entorno para la ejecución de ese proceso. Supongamos, entonces, que en vez de utilizar setenv(3) por cada una de las variables, se guardan en un arreglo y se lo coloca en el tercer argumento de una de las funciones de exec(3).
¿El comportamiento resultante es el mismo que en el primer caso? Explicar qué sucede y por qué.
Describir brevemente (sin implementar) una posible implementación para que el comportamiento sea el mismo.**

El comportamiento resultante no es el mismo que cuando se utiliza setenv ya que al pasarle un arreglo de variables de entorno, estas serán las únicas variables disponibles para ese proceso después de la ejecución de exec. En cambio, si utilizamos setenv, las variables se agregan o actualizan en el entorno existente del proceso.

Para lograr que tengan el mismo comportamiento se puede crear un array de las variables de entorno que contenga las variables de entorno deseadas (contenidas en environ o nuevas variables). Luego este array se lo pasa a la función exec.

---

### Pseudo-variables

**Investigar al menos otras tres variables mágicas estándar, y describir su propósito. Incluir un ejemplo de su uso en bash (u otra terminal similar).**

_$PWD_: tiene la ruta del directorio en el que te encuentras actualmente.

```bash
$ echo $PWD
/mnt/c/Users/”nombre_usuario”/Desktop
```

_$HOME_: tiene la ruta del directorio de inicio.

```bash
$ echo $HOME
/home/”nombre_usuario”
```

_$USER_: tiene el nombre de usuario.

```bash
$ echo $USER
“nombre usuario”
```

_$0_: tiene el nombre del comando que se está ejecutando.

```bash
$ echo $0
-bash
```

_$$_: tiene el identificador del proceso actual.

```bash
$ echo $$
296
```

_$_\_: tiene el último argumento del comando ejecutado anteriormente.

```bash
$ echo ejemplo de prueba
ejemplo de prueba
$ echo $_
prueba
```

_$#_: tiene la cantidad de argumentos pasados al script o función.

```bash
$ echo $#
0
```

---

### Comandos built-in

**¿Entre cd y pwd, alguno de los dos se podría implementar sin necesidad de ser built-in? ¿Por qué? ¿Si la respuesta es sí, cuál es el motivo, entonces, de hacerlo como built-in? (para esta última pregunta pensar en los built-in como true y false)**

Un comando "built-in" se refiere a una función o instrucción que ya está integrada en la shell. Estos comandos están disponibles sin necesidad de cargar bibliotecas adicionales o ejecutar programas externos.

No es posible implementar el comando cd que no sea built-in, porque no existe un ejecutable (binario), por lo que no se puede ejecutar mediante “execvp”, al ser este una propiedad del proceso de la misma shell. Por otra parte, el comando “pwd”, si tiene un ejecutable (binario) asociado por lo que “execvp” puede localizar y ejecutar este comando. Para esto último realizamos una prueba en nuestra shell del comando “pwd” con y sin la implementación, y notamos que el comando se ejecutaba correctamente sin necesidad de haberlo implementado.

El motivo por el cual se hace built- in es que se evita, como ya se comentó al inicio, el tener que cargar bibliotecas adicionales o ejecutar programas externos volviendo la ejecución de los comandos mucho más eficiente y rápida.

---

### Segundo plano avanzado

**Explicar detalladamente el mecanismo completo utilizado**

Se atrapa la señal "SIGCHLD" utilizando la syscall "sigaction". En esta syscall, se configura el manejador de señales "sigchild_handler", lo que permite que cada vez que un hijo finaliza su ejecución, envíe la señal "SIGCHLD" al padre y en vez de ser ignorada, como era su comportamiento por dafault, se ejecute la funcion que se configuro para manejar esa señal que lo que va a hacer es imprimir por pantalla el id del proceso en segundo plano que termino y su status. También se agrego el flag SA_RESTART al sigaction para que una vez que el manejador de señales haya terminado su ejecución se reinicie automáticamente el proceso que fue interrumpido.

```C
static void
sigchild_handler()
{
	pid_t pid;
	int status;

	pid = waitpid(0, &status, WNOHANG);

	if (pid > 0)
		printf_debug("===> terminado: PID=%d - STATUS=%d\n", pid, status);
}
```

Para que solo se imprima cuando se termine un proceso en segundo plano, ya que la función "sigchild_handler" se va a ejecutar siempre que un hijo termine sin importar si es en segundo plano o no, se modifica la syscall waitpid de la siguiente manera: se le pasa como primer parámetro un 0, lo cual indica que esperará a todos los procesos hijos que tengan el mismo ID de grupo de procesos que el proceso padre (shell). Además, se utiliza el flag WNOHANG, que permite que la función waitpid funcione de forma no bloqueante. Esto significa que si no hay ningún hijo que haya cambiado de estado, la función devolverá 0 y el proceso padre continuará ejecutándose. En caso contrario, devolverá el ID del proceso hijo en segundo plano que ha terminado, lo que permitirá que el manejador de señales sigchild_handler ejecute el condicional con la información de ese proceso.

Diferenciar a los procesos en segundo plano de los que no lo son implica mantener el ID de grupo de procesos de la shell para los procesos en segundo plano, ya que todos los procesos hijo heredan el mismo ID de grupo de procesos del padre. Para los procesos que no están en segundo plano (es decir, los que no son del tipo BACK), se cambio su ID de grupo de procesos utilizando la syscall setpgid(p, getpid()), lo que hace que su ID de grupo de procesos sea igual a su process id.

**¿Por qué es necesario el uso de señales?**

Sin el uso de señales, la shell no tendría una forma eficiente de saber cuándo un proceso hijo termina, lo que podría llevar a un comportamiento ineficiente o incorrecto al manejar procesos en segundo plano. Además también tendría que depender de mecanismos de consulta periódica, lo que podría resultar en un consumo innecesario de recursos del sistema y una respuesta menos ágil. Por lo tanto es necesario su uso para poder manejar las comunicaciones entre procesos, interrupciones, finalización de procesos en segundo plano y/o eventos que ocurriesen como así también notificar al usuario de las mismas.

Además de la señal SIGCHLD, hay muchas otras señales que pueden ser relevantes en diferentes contextos como, SIGINT (Ctrl+C), SIGTERM, SIGKILL, entre otras.


---
