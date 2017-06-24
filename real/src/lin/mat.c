#include "../common.h"


/**
 * Create an expression matrix.
 *   @width; The width.
 *   @height: The height.
 *   &returns: The expression matrix.
 */
struct rmat_expr_t *rmat_expr_new(unsigned int width, unsigned int height)
{
	unsigned int i;
	struct rmat_expr_t *mat;

	mat = malloc(sizeof(struct rmat_expr_t));
	mat->width = width;
	mat->height = height;
	mat->arr = malloc(width * height * sizeof(void *));

	for(i = 0; i < (width * height); i++)
		mat->arr[i] = r_expr_zero();

	return mat;
}

/**
 * Delete an expression matrix.
 *   @mat: The expression matrix.
 */
void rmat_expr_delete(struct rmat_expr_t *mat)
{
	unsigned int i;

	for(i = 0; i < (mat->width * mat->height); i++)
		r_expr_delete(mat->arr[i]);

	free(mat->arr);
	free(mat);
}


/**
 * Dump a matrix to standard out.
 *   @mat: The matrix.
 */
void rmat_expr_dump(struct rmat_expr_t *mat)
{
	unsigned int i, j;

	for(i = 0; i < mat->height; i++) {
		for(j = 0; j < mat->width; j++)
			printf("%C\t", r_expr_chunk(*rmat_expr_get(mat, j, i)));

		printf("\n");
	}
}


/**
 * Get an expression reference from the expression matrix.
 *   @mat: The expression matrix.
 *   @row: The row.
 *   @col: The column.
 *   &returns; The expression reference.
 */
struct r_expr_t **rmat_expr_get(struct rmat_expr_t *mat, unsigned int row, unsigned int col)
{
	assert((col < mat->width) && (row < mat->height));

	return &mat->arr[row * mat->width + col];
}


/**
 * Check if expression matrix is zero.
 *   @mat: The expression matrix.
 *   &returns: True if zero.
 */
bool rmat_expr_is_zero(struct rmat_expr_t *mat)
{
	unsigned int i, len;

	len = mat->width * mat->height;

	for(i = 0; i < len; i++) {
		if(!r_expr_is_zero(mat->arr[i]))
			return false;
	}

	return true;
}

/**
 * Check if expression matrix is the identity.
 *   @mat: The expression matrix.
 *   &returns: True if zero.
 */
bool rmat_expr_is_ident(struct rmat_expr_t *mat)
{
	assert(mat->width == mat->height);

	fatal("stub");
}


/**
 * Compute the determinant of an expression matrix.
 *   @mat: The expression matrix.
 *   &returns: The determinant expression.
 */
struct r_expr_t *rmat_expr_det(struct rmat_expr_t *mat)
{
	assert(mat->width == mat->height);

	unsigned int i;
	struct r_list_t *list;
	struct r_expr_t *sel, *expr;
	struct rmat_expr_t *tmp;

	if(mat->width == 1)
		return r_expr_copy(mat->arr[0]);

	list = r_list_new();

	for(i = 0; i < mat->width; i++) {
		sel = *rmat_expr_get(mat, 0, i);
		if(r_expr_is_zero(sel))
			continue;

		tmp = rmat_expr_exclude(mat, 0, i);
		expr = rmat_expr_det(tmp);
		rmat_expr_delete(tmp);

		if(!r_expr_is_one(sel))
			expr = r_expr_mul(r_expr_copy(sel), expr);

		if((i % 2) == 1)
			expr = r_expr_neg(expr);

		r_list_add(&list, expr);
	}

	if(list == NULL)
		r_list_add(&list, r_expr_zero());

	return r_expr_sum(list);
}

/**
 * Compute the determinant of an expression matrix, clearing the input.
 *   @mat: Consumed. The expression matrix.
 *   &returns: The determinant expression.
 */
struct r_expr_t *rmat_expr_det_clr(struct rmat_expr_t *mat)
{
	struct r_expr_t *res;

	res = rmat_expr_det(mat);
	rmat_expr_delete(mat);

	return res;
}


/**
 * Compute the inverse of a matrix.
 *   @mat: The matrix.
 *   &returns: The inverse.
 */
struct rmat_expr_t *rmat_expr_inv(struct rmat_expr_t *mat)
{
	assert(mat->width == mat->height);

	struct r_expr_t *det, *co;
	struct rmat_expr_t *inv;
	unsigned int i, j;

	det = rmat_expr_det(mat);
	inv = rmat_expr_new(mat->width, mat->height);

	for(j = 0; j < mat->width; j++) {
		for(i = 0; i < mat->height; i++) {
			co = r_expr_div(rmat_expr_det_clr(rmat_expr_exclude(mat, j, i)), r_expr_copy(det));

			if(((i + j) % 2) == 1)
				co = r_expr_neg(co);

			r_expr_set(rmat_expr_get(inv, i, j), co);
		}
	}

	r_expr_delete(det);

	return inv;
}


/**
 * Create an expression matrix excluding a row and column.
 *   @mat: The matrix.
 *   @row: The row.
 *   @col: The column.
 *   &returns: The expression.
 */
struct rmat_expr_t *rmat_expr_exclude(struct rmat_expr_t *mat, unsigned int row, unsigned int col)
{
	unsigned int i, j;
	struct rmat_expr_t *res;

	res = rmat_expr_new(mat->width - 1, mat->height - 1);

	for(j = 0; j < res->width; j++) {
		for(i = 0; i < res->height; i++)
			r_expr_set(rmat_expr_get(res, j, i), r_expr_copy(*rmat_expr_get(mat, (j >= row) ? (j + 1) : j, (i >= col) ? (i + 1) : i)));
	}

	return res;
}


/**
 * Create a float matrix.
 *   @w; The width.
 *   @h: The height.
 *   &returns: The float matrix.
 */
struct rmat_flt_t *rmat_flt_new(unsigned int w, unsigned int h)
{
	unsigned int i;
	struct rmat_flt_t *mat;

	mat = malloc(sizeof(struct rmat_flt_t));
	mat->arr = malloc(w * h * sizeof(double));

	for(i = 0; i < (w * h); i++)
		mat->arr[i] = 0.0;

	return mat;
}

/**
 * Delete a float matrix.
 *   @mat: The float matrix.
 */
void rmat_flt_delete(struct rmat_flt_t *mat)
{
	free(mat);
}
