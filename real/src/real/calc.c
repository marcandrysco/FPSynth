#include "../common.h"


/**
 * Compute the derivative of an expression.
 *   @expr: The expression.
 *   @var: The independent variable.
 *   &returns: The derivative.
 */
struct r_expr_t *r_deriv_expr(struct r_expr_t *expr, struct r_var_t *var)
{
	switch(expr->type) {
	case r_unk_v:
	case r_flt_v:
	case r_num_v:
	case r_const_v:
		return r_expr_zero();

	case r_var_v:
		return r_expr_flt((expr->data.var == var) ? 1.0 : 0.0);

	case r_neg_v:
		return r_expr_neg(r_deriv_expr(expr->data.expr, var));

	case r_add_v:
	case r_sub_v:
		{
			struct r_expr_t *left, *right;

			left = r_deriv_expr(expr->data.op2.left, var);
			right = r_deriv_expr(expr->data.op2.right, var);

			switch(expr->type) {
			case r_add_v: return r_expr_add(left, right);
			case r_sub_v: return r_expr_sub(left, right);
			default: __builtin_unreachable();
			}
		}

	case r_mul_v:
		{
			struct r_expr_t *left, *right, *res[2];

			left = expr->data.op2.left;
			right = expr->data.op2.right;

			res[0] = r_expr_mul(r_expr_copy(left), r_deriv_expr(right, var));
			res[1] = r_expr_mul(r_deriv_expr(left, var), r_expr_copy(right));

			return r_expr_add(res[0], res[1]);
		}

	case r_div_v:
		{
			struct r_expr_t *low, *high, *left, *right;

			low = expr->data.op2.left;
			high = expr->data.op2.right;

			left = r_expr_mul(r_expr_copy(low), r_deriv_expr(high, var));
			right = r_expr_mul(r_expr_copy(high), r_deriv_expr(low, var));

			return r_expr_div(r_expr_mul(left, right), r_expr_mul(r_expr_copy(low), r_expr_copy(low)));
		}

	case r_sum_v:
		{
			struct r_list_t *list, *res, **iter;

			res = r_list_new();
			iter = &res;

			for(list = expr->data.list; list != NULL; list = list->next)
				iter = r_list_add(iter, r_deriv_expr(list->expr, var));

			switch(expr->type) {
			case r_sum_v: return r_expr_sum(res);
			default: __builtin_unreachable();
			}
		}
	}

	__builtin_unreachable();
}

/**
 * Compute the constant from an expression.
 *   @expr: The expression.
 *   &returns: The constant.
 */
struct r_expr_t *r_const_expr(struct r_expr_t *expr)
{
	switch(expr->type) {
	case r_unk_v:
	case r_flt_v:
	case r_num_v:
	case r_const_v:
		return r_expr_copy(expr);

	case r_var_v:
		return r_expr_zero();

	case r_neg_v:
		return r_expr_neg(r_const_expr(expr->data.expr));

	case r_add_v:
	case r_sub_v:
	case r_mul_v:
	case r_div_v:
		{
			struct r_expr_t *left, *right;

			left = r_const_expr(expr->data.op2.left);
			right = r_const_expr(expr->data.op2.right);

			switch(expr->type) {
			case r_add_v: return r_expr_add(left, right);
			case r_sub_v: return r_expr_sub(left, right);
			case r_mul_v: return r_expr_mul(left, right);
			case r_div_v: return r_expr_div(left, right);
			default: __builtin_unreachable();
			}
		}

	case r_sum_v:
		{
			struct r_list_t *list, *res, **iter;

			res = r_list_new();
			iter = &res;

			for(list = expr->data.list; list != NULL; list = list->next)
				iter = r_list_add(iter, r_const_expr(list->expr));

			switch(expr->type) {
			case r_sum_v: return r_expr_sum(res);
			default: __builtin_unreachable();
			}
		}
	}

	__builtin_unreachable();
}
