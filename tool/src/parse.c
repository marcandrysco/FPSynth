#include "common.h"


/*
 * local declarations
 */
static void parse_proc(struct io_file_t file, void *arg);


/**
 * Open a parser on a path.
 *   @parse: Ref. The output parser.
 *   @path: The path.
 *   &returns: Error.
 */
char *cir_parse_open(struct cir_parse_t **parse, const char *path)
{
	FILE *file;

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Cannot open '%s' for reading.", path);

	*parse = malloc(sizeof(struct cir_parse_t));
	(*parse)->file = file;
	(*parse)->ch = fgetc(file);
	(*parse)->token = cir_eof_v;
	(*parse)->path = strdup(path);
	(*parse)->col = (*parse)->ncol = 1;
	(*parse)->line = (*parse)->nline = 1;
	cir_parse_next(*parse);

	return NULL;
}

/**
 * Close a parser.
 *   @parse: The parser.
 */
void cir_parse_close(struct cir_parse_t *parse)
{
	cir_parse_clear(parse);

	fclose(parse->file);
	free(parse->path);
	free(parse);
}


/**
 * Parse the next character.
 *   @parse: The parser.
 *   &returns: The character.
 */
int cir_parse_ch(struct cir_parse_t *parse)
{
	parse->ch = fgetc(parse->file);
	if(parse->ch == '\n') {
		parse->ncol = 0;
		parse->nline++;
	}
	else if(parse->ch != EOF)
		parse->ncol++;

	return parse->ch;
}

/**
 * Parse the next token.
 *   @parse: The parser.
 *   &returns: Error.
 */
char *cir_parse_next(struct cir_parse_t *parse)
{
	cir_parse_clear(parse);

	while(isspace(parse->ch))
		cir_parse_ch(parse);

	parse->line = parse->nline;
	parse->col = parse->ncol;

	if(isdigit(parse->ch) || (parse->ch == '.')) {
#define onexit strbuf_destroy(&buf);
		char *endptr;
		const char *str;
		struct strbuf_t buf;

		buf = strbuf_init(32);
		do {
			strbuf_addch(&buf, parse->ch);
			cir_parse_ch(parse);
		} while(isalnum(parse->ch) || (parse->ch == '.') || (parse->ch == 'e'));

		str = strbuf_finish(&buf);

		errno = 0;
		parse->data.flt = strtod(str, &endptr);
		if((errno != 0) || (*endptr != '\0'))
			fail("Invalid number '%s'.", str);

		parse->token = cir_flt_v;
		strbuf_destroy(&buf);
#undef onexit
	}
	else if(isalpha(parse->ch)) {
		struct strbuf_t buf;

		buf = strbuf_init(32);
		do {
			strbuf_addch(&buf, parse->ch);
			cir_parse_ch(parse);
		} while(isalnum(parse->ch));

		parse->data.str = strbuf_done(&buf);
		parse->token = cir_id_v;
	}
	else if(parse->ch == '"') {
#define onexit strbuf_destroy(&buf);
		struct strbuf_t buf;

		buf = strbuf_init(32);
		cir_parse_ch(parse);
		while(parse->ch != '"') {
			if((parse->ch == '\n') || (parse->ch == EOF))
				fail("Unterminated quote.");

			strbuf_addch(&buf, parse->ch);
			cir_parse_ch(parse);
		}

		cir_parse_ch(parse);
		parse->data.str = strbuf_done(&buf);
		parse->token = cir_str_v;
#undef onexit
	}
	else if(parse->ch != EOF) {
		parse->token = parse->ch;
		cir_parse_ch(parse);
	}
	else
		parse->token = cir_eof_v;

	return NULL;
}

/**
 * Clear any data on the parser.
 *   @parse: The parser.
 */
void cir_parse_clear(struct cir_parse_t *parse)
{
	if((parse->token == cir_id_v) || (parse->token == cir_str_v) || (parse->token == cir_err_v))
		free(parse->data.str);
}


/**
 * Print out the parser location.
 *   @parse: The parser.
 *   @file: The output file.
 */
void cir_parse_print(struct cir_parse_t *parse, struct io_file_t file)
{
	hprintf(file, "%s:%u:%u", parse->path, parse->line, parse->col);
}

/**
 * Create a chunk for the parser location.
 *   @parse: The parser.
 *   &returns: The chunk.
 */
struct io_chunk_t cir_parse_chunk(struct cir_parse_t *parse)
{
	return (struct io_chunk_t){ parse_proc, parse };
}
static void parse_proc(struct io_file_t file, void *arg)
{
	cir_parse_print(arg, file);
}


/**
 * Create a new list.
 *   &returns: The empty list.
 */
struct cir_list_t *cir_list_new(void)
{
	return NULL;
}

/**
 * Delete a list.
 *   @list: The list.
 */
void cir_list_delete(struct cir_list_t *list)
{
	struct cir_list_t *tmp;

	while(list != NULL) {
		tmp = list;
		list = tmp->next;

		cir_node_delete(tmp->node);
		free(tmp->id);
		free(tmp);
	}
}


/**
 * Get a node from the list.
 *   @list: The list.
 *   @id: The identifier.
 *   &returns: The node or null.
 */
struct cir_node_t *cir_list_get(struct cir_list_t *list, const char *id)
{
	while(list != NULL) {
		if(strcmp(list->id, id) == 0)
			return list->node;

		list = list->next;
	}

