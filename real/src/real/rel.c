#include "../common.h"


/*
 * local declarations
 */
static void rel_chunk(struct io_file_t file, void *arg);
static void sys_chunk(struct io_file_t file, void *arg);


/**
 * Create a relation.
 *   @type: The type.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The relation.
 */
struct r_rel_t *r_rel_new(enum r_rel_e type, struct r_expr_t *left, struct r_expr_t *right)
{
	struct r_rel_t *rel;

	rel = malloc(sizeof(struct r_rel_t));
	rel->type = type;
	rel->left = left;
	rel->right = right;

	return rel;
}

/**
 * Copy a relation.
 *   @rel: The original relation.
 *   &returns: The the copied relation.
 */
struct r_rel_t *r_rel_copy(struct r_rel_t *rel)
{
	return r_rel_new(rel->type, r_expr_copy(rel->left), r_expr_copy(rel->right));
}

/**
 * Delete a relation.
 *   @rel: The relation.
 */
void r_rel_delete(struct r_rel_t *rel)
{
	r_expr_delete(rel->left);
	r_expr_delete(rel->right);
	free(rel);
}


/**
 * Create an equality relation.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The relation.
 */
struct r_rel_t *r_rel_eq(struct r_expr_t *left, struct r_expr_t *right)
{
	return r_rel_new(r_eq_v, left, right);
}


/**
 * Normalize the form of a relation.
 *   @rel: The relation.
 */
void r_rel_norm(struct r_rel_t *rel)
{
	if(!r_expr_is_zero(rel->left) && !r_expr_is_zero(rel->right)) {
		rel->left = r_expr_sub(rel->left, rel->right);
		rel->right = r_expr_zero();
	}
	else if(r_expr_is_zero(rel->left) && !r_expr_is_zero(rel->right))
		r_expr_swap(&rel->left, &rel->right);
}


/**
 * Print a relation to a file.
 *   @rel: The relation.
 *   @file: The file.
 */
void r_rel_print(const struct r_rel_t *rel, struct io_file_t file)
{
	hprintf(file, "%C %s %C", r_expr_chunk(rel->left), "=", r_expr_chunk(rel->right));
}

/**
 * Create a chunk for the relation.
 *   @rel: The relation.
 *   &returns: The chunk.
 */
struct io_chunk_t r_rel_chunk(const struct r_rel_t *rel)
{
	return (struct io_chunk_t){ rel_chunk, (void *)rel };
}
static void rel_chunk(struct io_file_t file, void *arg)
{
	r_rel_print(arg, file);
}


/**
 * Create a new system of equations.
 *   &returns: The system.
 */
struct r_sys_t *r_sys_new(void)
{
	return NULL;
}

/**
 * Delete a system of equations.
 *   @sys: The system.
 */
void r_sys_delete(struct r_sys_t *sys)
{
	struct r_sys_t *tmp;

	while(sys != NULL) {
		tmp = sys;
		sys = tmp->next;

		r_rel_delete(tmp->rel);
		free(tmp);
	}
}


/**
 * Compute the number of relation in the system of equations.
 *   @sys: The system.
 *   &returns: The count.
 */
unsigned int r_sys_cnt(struct r_sys_t *sys)
{
	unsigned int n = 0;

	while(sys != NULL)
		n++, sys = sys->next;

	return n;
}


/**
 * Add a relation to a system of equations.
 *   @sys: The system.
 *   @rel: Consumed. The relation.
 *   &returns: The new system reference, not the head of the system.
 */
struct r_sys_t **r_sys_add(struct r_sys_t **sys, struct r_rel_t *rel)
{
	while(*sys != NULL)
		sys = &(*sys)->next;

	*sys = malloc(sizeof(struct r_sys_t));
	(*sys)->rel = rel;
	(*sys)->next = NULL;

	return sys;
}


/**
 * Normalize all relations in a system of equations.
 *   @sys: The system.
 */
void r_sys_norm(struct r_sys_t *sys)
{
	while(sys != NULL) {
		r_rel_norm(sys->rel);
		sys = sys->next;
	}
}


/**
 * Remove a relation from a system.
 *   @sys: The system reference.
 *   &returns: The relation.
 */
struct r_rel_t *r_sys_remove(struct r_sys_t **sys)
{
	struct r_sys_t *tmp;
	struct r_rel_t *rel;

	tmp = *sys;
	*sys = tmp->next;

	rel = tmp->rel;
	free(tmp);

	return rel;
}

/**
 * Purge a relation from the system.
 *   @sys: The system reference.
 */
void r_sys_purge(struct r_sys_t **sys)
{
	r_rel_delete(r_sys_remove(sys));
}


/**
 * Print a system to a file.
 *   @sys: The system.
 *   @file: The file.
 */
void r_sys_print(const struct r_sys_t *sys, struct io_file_t file)
{
	while(sys != NULL) {
		hprintf(file, "%C\n", r_rel_chunk(sys->rel));
		sys = sys->next;
	}
}

/**
 * Create a chunk for the system.
 *   @sys: The system.
 *   &returns: The chunk.
 */
struct io_chunk_t r_sys_chunk(const struct r_sys_t *sys)
{
	return (struct io_chunk_t){ sys_chunk, (void *)sys };
}
static void sys_chunk(struct io_file_t file, void *arg)
{
	r_sys_print(arg, file);
}
