#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILENAME 255
#define MAX_CONTENIDO 1024
#define MAX_INODOS 80
#define MAX_BLOQUES 80
#define BLOQUES_POR_INODO 1

#define FILE_T 0
#define DIR_T 1

#define VACIO 0
#define OCUPADO 1

struct block {
	char contenido[MAX_CONTENIDO];
} typedef block_t;

struct inode {
	int id;
	int size;
	int type;
	mode_t mode;   // permisos
	time_t atime;  // acceso
	time_t mtime;  // modificación
	time_t ctime;
	char name[MAX_FILENAME];
	int indice_bloque;
} typedef inode_t;

struct superblock {
	int bloques;
	int inodos;
} typedef superblock_t;

struct filesystem {
	superblock_t superblock;
	inode_t inodos[MAX_INODOS];
	block_t bloques[MAX_BLOQUES];
	int bitmap_inodos[MAX_INODOS];
	int bitmap_bloques[MAX_BLOQUES];
} typedef filesystem_t;