	return NULL;
}

/**
 * Add a node to the list.
 *   @list: Ref. The list.
 *   @id: Consumed. The identifier.
 *   @node: The node.
 */
void cir_list_add(struct cir_list_t **list, char *id, struct cir_node_t *node)
{
	while(*list != NULL)
		list = &(*list)->next;

	*list = malloc(sizeof(struct cir_list_t));
	**list = (struct cir_list_t){ id, node, NULL };
}


/**
 * Parse a circuit from a path.
 *   @path: The path.
 *   @list: Ref. The output list.
 *   &returns: Error.
 */
char *cir_parse_list(const char *path, struct cir_list_t **list)
{
#define onexit cir_parse_close(parse); cir_list_delete(*list);
	struct cir_parse_t *parse;

	chkret(cir_parse_open(&parse, path));
	*list = cir_list_new();

	while(parse->token != cir_eof_v) {
		if(parse->token != cir_id_v)
			fail("%C: Expected identifier.", cir_parse_chunk(parse));

		if(strcmp(parse->data.str, "wire") == 0) {
			chkfail(cir_parse_next(parse));
			chkfail(cir_parse_wire(parse, list));
		}
		else {
			char *id;
			struct cir_node_t *node;

			chkfail(cir_parse_node(parse, &id, &node));
			cir_list_add(list, id, node);
		}
	}

	cir_parse_close(parse);

	return NULL;
#undef onexit
}

/**
 * Parse a node.
 *   @parse: The parser.
 *   @id: Ref. The output identifier.
 *   @node: Ref. The output node.
 *   &returns: Error.
 */
char *cir_parse_node(struct cir_parse_t *parse, char **id, struct cir_node_t **node)
{
#define onexit erase(*id); cir_node_erase(*node);
	enum cir_node_e type;

	*id = NULL;
	*node = NULL;

	if(strcmp(parse->data.str, "input") == 0)
		type = cir_input_v;
	else if(strcmp(parse->data.str, "output") == 0)
		type = cir_output_v;
	else if(strcmp(parse->data.str, "res") == 0)
		type = cir_res_v;
	else if(strcmp(parse->data.str, "cap") == 0)
		type = cir_cap_v;
	else
		fail("%C: Expected node type or 'wire'.", cir_parse_chunk(parse));

	chkfail(cir_parse_next(parse));
	if(parse->token != cir_id_v)
		fail("%C: Expected node name.", cir_parse_chunk(parse));

	*id = strdup(parse->data.str);
	chkfail(cir_parse_next(parse));

	switch(type) {
	case cir_input_v:
		*node = cir_node_input(strdup(*id));
		break;

	case cir_output_v:
		*node = cir_node_output(strdup(*id));
		break;

	case cir_res_v:
		{
			if(parse->token != cir_flt_v)
				fail("%C: Expected resistance value.", cir_parse_chunk(parse));

			*node = cir_node_res(parse->data.flt);
			chkfail(cir_parse_next(parse));
		}

		break;

	case cir_cap_v:
		{
			if(parse->token != cir_flt_v)
				fail("%C: Expected capacitance value.", cir_parse_chunk(parse));

			*node = cir_node_cap(parse->data.flt);
			chkfail(cir_parse_next(parse));
		}

		break;

	default:
		__builtin_unreachable();
	}

	if(parse->token != ';')
		fail("%C: Expected ';'.", cir_parse_chunk(parse));

	chkfail(cir_parse_next(parse));

	return NULL;
#undef onexit
}

/**
 * Parse a wire, attaching nodes.
 *   @parse: The parser.
 *   @list: The node list.
 *   &returns: Error.
 */
char *cir_parse_wire(struct cir_parse_t *parse, struct cir_list_t **list)
{
	unsigned int sel;
	struct cir_node_t *node;
	struct cir_port_t *port, *wire = NULL;

	while(parse->token != ';') {
		if(parse->token == cir_id_v) {
			node = cir_list_get(*list, parse->data.str);
			if(node == NULL)
				fatal("%C: Unknown node '%s'.", cir_parse_chunk(parse), parse->data.str);

			chkret(cir_parse_next(parse));
			if(parse->token != ':')
				fatal("%C: Expected ':'.", cir_parse_chunk(parse));

			chkret(cir_parse_next(parse));
			if(parse->token != cir_flt_v)
				fatal("%C: Expected port number.", cir_parse_chunk(parse));

			sel = parse->data.flt;
			if((double)sel != parse->data.flt)
				fatal("%C: Expected integer port number.", cir_parse_chunk(parse));

			if(sel >= node->cnt)
				fatal("%C: Port %u does not exist.", cir_parse_chunk(parse), sel);

			port = &node->port[sel];
			chkret(cir_parse_next(parse));
		}
		else if(parse->token == cir_flt_v) {
			cir_list_add(list, strdup("_"), node = cir_node_value(parse->data.flt));

			port = &node->port[0];
			chkret(cir_parse_next(parse));
		}
		else
			fatal("%C: Expected node name, value, 'gnd', or ';'.", cir_parse_chunk(parse));

		if(wire == NULL)
			wire = port;
		else
			cir_connect(wire, port);
	}

	chkret(cir_parse_next(parse));

	return NULL;
}
