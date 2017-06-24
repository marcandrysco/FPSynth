#ifndef REAL_ARITH_H
#define REAL_ARITH_H


/**
 * Addition structure.
 *   @left, right: The left and right expressions.
 */
struct r_add_t {
	struct r_expr_t *left, *right;
};

/**
 * Multiply structure.
 *   @left, right: The left and right expressions.
 */
struct r_mul_t {
	struct r_expr_t *left, *right;
};


/*
 * add declarations
 */
struct r_add_t *r_add_new(struct r_expr_t *left, struct r_expr_t *right);
struct r_add_t *r_add_copy(struct r_add_t *add);
void r_add_delete(struct r_add_t *add);

void r_add_print(const struct r_add_t *add, struct io_file_t file);
struct io_chunk_t r_add_chunk(const struct r_add_t *add);

/*
 * mul declarations
 */
struct r_mul_t *r_mul_new(struct r_expr_t *left, struct r_expr_t *right);
struct r_mul_t *r_mul_copy(struct r_mul_t *mul);
void r_mul_delete(struct r_mul_t *mul);

void r_mul_print(const struct r_mul_t *mul, struct io_file_t file);
struct io_chunk_t r_mul_chunk(const struct r_mul_t *mul);

#endif
