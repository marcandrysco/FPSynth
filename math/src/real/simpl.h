#ifndef REAL_SIMPLE_H
#define REAL_SIMPLE_H

/*
 * folding declarations
 */
struct r_expr_t *r_fold_expr(struct r_expr_t *expr);
struct r_expr_t *r_fold_expr_clr(struct r_expr_t *expr);

struct rvec_expr_t *rvec_fold_expr_clr(struct rvec_expr_t *vec);
struct rmat_expr_t *rmat_fold_expr_clr(struct rmat_expr_t *mat);

#endif
