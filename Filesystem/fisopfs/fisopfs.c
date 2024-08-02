#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "filesystem.h"

filesystem_t filesystem;

char filesystem_file[MAX_FILENAME] = "filesystem.fisopfs";

static void
set_inodo(inode_t inodo)
{
	inodo.id = 0;
	inodo.size = 0;
	inodo.type = -1;
	inodo.indice_bloque = -1;
	inodo.mode = 0;
	inodo.atime = time(NULL);
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);
	strcpy(inodo.name, "");
}

static int
buscar_inodo_vacio()
{
	int i = 0;
	while (i < MAX_INODOS) {
		if (filesystem.bitmap_inodos[i] == VACIO)
			return i;
		i++;
	}

	return -1;
}

static int
buscar_bloque_vacio()
{
	int i = 0;
	while (i < MAX_BLOQUES) {
		if (filesystem.bitmap_bloques[i] == VACIO)
			return i;
		i++;
	}

	return -1;
}

static int
buscar_inodo(const char *path)
{
	int i = 0;
	while (i < MAX_INODOS) {
		if (strcmp(filesystem.inodos[i].name, path) == 0)
			return i;
		i++;
	}

	return -1;
}

static int
crear_inodo(const char *path, mode_t mode, int type)
{
	int indice_inodo_vacio = buscar_inodo_vacio();
	int indice_bloque_vacio = buscar_bloque_vacio();

	if (indice_inodo_vacio < 0 || indice_bloque_vacio < 0)
		return 0;

	filesystem.bitmap_inodos[indice_inodo_vacio] = OCUPADO;
	filesystem.bitmap_bloques[indice_bloque_vacio] = OCUPADO;

	filesystem.inodos[indice_inodo_vacio].mode = mode;
	filesystem.inodos[indice_inodo_vacio].id = indice_inodo_vacio;
	filesystem.inodos[indice_inodo_vacio].indice_bloque = indice_bloque_vacio;
	filesystem.inodos[indice_inodo_vacio].size = 0;
	filesystem.inodos[indice_inodo_vacio].mtime = time(NULL);
	filesystem.inodos[indice_inodo_vacio].atime = time(NULL);
	filesystem.inodos[indice_inodo_vacio].ctime = time(NULL);
	strcpy(filesystem.inodos[indice_inodo_vacio].name, path);
	filesystem.inodos[indice_inodo_vacio].type = type;

	return 0;
}

static void
inicializar_filesystem()
{
	filesystem.superblock.bloques = MAX_BLOQUES;
	filesystem.superblock.inodos = MAX_INODOS;
	for (int i = 0; i < MAX_BLOQUES; i++)
		filesystem.bitmap_bloques[i] = VACIO;
	for (int i = 0; i < MAX_INODOS; i++) {
		filesystem.bitmap_inodos[i] = VACIO;
		set_inodo(filesystem.inodos[i]);
	}
	printf("[debug] filesystem inicializado\n");
}

static void *
fisopfs_init(struct fuse_conn_info *conn_info)
{
	printf("[debug] IMPLEMENTADO INIT\n");

	FILE *file;
	file = fopen(filesystem_file, "r");

	if (!file) {
		inicializar_filesystem();
	} else {
		int resultado = fread(&filesystem, sizeof(filesystem), 1, file);
		if (resultado != 1) {
			perror("[Error] fread: Error al cargar el "
			       "filesystem\n");
			return NULL;
		}

		fclose(file);
	}

	return NULL;
}

