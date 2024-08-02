# Sistemas Operativos
Trabajos Prácticos de la materia Sistemas Operativos (TA043)

# Trabajo Individual (Fork)
## Descripción
<p align="justify">
El objetivo de este lab es familiarizarse con las llamadas al sistema fork(2) (que crea una copia del proceso actual) y pipe(2) (que proporciona un mecanismo de comunicación unidireccional entre dos procesos).
</p>

# Trabajos Grupales (Shell/Scheduler/Filesystem)
## Integrantes
* Sebastián Brizuela
* Lucia Agha Zadeh Dehdeh --> [Github](https://github.com/Lucia-azd)
* Federico Solari --> [Github](https://github.com/fedesolari)
* Juan Sebastian Del Rio --> [Github](https://github.com/S2JuanS2)

## Descripción
### Shell
En este trabajo se va a desarrollar la funcionalidad mínima que caracteriza a un intérprete de comandos shell similar a lo que realizan bash, zsh, fish.

### Scheduler
<p align="justify">
En este trabajo se implementarán el mecanismo de cambio de contexto para procesos y el scheduler (i.e. planificador) sobre un sistema operativo preexistente.
El kernel a utilizar será una modificación de JOS, un exokernel educativo con licencia libre del grupo de Sistemas Operativos Distribuidos del MIT.
JOS está diseñado para correr en la arquitectura Intel x86, y para poder ejecutarlo utilizaremos QEMU que emula dicha arquitectura.
</p>

### Filesystem
<p align="justify">
En este trabajo implementaremos nuestro propio sistema de archivos (o filesystem) para Linux. El sistema de archivos utilizará el mecanismo de FUSE (Filesystem in USErspace) provisto por el kernel, que nos permitirá definir en modo usuario la implementación de un filesystem. Gracias a ello, el mismo tendrá la interfaz VFS y podrá ser accedido con las syscalls y programas habituales (read, open, ls, etc).
La implementación del filesystem será enteramente en memoria: tanto archivos como directorios serán representados mediante estructuras que vivirán en memoria RAM. Por esta razón, buscamos un sistema de archivos que apunte a la velocidad de acceso, y no al volumen de datos o a la persistencia (algo similar a tmpfs). Aún así, los datos de nuestro filesystem estarán representados en disco por un archivo.
</p>
