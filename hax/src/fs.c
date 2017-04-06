#include "common.h"


/*
 * local declarations
 */
static void dirname_proc(struct io_file_t file, void *arg);
static void basename_proc(struct io_file_t file, void *arg);


/**
 * Create a chunk for the directory name.
 *   @path: The path.
 *   &returns: The chunk.
 */
struct io_chunk_t fs_dirname(const char *path)
{
	return (struct io_chunk_t){ dirname_proc, (void *)path };
}
static void dirname_proc(struct io_file_t file, void *arg)
{
	char tmp[fs_dirname_len(arg) + 1];

	fs_dirname_str(tmp, arg);
	hprintf(file, "%s", tmp);
}

/**
 * Calculate the directory name length.
 *   @str: The string.
 *   &returns: The length, not including the null byte.
 */
size_t fs_dirname_len(const char *str)
{
	const char *endptr;

	endptr = strrchr(str, '/');
	if(endptr == NULL)
		return 1;
	else if(endptr == str)
		return 1;
	else
		return endptr - str;
}

/**
 * Copy the directory name to the output.
 *   @out: The output.
 *   @path: The path.
 */
void fs_dirname_str(char *out, const char *path)
{
	const char *endptr;

	endptr = strrchr(path, '/');
	if(endptr == NULL)
		strcpy(out, ".");
	else if(endptr == path)
		strcpy(out, "/");
	else {
		memcpy(out, path, endptr - path);
		out[endptr-path] = '\0';
	}
}


/**
 * Create a chunk for the directory name.
 *   @path: The path.
 *   &returns: The chunk.
 */
struct io_chunk_t fs_basename(const char *path)
{
	return (struct io_chunk_t){ basename_proc, (void *)path };
}
static void basename_proc(struct io_file_t file, void *arg)
{
	char tmp[fs_basename_len(arg) + 1];

	fs_basename_str(tmp, arg);
	hprintf(file, "%s", tmp);
}

/**
 * Calculate the base name length.
 *   @str: The string.
 *   &returns: The length, not including the null byte.
 */
size_t fs_basename_len(const char *str)
{
	const char *endptr;

	endptr = strrchr(str, '/');
	if(endptr == NULL)
		return strlen(str);
	else
		return str + strlen(str) - endptr - 1;
}

/**
 * Copy the base name to the output.
 *   @out: The output.
 *   @path: The path.
 */
void fs_basename_str(char *out, const char *path)
{
	const char *endptr;

	endptr = strrchr(path, '/');
	strcpy(out, (endptr != NULL) ? (endptr + 1) : path);
}
