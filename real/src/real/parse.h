#ifndef REAL_PARSE_H
#define REAL_PARSE_H

/*
 * parser declarations
 */
struct r_parse_t;

char *r_parse_open(struct r_parse_t **parse, const char *path);
void r_parse_close(struct r_parse_t *parse);

int r_parse_ch(struct r_parse_t *parse);
int r_parse_next(struct r_parse_t *parse);
void r_parse_clear(struct r_parse_t *parse);
void r_parse_error(struct r_parse_t *parse, char *msg);

#endif