static int
fisopfs_getattr(const char *path, struct stat *st)
{
	printf("[debug] fisopfs_getattr - path: %s\n", path);

	if (strcmp(path, "/") == 0) {
		st->st_uid = 1717;
		st->st_mode = __S_IFDIR | 0755;
		st->st_nlink = 2;
	} else {
		int inodo_indice = buscar_inodo(path);
		if (inodo_indice < 0) {
			errno = ENOENT;
			return -ENOENT;
		}

		inode_t inodo_aux = filesystem.inodos[inodo_indice];

		st->st_mode = inodo_aux.mode;

		st->st_mode = __S_IFDIR | 0755;
		if (inodo_aux.type == FILE_T) {
			st->st_mode = __S_IFREG | 0644;
			st->st_size = 0;
		}
		st->st_atime = inodo_aux.atime;
		st->st_mtime = inodo_aux.mtime;
		st->st_ctime = inodo_aux.ctime;
		st->st_size = inodo_aux.size;
		st->st_uid = inodo_aux.id;
		st->st_gid = 0;
		st->st_ino = inodo_indice;
		st->st_blocks =
		        BLOQUES_POR_INODO;  // En nuestra implementación el inodo solo puede tener un bloque
	}

	return 0;
}

static int
fisopfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	printf("[debug] IMPLEMENTADO CREATE\n");
	return crear_inodo(path, mode, FILE_T);
}

static int
fisopfs_read(const char *path,
             char *buffer,
             size_t size,
             off_t offset,
             struct fuse_file_info *fi)
{
	printf("[debug] fisopfs_read - path: %s, offset: %lu, size: %lu\n",
	       path,
	       offset,
	       size);

	int indice = buscar_inodo(path);

	if (indice < 0) {
		perror("[Error] read: archivo no encontrado\n");
		return -ENOENT;
	}

	if (filesystem.inodos[indice].type == DIR_T) {
		perror("[Error] read: no es un archivo\n");
		return -EISDIR;
	}

	int block_index = filesystem.inodos[indice].indice_bloque;
	char *contenido = filesystem.bloques[block_index].contenido;

	int size_file = filesystem.inodos[indice].size;

	if (size_file - offset < size) {
		size = size_file - offset;
	}

	strncpy(buffer, contenido + offset, size);

	filesystem.inodos[indice].atime = time(NULL);

	return size;
}

static int
fisopfs_write(const char *path,
              const char *buf,
              size_t size,
              off_t offset,
              struct fuse_file_info *fi)
{
	printf("[debug] IMPLEMENTADO WRITE\n");

	if (size + offset > MAX_CONTENIDO) {
		return -EFBIG;
	}

	int indice_inodo = buscar_inodo(path);
	if (indice_inodo < 0) {
		fisopfs_create(path, 0755, fi);
		indice_inodo = buscar_inodo(path);
	}

	if (indice_inodo < 0) {
		return -ENOENT;
	}

	inode_t inodo = filesystem.inodos[indice_inodo];
	block_t bloque = filesystem.bloques[inodo.indice_bloque];

	if (inodo.type == DIR_T) {
		return -EISDIR;
	}

	strncpy(bloque.contenido + offset, buf, size);
	inodo.atime = time(NULL);
	inodo.mtime = time(NULL);
	inodo.size = strlen(bloque.contenido);
	bloque.contenido[inodo.size] = '\0';

	filesystem.inodos[indice_inodo] = inodo;
	filesystem.bloques[inodo.indice_bloque] = bloque;

	return (int) size;
}

static int
fisopfs_truncate(const char *path, off_t size)
{
	printf("[debug] IMPLEMENTADO TRUNCATE\n");

	if (size > MAX_CONTENIDO) {
		perror("[Error] truncate: size excede MAX_CONTENIDO\n");
		return -EINVAL;
	}

	int indice = buscar_inodo(path);
	if (indice < 0) {
		perror("[Error] truncate: archivo no encontrado\n");
		return -ENOENT;
	}

	filesystem.inodos[indice].size = size;
	filesystem.inodos[indice].mtime = time(NULL);

	return 0;
}

static int
fisopfs_utimens(const char *path, const struct timespec ts[2])
{
	printf("[debug] IMPLEMENTADO UTIMEN\n");
	int i = buscar_inodo(path);
	if (i < 0) {
		perror("[Error] utimens: archivo no encontrado");
		return -ENOENT;
	}

	if (ts == NULL) {
		perror("[Error] utimens");
		return -EACCES;
	}
	filesystem.inodos[i].atime = ts[0].tv_sec;
	filesystem.inodos[i].mtime = ts[1].tv_sec;

	return 0;
}

