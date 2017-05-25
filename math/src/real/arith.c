#include "../common.h"


/*
 * local declarations
 */
static void add_proc(struct io_file_t file, void *arg);
static void mul_proc(struct io_file_t file, void *arg);


/**
 * Create an addition.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The addition.
 */
struct r_add_t *r_add_new(struct r_expr_t *left, struct r_expr_t *right)
{
	struct r_add_t *add;

	add = malloc(sizeof(struct r_add_t));
	add->left = left;
	add->right = right;

	return add;
}

/**
 * Copy an addition.
 *   @expr: The original addition.
 *   &returns: The copied addition.
 */
struct r_add_t *r_add_copy(struct r_add_t *add)
{
	return r_add_new(r_expr_copy(add->left), r_expr_copy(add->right));
}

/**
 * Delete an addition.
 *   @add: The addition.
 */
void r_add_delete(struct r_add_t *add)
{
	r_expr_delete(add->left);
	r_expr_delete(add->right);
	free(add);
}


/**
 * Print an addition.
 *   @add: The addition.
 *   @file: The file.
 */
void r_add_print(const struct r_add_t *add, struct io_file_t file)
{
	hprintf(file, "(%C+%C)", r_expr_chunk(add->left), r_expr_chunk(add->right));
}

/**
 * Retrieve a chunk for an addition.
 *   @add: The addition.
 *   &returns: The chunk.
 */
struct io_chunk_t r_add_chunk(const struct r_add_t *add)
{
	return (struct io_chunk_t){ add_proc, (void *)add };
}
static void add_proc(struct io_file_t file, void *arg)
{
	r_add_print(arg, file);
}


/**
 * Create a multiplication.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The multiplication.
 */
struct r_mul_t *r_mul_new(struct r_expr_t *left, struct r_expr_t *right)
{
	struct r_mul_t *mul;

	mul = malloc(sizeof(struct r_mul_t));
	mul->left = left;
	mul->right = right;

	return mul;
}

/**
 * Copy a multiplication.
 *   @expr: The original multiplication.
 *   &returns: The copied multiplication.
 */
struct r_mul_t *r_mul_copy(struct r_mul_t *mul)
{
	return r_mul_new(r_expr_copy(mul->left), r_expr_copy(mul->right));
}

/**
 * Delete a multiplication.
 *   @mul: The multiplication.
 */
void r_mul_delete(struct r_mul_t *mul)
{
	r_expr_delete(mul->left);
	r_expr_delete(mul->right);
	free(mul);
}


/**
 * Print a multiplication.
 *   @mul: The multiplication.
 *   @file: The file.
 */
void r_mul_print(const struct r_mul_t *mul, struct io_file_t file)
{
	hprintf(file, "(%C*%C)", r_expr_chunk(mul->left), r_expr_chunk(mul->right));
}

/**
 * Retrieve a chunk for a multiplication.
 *   @mul: The multiplication.
 *   &returns: The chunk.
 */
struct io_chunk_t r_mul_chunk(const struct r_mul_t *mul)
{
	return (struct io_chunk_t){ mul_proc, (void *)mul };
}
static void mul_proc(struct io_file_t file, void *arg)
{
	r_mul_print(arg, file);
}
