#include "../common.h"


enum r_parse_e {
	r_parse_flt_v = 0x100,
	r_parse_id_v = 0x101,
	r_parse_eof_v = 0xFF00,
	r_parse_err_v = 0xFF01,
};

/**
 * Parser data union.
 *   @flt: Floating-point number.
 */
union r_parse_u {
	double flt;
	char *str;
};

/**
 * Parser structure.
 *   @ch, token: The buffered character and token.
 *   @file: The file.
 *   @path: The path.
 *   @line, col, next; The line and column information.
 *   @data: The token data.
 */
struct r_parse_t {
	int ch, token;
	FILE *file;

	char *path;
	unsigned int line, col, nline, ncol;

	union r_parse_u data;
};


/**
 * Open a parser.
 *   @parse: Ref. The output parser.
 *   @path: The path.
 *   &returns: Error.
 */
char *r_parse_open(struct r_parse_t **parse, const char *path)
{
	FILE *file;

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Failed to open '%s' for reading. %s.", path, strerror(errno));

	*parse = malloc(sizeof(struct r_parse_t));
	(*parse)->file = file;
	(*parse)->path = strdup(path);
	(*parse)->line = (*parse)->nline = 1;
	(*parse)->col = (*parse)->ncol = 1;

	(*parse)->token = r_parse_eof_v;
	(*parse)->ch = fgetc((*parse)->file);

	return NULL;
}

/**
 * Close a parser.
 *   @parse: The parser.
 */
void r_parse_close(struct r_parse_t *parse)
{
	r_parse_clear(parse);
	fclose(parse->file);
	free(parse->path);
	free(parse);
}


/**
 * Read the next character.
 *   @parse: The parser.
 *   &returns: The character.
 */
int r_parse_ch(struct r_parse_t *parse)
{
	parse->ch = fgetc(parse->file);
	if(parse->ch == '\n') {
		parse->nline++;
		parse->ncol = 1;
	}
	else
		parse->ncol++;

	return parse->ch;
}

/**
 * Read th next token from the parser.
 *   @parse: The parser.
 *   &returns: The token.
 */
int r_parse_next(struct r_parse_t *parse)
{
	r_parse_clear(parse);

	while(isspace(parse->ch))
		r_parse_ch(parse);

	parse->line = parse->nline;
	parse->col = parse->ncol;

	if(isdigit(parse->ch)) {
		char *endptr;
		const char *str;
		struct strbuf_t buf;

		buf = strbuf_init(32);

		do {
			strbuf_addch(&buf, parse->ch);
			r_parse_ch(parse);
		} while(isdigit(parse->ch));

		str = strbuf_finish(&buf);

		errno = 0;
		parse->token = r_parse_flt_v;
		parse->data.flt = strtod(str, &endptr);
		if((errno != 0) || (*endptr != '\0'))
			r_parse_error(parse, mprintf("Invalid number '%s'.", str));

		strbuf_destroy(&buf);
	}
	else if(isalpha(parse->ch) || (parse->ch == '_')) {
		struct strbuf_t buf;

		buf = strbuf_init(32);

		do {
			strbuf_addch(&buf, parse->ch);
			r_parse_ch(parse);
		} while(isalnum(parse->ch) || (parse->ch == '_') || (parse->ch == '\''));

		parse->token = r_parse_id_v;
		parse->data.str = strbuf_done(&buf);
	}
	else {
		parse->token = parse->ch;
		r_parse_ch(parse);
	}

	return parse->token;
}

/**
 * Clear all data in the parser.
 *   @parse: The parser.
 */
void r_parse_clear(struct r_parse_t *parse)
{
	if((parse->token == r_parse_err_v))
		free(parse->data.str);

	parse->token = r_parse_eof_v;
}

/**
 * Set an error message on the parser.
 *   @parse: The parser.
 */
void r_parse_error(struct r_parse_t *parse, char *msg)
{
	parse->token = r_parse_err_v;
	parse->data.str = msg;
}

char *r_parse_expr(struct r_parse_t *parse, struct r_expr_t **expr)
{
	return NULL;
}