static int
fisopfs_unlink(const char *path)
{
	printf("[debug] IMPLEMENTADO UNLINK\n");

	int indice = buscar_inodo(path);
	if (indice < 0) {
		perror("[Error] unlink: archivo no encontrado\n");
		return -ENOENT;
	}

	if (filesystem.inodos[indice].type == DIR_T) {
		perror("[Error] unlink: no se puede eliminar un directorio\n");
		return -EISDIR;
	}

	int block_index = filesystem.inodos[indice].indice_bloque;

	memset(&filesystem.bloques[block_index], 0, sizeof(block_t));
	memset(&filesystem.inodos[indice], 0, sizeof(inode_t));
	filesystem.bitmap_inodos[indice] = VACIO;
	filesystem.bitmap_bloques[indice] = VACIO;

	return 0;
}

static int
fisopfs_mkdir(const char *path, mode_t mode)
{
	return crear_inodo(path, mode, DIR_T);
}

static int
fisopfs_readdir(const char *path,
                void *buffer,
                fuse_fill_dir_t filler,
                off_t offset,
                struct fuse_file_info *fi)
{
	printf("[debug] fisopfs_readdir - path: %s\n", path);

	// Los directorios '.' y '..'
	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);

	if (strcmp(path, "/") == 0) {
		for (int i = 0; i < MAX_INODOS; i++) {
			if (filesystem.bitmap_inodos[i] != VACIO)
				filler(buffer,
				       filesystem.inodos[i].name + 1,
				       NULL,
				       0);
		}
		return 0;
	}

	return -ENOENT;
}

static int
fisopfs_rmdir(const char *path)
{
	printf("[debug] IMPLEMENTADO RMDIR\n");

	int indice = buscar_inodo(path);

	if (indice < 0) {
		perror("[Error] rmdir: directorio no encontrado\n");
		return -ENOENT;
	}

	if (filesystem.inodos[indice].type == FILE_T) {
		perror("[Error] rmdir: no es un directorio\n");
		return -ENOTDIR;
	}

	if (filesystem.inodos[indice].size > 0) {
		perror("[Error] rmdir: no se puede borrar un directorio "
		       "no "
		       "vacío\n");
		return -ENOTEMPTY;
	}

	int block_index = filesystem.inodos[indice].indice_bloque;

	memset(&filesystem.bloques[block_index], 0, sizeof(block_t));
	memset(&filesystem.inodos[indice], 0, sizeof(inode_t));
	filesystem.bitmap_inodos[indice] = VACIO;
	filesystem.bitmap_bloques[indice] = VACIO;

	return 0;
}

static void
fisopfs_destroy(void *private_data)
{
	printf("[debug] IMPLEMENTADO DESTROY\n");


	FILE *file;
	file = fopen(filesystem_file, "w");
	if (!file) {
		perror("[Error] fopen: Error al guardar el filesystem\n");
	}

	fwrite(&filesystem, sizeof(filesystem), 1, file);
	fflush(file);
	fclose(file);
}

static struct fuse_operations operations = {
	.init = fisopfs_init,
	.getattr = fisopfs_getattr,
	.create = fisopfs_create,
	.read = fisopfs_read,
	.write = fisopfs_write,
	.truncate = fisopfs_truncate,
	.utimens = fisopfs_utimens,
	.unlink = fisopfs_unlink,
	.mkdir = fisopfs_mkdir,
	.readdir = fisopfs_readdir,
	.rmdir = fisopfs_rmdir,
	.destroy = fisopfs_destroy,
};

int
main(int argc, char *argv[])
{
	if (argc >= 4) {
		strcpy(filesystem_file, argv[3]);
		argv[3] = NULL;
		argc--;
	}

	return fuse_main(argc, argv, &operations, NULL);
}
