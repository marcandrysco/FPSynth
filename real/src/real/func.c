#include "../common.h"


/*
 * local declarations
 */
static void func_proc(struct io_file_t file, void *arg);


/**
 * Create a new function.
 *   @expr: Consumed. The expression body.
 *   @param: Consumed. The paramater list.
 *   &returns: The function.
 */
struct r_func_t *r_func_new(struct r_expr_t *expr, struct r_param_t *param)
{
	struct r_func_t *func;

	func = malloc(sizeof(struct r_func_t));
	func->param = param;
	func->expr = expr;

	return func;
}

/**
 * Copy a function.
 *   @func: The function.
 *   &returns: The copy.
 */
struct r_func_t *r_func_copy(const struct r_func_t *func)
{
	return r_func_new(r_expr_copy(func->expr), r_param_copy(func->param));
}

/**
 * Delete a function.
 *   @func: The function.
 */
void r_func_delete(struct r_func_t *func)
{
	struct r_param_t *param;

	for(param = func->param; param != NULL; param = func->param) {
		func->param = param->next;
		r_param_delete(param);
	}

	r_expr_delete(func->expr);
	free(func);
}


/**
 * Print a function.
 *   @func: The function.
 *   @file: The file.
 */
void r_func_print(const struct r_func_t *func, struct io_file_t file)
{
	struct r_param_t *param;

	for(param = func->param; param != NULL; param = param->next)
		hprintf(file, "%s%s", (param != func->param) ? "," : "", param->var->id);

	hprintf(file, " -> %C", r_expr_chunk(func->expr));
}

/**
 * Retrieve a chunk for a function.
 *   @func: The function.
 *   &returns: The chunk.
 */
struct io_chunk_t r_func_chunk(const struct r_func_t *func)
{
	return (struct io_chunk_t){ func_proc, (void *)func };
}
static void func_proc(struct io_file_t file, void *arg)
{
	r_func_print(arg, file);
}


/**
 * Create a new parameter.
 *   @var: Consumed. The variable.
 *   &returns: The parameter.
 */
struct r_param_t *r_param_new(struct r_var_t *var)
{
	struct r_param_t *param;

	param = malloc(sizeof(struct r_param_t));
	param->var = var;
	param->next = NULL;

	return param;
}

/**
 * Copy a parameter list.
 *   @param: The original parameter list.
 *   &returns: The copied parameter list.
 */
struct r_param_t *r_param_copy(const struct r_param_t *param)
{
	struct r_param_t *head, **cur = &head;

	while(param != NULL) {
		*cur = malloc(sizeof(struct r_param_t));
		(*cur)->var = r_var_copy(param->var);
		cur = &(*cur)->next;
	}

	*cur = NULL;

	return head;
}

/**
 * Delete a parameter.
 *   @param: The parameter.
 */
void r_param_delete(struct r_param_t *param)
{
	r_var_delete(param->var);
	free(param);
}


/**
 * Add a parameter.
 *   @param: The list pointer.
 *   @var: The varirable to add.
 */
void r_param_append(struct r_param_t **param, struct r_var_t *var)
{
	while(*param != NULL)
		param = &(*param)->next;

	*param = r_param_new(var);
}


/**
 * Create a new variable.
 *   @id: The identifier.
 *   &returns: The variable.
 */
struct r_var_t *r_var_new(char *id)
{
	struct r_var_t *var;

	var = malloc(sizeof(struct r_var_t));
	var->id = id;
	var->refcnt = 1;

	return var;
}

/**
 * Copy a variable.
 *   @var: The original variable.
 *   &returns: The copied variable.
 */
struct r_var_t *r_var_copy(struct r_var_t *var)
{
	var->refcnt++;

	return var;
}

/**
 * Delete a variable.
 *   @var: The variable.
 */
void r_var_delete(struct r_var_t *var)
{
	if(var->refcnt-- > 1)
		return;

	free(var->id);
	free(var);
}
