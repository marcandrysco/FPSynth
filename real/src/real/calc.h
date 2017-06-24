#ifndef REAL_CALC_H
#define REAL_CALC_H

/*
 * calculus declarations
 */
struct r_expr_t *r_deriv_expr(struct r_expr_t *expr, struct r_var_t *var);

struct r_expr_t *r_const_expr(struct r_expr_t *expr);

#endif
