#include "common.h"


/**
 * Generate the data for circuit one.
 */
void dat_cir1(void)
{
	struct r_sys_t *sys;
	struct rvec_var_t *var;
	struct cir_list_t *list;

	chkabort(cir_parse_list("dat/cir1.netlist", &list));

	sys = cir_system(list->node);
	r_sys_norm(sys);

	var = rvec_gather_sys(sys);

	r_sys_print(sys, io_file_wrap(stdout));

	{
		struct rmat_expr_t *mat, *inv;
		struct rvec_expr_t *vec, *res;

		vec = rvec_expr_new(var->len);
		mat = rmat_expr_new(var->len, var->len);

		struct r_sys_t *iter;
		unsigned int i, j;

		for(iter = sys, j = 0; iter != NULL; iter = iter->next, j++) {
			assert(r_expr_is_zero(iter->rel->right));

			for(i = 0; i < var->len; i++)
				r_expr_set(rmat_expr_get(mat, j, i), r_fold_expr_clr(r_deriv_expr(iter->rel->left, var->arr[i])));

			r_expr_set(&vec->arr[j], r_fold_expr_clr(r_expr_neg(r_const_expr(iter->rel->left))));
		}

		rvec_expr_dump(vec);
		rmat_expr_dump(mat);

		inv = rmat_fold_expr_clr(rmat_expr_inv(mat));

		rvec_var_dump(var);
		rmat_expr_dump(inv);
		res = rvec_fold_expr_clr(rvec_expr_mul(inv, vec));

		for(i = 0; i < res->len; i++)
			printf("%s = %C\n", var->arr[i]->id, r_expr_chunk(res->arr[i]));

		rvec_expr_delete(vec);
		rmat_expr_delete(mat);
		rmat_expr_delete(inv);
		rvec_expr_delete(res);
	}

	rvec_var_delete(var);
	r_sys_delete(sys);
	cir_list_delete(list);
}
