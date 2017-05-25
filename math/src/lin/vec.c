#include "../common.h"


/**
 * Create an expression vector.
 *   @len: The length.
 *   &returns: The expression vector.
 */
struct rvec_expr_t *rvec_expr_new(unsigned int len)
{
	unsigned int i;
	struct rvec_expr_t *vec;

	vec = malloc(sizeof(struct rvec_expr_t));
	vec->arr = malloc(len * sizeof(void *));
	vec->len = len;

	for(i = 0; i < len; i++)
		vec->arr[i] = r_expr_zero();

	return vec;
}

/**
 * Delete an expression vector.
 *   @vec: The expression vector.
 */
void rvec_expr_delete(struct rvec_expr_t *vec)
{
	unsigned int i;

	for(i = 0; i < vec->len; i++)
		r_expr_delete(vec->arr[i]);

	free(vec->arr);
	free(vec);
}


/**
 * Dump an expression vector.
 *   @vec: The expression vector.
 */
void rvec_expr_dump(struct rvec_expr_t *vec)
{
	unsigned int i;

	for(i = 0; i < vec->len; i++)
		printf("%C\n", r_expr_chunk(vec->arr[i]));
}


/**
 * Multiply a matrix and a vector as expressions.
 *   @mat: The expression matrix.
 *   @vec: The expression vector.
 *   &returns: The resulting expression vector.
 */
struct rvec_expr_t *rvec_expr_mul(struct rmat_expr_t *mat, struct rvec_expr_t *vec)
{
	assert(mat->width == vec->len);

	unsigned int i, j;
	struct rvec_expr_t *res;
	struct r_list_t *list;

	res = rvec_expr_new(mat->height);

	for(i = 0; i < res->len; i++) {
		list = r_list_new();

		for(j = 0; j < mat->width; j++)
			r_list_add(&list, r_expr_mul(r_expr_copy(*rmat_expr_get(mat, i, j)), r_expr_copy(vec->arr[j])));

		r_expr_set(&res->arr[i], r_expr_sum(list));
	}

	return res;
}


/**
 * Create a variable vector.
 *   &returns: The variable vector.
 */
struct rvec_var_t *rvec_var_new(void)
{
	struct rvec_var_t *var;

	var = malloc(sizeof(struct rvec_var_t));
	var->arr = malloc(0);
	var->len = 0;

	return var;
}

/**
 * Delete a variable vector.
 *   @var: The variable vector.
 */
void rvec_var_delete(struct rvec_var_t *var)
{
	unsigned int i;

	for(i = 0; i < var->len; i++)
		r_var_delete(var->arr[i]);

	free(var->arr);
	free(var);
}


/**
 * Retrieve the index of an element.
 *   @var: The variable vector.
 *   @elem: The element.
 *   &returns: The index if found, negative otherwise.
 */
int rvec_var_idx(struct rvec_var_t *var, struct r_var_t *elem)
{
	unsigned int i;

	for(i = 0; i < var->len; i++) {
		if(var->arr[i] == elem)
			return i;
	}

	return -1;
}

/**
 * Add an element to the variable vector.
 *   @var: The variable vector.
 *   @elem: Consumed. The element.
 */
void rvec_var_add(struct rvec_var_t *var, struct r_var_t *elem)
{
	var->arr = realloc(var->arr, (var->len + 1) * sizeof(void *));
	var->arr[var->len++] = elem;
}
