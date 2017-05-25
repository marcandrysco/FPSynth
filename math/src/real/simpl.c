#include "../common.h"


/**
 * Fold constant values in an expression.
 *   @expr: The expression.
 *   &returns: The constant.
 */
struct r_expr_t *r_fold_expr(struct r_expr_t *expr)
{
	return r_fold_expr_clr(r_expr_copy(expr));
}

/**
 * Fold constant values in an expression, clearing the input.
 *   @expr: Consumed The expression.
 *   &returns: The constant.
 */
struct r_expr_t *r_fold_expr_clr(struct r_expr_t *expr)
{
	switch(expr->type) {
	case r_unk_v:
	case r_flt_v:
	case r_num_v:
	case r_const_v:
	case r_var_v:
		return expr;

	case r_neg_v:
		expr->data.expr = r_fold_expr_clr(expr->data.expr);

		if(expr->data.expr->type == r_flt_v)
			r_expr_set(&expr, r_expr_flt(-expr->data.expr->data.flt));
		else if(expr->data.expr->type == r_neg_v)
			r_expr_set(&expr, r_expr_copy(expr->data.expr->data.expr));

		return expr;
	
	case r_add_v:
	case r_sub_v:
	case r_mul_v:
	case r_div_v:
		expr->data.op2.left = r_fold_expr_clr(expr->data.op2.left);
		expr->data.op2.right = r_fold_expr_clr(expr->data.op2.right);

		switch(expr->type) {
		case r_add_v:
			if((expr->data.op2.left->type == r_flt_v) && (expr->data.op2.right->type == r_flt_v))
				r_expr_set(&expr, r_expr_flt(expr->data.op2.left->data.flt + expr->data.op2.right->data.flt));
			else if(r_expr_is_zero(expr->data.op2.left))
				r_expr_set(&expr, r_expr_copy(expr->data.op2.right));
			else if(r_expr_is_zero(expr->data.op2.right))
				r_expr_set(&expr, r_expr_copy(expr->data.op2.left));

			break;
		
		case r_sub_v:
			if((expr->data.op2.left->type == r_flt_v) && (expr->data.op2.right->type == r_flt_v))
				r_expr_set(&expr, r_expr_flt(expr->data.op2.left->data.flt - expr->data.op2.right->data.flt));
			else if(r_expr_is_zero(expr->data.op2.left)) {
				if(expr->data.op2.right->type == r_flt_v)
					r_expr_set(&expr, r_expr_flt(-expr->data.op2.right->data.flt));
				else if(expr->data.op2.right->type == r_neg_v)
					r_expr_set(&expr, r_expr_copy(expr->data.op2.right->data.expr));
				else
					r_expr_set(&expr, r_expr_neg(r_expr_copy(expr->data.op2.right)));
			}
			else if(r_expr_is_zero(expr->data.op2.right))
				r_expr_set(&expr, r_expr_copy(expr->data.op2.left));

			break;
		
		case r_mul_v:
			if((expr->data.op2.left->type == r_flt_v) && (expr->data.op2.right->type == r_flt_v))
				r_expr_set(&expr, r_expr_flt(expr->data.op2.left->data.flt * expr->data.op2.right->data.flt));
			else if(r_expr_is_zero(expr->data.op2.left) || r_expr_is_zero(expr->data.op2.right))
				r_expr_set(&expr, r_expr_zero());
			else if(r_expr_is_one(expr->data.op2.left))
				r_expr_set(&expr, r_expr_copy(expr->data.op2.right));
			else if(r_expr_is_one(expr->data.op2.right))
				r_expr_set(&expr, r_expr_copy(expr->data.op2.left));

			break;
		
		case r_div_v:
			if((expr->data.op2.left->type == r_flt_v) && (expr->data.op2.right->type == r_flt_v))
				r_expr_set(&expr, r_expr_flt(expr->data.op2.left->data.flt / expr->data.op2.right->data.flt));
			else if(r_expr_is_one(expr->data.op2.right))
				r_expr_set(&expr, r_expr_copy(expr->data.op2.left));

			break;

		default:
			__builtin_unreachable();
		}
	
		return expr;

	case r_sum_v:
		{
			double flt = 0.0;
			struct r_list_t **iter = &expr->data.list;

			while(*iter != NULL) {
				(*iter)->expr = r_fold_expr_clr((*iter)->expr);

				if((*iter)->expr->type == r_flt_v) {
					flt += (*iter)->expr->data.flt;
					r_list_purge(iter);
				}
				else
					iter = &(*iter)->next;
			}

			if(flt == 0.0) {
				if(expr->data.list == NULL)
					r_expr_set(&expr, r_expr_zero());
				else if(expr->data.list->next == NULL)
					r_expr_set(&expr, r_expr_copy(expr->data.list->expr));
			}
			else if(expr->data.list == NULL)
				r_expr_set(&expr, r_expr_flt(flt));
			else
				r_list_add(iter, r_expr_flt(flt));

			return expr;
		}
	}

	__builtin_unreachable();
}


/**
 * Constant fold over an expression vector, clearing the input.
 *   @vec: Consumed. The expression vector.
 *   &returns: The foled expression vector.
 */
struct rvec_expr_t *rvec_fold_expr_clr(struct rvec_expr_t *vec)
{
	unsigned int i;

	for(i = 0; i < vec->len; i++)
		vec->arr[i] = r_fold_expr_clr(vec->arr[i]);

	return vec;
}

/**
 * Constant fold over an expression matrix, clearing the input.
 *   @mat: Consumed. The expression matrix.
 *   &returns: The foled expression matrix.
 */
struct rmat_expr_t *rmat_fold_expr_clr(struct rmat_expr_t *mat)
{
	unsigned int i, len;

	len = mat->width * mat->height;

	for(i = 0; i < len; i++)
		mat->arr[i] = r_fold_expr_clr(mat->arr[i]);

	return mat;
}
