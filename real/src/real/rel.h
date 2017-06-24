#ifndef REAL_REL_H
#define REAL_REL_H

/**
 * Relation type enumerator.
 *   @r_eq_v: Equality.
 */
enum r_rel_e {
	r_eq_v,
};

/**
 * Relation structure.
 *   @type: The type.
 *   @left, right: The left and right expressions.
 */
struct r_rel_t {
	enum r_rel_e type;
	struct r_expr_t *left, *right;
};

/*
 * relation declarations
 */
struct r_rel_t *r_rel_new(enum r_rel_e type, struct r_expr_t *left, struct r_expr_t *right);
struct r_rel_t *r_rel_copy(struct r_rel_t *rel);
void r_rel_delete(struct r_rel_t *rel);

struct r_rel_t *r_rel_eq(struct r_expr_t *left, struct r_expr_t *right);

void r_rel_norm(struct r_rel_t *rel);

void r_rel_print(const struct r_rel_t *rel, struct io_file_t file);
struct io_chunk_t r_rel_chunk(const struct r_rel_t *rel);


/**
 * System of equations.
 *   @rel: The relation.
 *   @next: The next system.
 */
struct r_sys_t {
	struct r_rel_t *rel;

	struct r_sys_t *next;
};

/*
 * system of equations declarations
 */
struct r_sys_t *r_sys_new(void);
void r_sys_delete(struct r_sys_t *sys);

unsigned int r_sys_cnt(struct r_sys_t *sys);

struct r_sys_t **r_sys_add(struct r_sys_t **sys, struct r_rel_t *rel);

void r_sys_norm(struct r_sys_t *sys);

struct r_rel_t *r_sys_remove(struct r_sys_t **sys);
void r_sys_purge(struct r_sys_t **sys);

void r_sys_print(const struct r_sys_t *sys, struct io_file_t file);
struct io_chunk_t r_sys_chunk(const struct r_sys_t *sys);

#endif
