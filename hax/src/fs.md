File System API
===============


## Path String Functions

The functions in this section are used to access or modify null-terminated
path strings.

### Directory Name -- `fs_dirname` `fs_dirname_len` `fs_dirname_str`

The directory name functions take a path as input, either relative or
absolute, and produces only the directory component of the path.

The `fs_dirname` function returns an I/O chunk that generates the directory
path. The returned chunk remains valid so long as the `path` pointer is valid.

The `fs_dirname_len` function computes the length of the directory path.

The `fs_dirname_str` writes the directory path to the output string. The
buffer `out` must have enough space to write the directory path and the
trailing null byte.

### Base Name -- `fs_dirname` `fs_dirname_len` `fs_dirname_str`

The base name functions take a path as input, either relative or absolute, and
produces only the base component of the path (i.e. the last component of the
path).

The `fs_basename` function returns an I/O chunk that generates the base path.
The returned chunk remains valid so long as the `path` pointer is valid.

The `fs_basename_len` function computes the length of the base path.

The `fs_basename_str` writes the base path to the output string. The buffer
`out` must have enough space to write the base path and the trailing null
byte.
