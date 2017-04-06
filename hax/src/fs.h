#ifndef FS_H
#define FS_H

/*
 * path name declarations
 */
size_t fs_dirname_len(const char *str);
void fs_dirname_str(char *out, const char *path);
struct io_chunk_t fs_dirname(const char *path);

size_t fs_basename_len(const char *str);
void fs_basename_str(char *out, const char *path);
struct io_chunk_t fs_basename(const char *path);

#endif
