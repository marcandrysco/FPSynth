#include "../common.h"


/**
 * Create a new environment.
 *   &returns: The environment.
 */
struct r_env_t *r_env_new(void)
{
	return NULL;
}

/**
 * Delete an environment.
 *   @env: The environment.
 */
void r_env_delete(struct r_env_t *env)
{
	struct r_env_t *tmp;

	while(env != NULL) {
		tmp = env;
		env = tmp->next;

		free(tmp->id);
		free(tmp);
	}
}


/**
 * Get a value from the environment.
 *   @env: The environemnt.
 *   @id: The identifier.
 *   &returns: The matched environment.
 */
struct r_env_t *r_env_get(struct r_env_t *env, const char *id)
{
	while(env != NULL) {
		if(strcmp(env->id, id) == 0)
			break;

		env = env->next;
	}

	return env;
}

/**
 * Put a value into the environment.
 *   @env: Ref. The environment.
 *   @id: The identifier.
 *   @flt: The floating-point value.
 */
void r_env_put(struct r_env_t **env, const char *id, double flt)
{
	while(*env != NULL) {
		if(strcmp((*env)->id, id) == 0)
			break;

		env = &(*env)->next;
	}

	if(*env == NULL) {
		*env = malloc(sizeof(struct r_env_t));
		(*env)->id = strdup(id);
		(*env)->next = NULL;
	}
	
	(*env)->flt = flt;
}


/**
 * Evaluate an expression.
 *   @expr: The expression.
 *   @env: The environment.
 *   @res: The result.
 *   &returns: Error.
 */
char *r_eval_expr(struct r_expr_t *expr, struct r_env_t *env, double *res)
{
	switch(expr->type) {
	case r_unk_v:
		*res = NAN;
		break;

	case r_flt_v:
		*res = expr->data.flt;
		break;

	case r_num_v:
		*res = NAN;
		break;

	case r_const_v:
		{
			struct r_env_t *get;
		
			get = r_env_get(env, expr->data.name);
			if(get == NULL)
				return mprintf("Unknown constant '%s'.", expr->data.name);

			*res = get->flt;
		}
		break;

	case r_var_v:
		{
			struct r_env_t *get;
		
			get = r_env_get(env, expr->data.var->id);
			if(get == NULL)
				return mprintf("Unknown variable '%s'.", expr->data.var->id);

			*res = get->flt;
		}
		break;

	case r_neg_v:
		chkret(r_eval_expr(expr->data.expr, env, res));
		break;

	case r_add_v:
	case r_sub_v:
	case r_mul_v:
	case r_div_v:
		{
			double left, right;

			chkret(r_eval_expr(expr->data.op2.left, env, &left));
			chkret(r_eval_expr(expr->data.op2.right, env, &right));

			switch(expr->type) {
			case r_add_v: *res = left + right; break;
			case r_sub_v: *res = left - right; break;
			case r_mul_v: *res = left * right; break;
			case r_div_v: *res = left / right; break;
			default: __builtin_unreachable();
			}
		}
		break;

	case r_sum_v:
		{
			double tmp;
			struct r_list_t *list;

			*res = 0.0;
			for(list = expr->data.list; list != NULL; list = list->next) {
				chkret(r_eval_expr(list->expr, env, &tmp));
				*res += tmp;
			}
		}
		break;
	}

	return NULL;
}
