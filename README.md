# 🖥 **Sistemas Operativos** - Trabajos Prácticos
<p align="justify">
Bienvenidos al repositorio de <b>Sistemas Operativos (TA043)</b>. Aquí encontrarás los trabajos prácticos realizados a lo largo de la materia, los cuales abarcan conceptos clave como la creación y gestión de procesos, planificación, y sistemas de archivos.
</p>

---

## 📂 **Contenido**

1. [Trabajo Individual: Fork](#trabajo-individual-fork)
2. [Trabajos Grupales](#trabajos-grupales)
   - [Shell](#shell)
   - [Scheduler](#scheduler)
   - [Filesystem](#filesystem)

---

## 🔀 **Trabajo Individual: Fork** <a name="trabajo-individual-fork"></a>

### 📝 **Descripción**
<p align="justify">
El objetivo de este lab es familiarizarse con las llamadas al sistema <i>fork(2)</i> (que crea una copia del proceso actual) y <i>pipe(2)</i> (que proporciona un mecanismo de comunicación unidireccional entre dos procesos).
</p>

---

## 👥 **Trabajos Grupales** <a name="trabajos-grupales"></a>

### 👨‍👩‍👧‍👦 **Equipo**
| **Integrante**                     | **Github**                                 |
|------------------------------------|--------------------------------------------|
| Sebastián Brizuela                 | -                                          |
| Lucia Agha Zadeh Dehdeh            | [Lucia-azd](https://github.com/Lucia-azd)  |
| Federico Solari                    | [fedesolari](https://github.com/fedesolari)|
| Juan Sebastian Del Rio             | [S2JuanS2](https://github.com/S2JuanS2)    |

---

### ⌨️ **Shell** <a name="shell"></a>

En este trabajo se va a desarrollar la funcionalidad mínima que caracteriza a un intérprete de comandos shell similar a lo que realizan bash, zsh, fish.

---

### ⏳ **Scheduler** <a name="scheduler"></a>

<p align="justify">
En este trabajo se implementarán el mecanismo de cambio de contexto para procesos y el scheduler (i.e. planificador) sobre un sistema operativo preexistente.
El kernel a utilizar será una modificación de JOS, un exokernel educativo con licencia libre del grupo de Sistemas Operativos Distribuidos del MIT.
JOS está diseñado para correr en la arquitectura Intel x86, y para poder ejecutarlo utilizaremos QEMU que emula dicha arquitectura.
</p>

---

### 🗂 **Filesystem** <a name="filesystem"></a>

<p align="justify">
En este trabajo implementaremos nuestro propio sistema de archivos (o filesystem) para Linux. El sistema de archivos utilizará el mecanismo de FUSE (Filesystem in USErspace) provisto por el kernel, que nos permitirá definir en modo usuario la implementación de un filesystem. Gracias a ello, el mismo tendrá la interfaz VFS y podrá ser accedido con las syscalls y programas habituales (read, open, ls, etc).
La implementación del filesystem será enteramente en memoria: tanto archivos como directorios serán representados mediante estructuras que vivirán en memoria RAM. Por esta razón, buscamos un sistema de archivos que apunte a la velocidad de acceso, y no al volumen de datos o a la persistencia (algo similar a tmpfs). Aún así, los datos de nuestro filesystem estarán representados en disco por un archivo.
</p>

## 📄 Licencia  
Este proyecto está bajo la licencia MIT. Para más detalles, consulta el archivo [LICENSE](./LICENSE).
