#ifndef PARSE_H
#define PARSE_H

/**
 * Parser token enumerator.
 *   @cir_id_v: Identifier.
 *   @cir_str_v: String.
 *   @cir_flt_v: Floating-point value.
 *   @cir_err_v: Error.
 *   @cir_eof_v: End-of-file.
 */
enum cir_parse_e {
	cir_id_v = 0x100,
	cir_str_v = 0x101,
	cir_flt_v = 0x102,
	cir_err_v = 0xFFFE,
	cir_eof_v = 0xFFFF
};

/**
 * Parser data union.
 *   @str: String.
 *   @flt: Floating-point value.
 */
union cir_parse_u {
	char *str;
	double flt;
};


/**
 * Circuit parser structure.
 *   @file: The file.
 *   @ch: The current character.
 *   @token: The token.
 *   @data: Associated data.
 *   @path: The path.
 *   @col, line, ncol, nline: Column and line information.
 */
struct cir_parse_t {
	FILE *file;
	int ch;
	uint16_t token;
	union cir_parse_u data;

	char *path;
	unsigned int col, line, ncol, nline;
};

/*
 * parser declarations
 */
char *cir_parse_open(struct cir_parse_t **parse, const char *path);
void cir_parse_close(struct cir_parse_t *parse);

int cir_parse_ch(struct cir_parse_t *parse);
char *cir_parse_next(struct cir_parse_t *parse);
void cir_parse_clear(struct cir_parse_t *parse);

void cir_parse_print(struct cir_parse_t *parse, struct io_file_t file);
struct io_chunk_t cir_parse_chunk(struct cir_parse_t *parse);

/*
 * list declarations
 */
struct cir_list_t *cir_list_new(void);
void cir_list_delete(struct cir_list_t *list);

struct cir_node_t *cir_list_get(struct cir_list_t *list, const char *id);
void cir_list_add(struct cir_list_t **list, char *id, struct cir_node_t *node);


char *cir_parse_list(const char *path, struct cir_list_t **list);
char *cir_parse_node(struct cir_parse_t *parse, char **id, struct cir_node_t **node);
char *cir_parse_wire(struct cir_parse_t *parse, struct cir_list_t **list);


/**
 * List structure.
 *   @id: The identifier.
 *   @node: The node.
 *   @next: The next element.
 */
struct cir_list_t {
	char *id;
	struct cir_node_t *node;

	struct cir_list_t *next;
};

#endif
