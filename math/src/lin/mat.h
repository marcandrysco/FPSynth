#ifndef LIN_MAT_H
#define LIN_MAT_H

/**
 * Expression matrix structure.
 *   @width, height: The width and height.
 *   @arr: The expression array.
 */
struct rmat_expr_t {
	unsigned int width, height;
	struct r_expr_t **arr;
};

/*
 * expression matrix declarations
 */
struct rmat_expr_t *rmat_expr_new(unsigned int width, unsigned int height);
void rmat_expr_delete(struct rmat_expr_t *mat);

void rmat_expr_dump(struct rmat_expr_t *mat);

struct r_expr_t **rmat_expr_get(struct rmat_expr_t *mat, unsigned int row, unsigned int col);

bool rmat_expr_is_zero(struct rmat_expr_t *mat);
bool rmat_expr_is_ident(struct rmat_expr_t *mat);

struct r_expr_t *rmat_expr_det(struct rmat_expr_t *mat);
struct r_expr_t *rmat_expr_det_clr(struct rmat_expr_t *mat);

struct rmat_expr_t *rmat_expr_inv(struct rmat_expr_t *mat);

struct rmat_expr_t *rmat_expr_exclude(struct rmat_expr_t *mat, unsigned int row, unsigned int col);


/**
 * Float matrix structure.
 *   @width, height: The width and height.
 *   @arr: The expression array.
 */
struct rmat_flt_t {
	unsigned int w, h;
	double *arr;
};

/*
 * float matrix declarations
 */
struct rmat_expr_t *rmat_expr_new(unsigned int w, unsigned int h);
void rmat_expr_delete(struct rmat_expr_t *mat);

#endif
