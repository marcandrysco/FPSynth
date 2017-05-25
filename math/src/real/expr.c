#include "../common.h"


/*
 * local declarations
 */
static void expr_proc(struct io_file_t file, void *arg);
static void num_proc(struct io_file_t file, void *arg);


/**
 * Create a expression.
 *   @type: The type.
 *   @data: Consumed. The data.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_new(enum r_expr_e type, union r_expr_u data)
{
	struct r_expr_t *expr;

	expr = malloc(sizeof(struct r_expr_t));
	*expr = (struct r_expr_t){ type, data };

	return expr;
}

/**
 * Copy an expression.
 *   @expr: The original expression.
 *   &returns: The copied expression.
 */
struct r_expr_t *r_expr_copy(struct r_expr_t *expr)
{
	switch(expr->type) {
	case r_unk_v: return r_expr_unk();
	case r_flt_v: return r_expr_flt(expr->data.flt);
	case r_num_v: return r_expr_num(r_num_copy(expr->data.num));
	case r_var_v: return r_expr_var(r_var_copy(expr->data.var));
	case r_const_v: return r_expr_const(strdup(expr->data.name));
	case r_neg_v: return r_expr_neg(r_expr_copy(expr->data.expr));
	case r_add_v: return r_expr_add(r_expr_copy(expr->data.op2.left), r_expr_copy(expr->data.op2.right));
	case r_sub_v: return r_expr_sub(r_expr_copy(expr->data.op2.left), r_expr_copy(expr->data.op2.right));
	case r_mul_v: return r_expr_mul(r_expr_copy(expr->data.op2.left), r_expr_copy(expr->data.op2.right));
	case r_div_v: return r_expr_div(r_expr_copy(expr->data.op2.left), r_expr_copy(expr->data.op2.right));
	case r_sum_v: return r_expr_sum(r_list_copy(expr->data.list));
	}

	fatal("Invalid real expression type.");
}

/**
 * Delete an expression.
 *   @expr: The expression.
 */
void r_expr_delete(struct r_expr_t *expr)
{
	switch(expr->type) {
	case r_unk_v:
	case r_flt_v:
		break;

	case r_num_v:
		r_num_delete(expr->data.num);
		break;

	case r_const_v:
		free(expr->data.name);
		break;

	case r_var_v:
		r_var_delete(expr->data.var);
		break;

	case r_neg_v:
		r_expr_delete(expr->data.expr);
		break;

	case r_add_v:
	case r_sub_v:
	case r_mul_v:
	case r_div_v:
		r_expr_delete(expr->data.op2.left);
		r_expr_delete(expr->data.op2.right);
		break;

	case r_sum_v:
		r_list_delete(expr->data.list);
		break;
	}

	free(expr);
}


/**
 * Create a unknown expression
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_unk(void)
{
	return r_expr_new(r_unk_v, (union r_expr_u){ });
}

/**
 * Create a floating-point expression.
 *   @flt: The floating-point value.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_flt(double flt)
{
	return r_expr_new(r_flt_v, (union r_expr_u){ .flt = flt });
}

/**
 * Create a number expression.
 *   @num: The number.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_num(struct r_num_t *num)
{
	return r_expr_new(r_num_v, (union r_expr_u){ .num = num });
}

/**
 * Create a constant expression.
 *   @name: Consumed. The name.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_const(char *name)
{
	return r_expr_new(r_const_v, (union r_expr_u){ .name = name });
}

/**
 * Create a variable expression.
 *   @var: Consumed. The variable.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_var(struct r_var_t *var)
{
	return r_expr_new(r_var_v, (union r_expr_u){ .var = var });
}

/**
 * Create a single expression.
 *   @type: The type.
 *   @list: The list.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_single(enum r_expr_e type, struct r_expr_t *expr)
{
	return r_expr_new(type, (union r_expr_u){ .expr = expr});
}

/**
 * Create a list expression.
 *   @type: The type.
 *   @list: The list.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_list(enum r_expr_e type, struct r_list_t *list)
{
	return r_expr_new(type, (union r_expr_u){ .list = list });
}


/**
 * Create a zero expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_zero(void)
{
	return r_expr_flt(0.0);
}

/**
 * Create a one expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_one(void)
{
	return r_expr_flt(0.0);
}

/**
 * Create a negation expression.
 *   @expr: The internal expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_neg(struct r_expr_t *expr)
{
	return r_expr_single(r_neg_v, expr);
}

/**
 * Create an addition expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_add(struct r_expr_t *left, struct r_expr_t *right)
{
	return r_expr_new(r_add_v, (union r_expr_u){ .op2 = { left, right }});
}

/**
 * Create a subtraction expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_sub(struct r_expr_t *left, struct r_expr_t *right)
{
	return r_expr_new(r_sub_v, (union r_expr_u){ .op2 = { left, right }});
}

/**
 * Create a multiplication expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_mul(struct r_expr_t *left, struct r_expr_t *right)
{
	return r_expr_new(r_mul_v, (union r_expr_u){ .op2 = { left, right }});
}

/**
 * Create a division expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_div(struct r_expr_t *left, struct r_expr_t *right)
{
	return r_expr_new(r_div_v, (union r_expr_u){ .op2 = { left, right }});
}

/**
 * Create a sum expression.
 *   @list: The expression list.
 *   &returns: The expression.
 */
struct r_expr_t *r_expr_sum(struct r_list_t *list)
{
	return r_expr_list(r_sum_v, list);
}


/**
 * Check if an expression is zero.
 *   @expr: The expression.
 *   &returns: True if zero.
 */
bool r_expr_is_zero(struct r_expr_t *expr)
{
	return (expr->type == r_flt_v) && (expr->data.flt == 0.0);
}

/**
 * Check if an expression is one.
 *   @expr: The expression.
 *   &returns: True if one.
 */
bool r_expr_is_one(struct r_expr_t *expr)
{
	return (expr->type == r_flt_v) && (expr->data.flt == 0.0);
}

/**
 * Check if an expression has an unknown.
 *   @expr: The expression.
 *   &returns: True if has unknown.
 */
bool r_expr_has_unk(struct r_expr_t *expr)
{
	switch(expr->type) {
	case r_unk_v:
		return true;

	case r_flt_v:
	case r_num_v:
	case r_const_v:
	case r_var_v:
		return false;

	case r_neg_v:
		return r_expr_has_unk(expr->data.expr);

	case r_add_v:
	case r_sub_v:
	case r_mul_v:
	case r_div_v:
		return r_expr_has_unk(expr->data.op2.left) || r_expr_has_unk(expr->data.op2.right);

	case r_sum_v:
		return r_list_has_unk(expr->data.list);
	}

	__builtin_unreachable();
}


/**
 * Print an expression.
 *   @expr: The expression.
 *   @file: The file.
 */
void r_expr_print(const struct r_expr_t *expr, struct io_file_t file)
{
	switch(expr->type) {
	case r_unk_v:
		hprintf(file, "_");
		break;

	case r_flt_v:
		hprintf(file, "%.2g", expr->data.flt);
		break;

	case r_num_v:
		r_num_print(expr->data.num, file);
		break;

	case r_const_v:
		hprintf(file, "%s", expr->data.name);
		break;

	case r_var_v:
		hprintf(file, "%s", expr->data.var->id);
		break;

	case r_neg_v:
		hprintf(file, "-(%C)", r_expr_chunk(expr->data.expr));
		break;

	case r_add_v: hprintf(file, "(%C+%C)", r_expr_chunk(expr->data.op2.left), r_expr_chunk(expr->data.op2.right)); break;
	case r_sub_v: hprintf(file, "(%C-%C)", r_expr_chunk(expr->data.op2.left), r_expr_chunk(expr->data.op2.right)); break;
	case r_mul_v: hprintf(file, "(%C*%C)", r_expr_chunk(expr->data.op2.left), r_expr_chunk(expr->data.op2.right)); break;
	case r_div_v: hprintf(file, "(%C/%C)", r_expr_chunk(expr->data.op2.left), r_expr_chunk(expr->data.op2.right)); break;

	case r_sum_v:
		{
			struct r_list_t *list;

			hprintf(file, "(");
			for(list = expr->data.list; list != NULL; list = list->next) {
				if(list != expr->data.list)
					hprintf(file, "+");

				hprintf(file, "%C", r_expr_chunk(list->expr));
			}
			hprintf(file, ")");
		}
		break;
	}
}

/**
 * Retrieve a chunk for a expression.
 *   @expr: The expression.
 *   &returns: The chunk.
 */
struct io_chunk_t r_expr_chunk(const struct r_expr_t *expr)
{
	return (struct io_chunk_t){ expr_proc, (void *)expr };
}
static void expr_proc(struct io_file_t file, void *arg)
{
	r_expr_print(arg, file);
}


/**
 * Create a new number.
 *   @val: The initial value.
 *   &returns: The number.
 */
struct r_num_t *r_num_new(int val)
{
	struct r_num_t *num;

	num = malloc(sizeof(struct r_num_t));
	mpz_init_set_si(num->mpz, val);

	return num;
}

/**
 * Copy a number.
 *   @num: The original number.
 *   &returns: The copied number.
 */
struct r_num_t *r_num_copy(struct r_num_t *num)
{
	struct r_num_t *copy;

	copy = malloc(sizeof(struct r_num_t));
	mpz_init_set(copy->mpz, num->mpz);

	return copy;
}

/**
 * Delete a number.
 *   @num: The number.
 */
void r_num_delete(struct r_num_t *num)
{
	mpz_clear(num->mpz);
	free(num);
}


/**
 * Print a number.
 *   @num: The number.
 *   @file: The file.
 */
void r_num_print(const struct r_num_t *num, struct io_file_t file)
{
	unsigned int len = gmp_snprintf(NULL, 0, "%Zd", num->mpz);
	char buf[len + 1];

	gmp_sprintf(buf, "%Zd", num->mpz);
	io_file_write(file, buf, len);
}

/**
 * Retrieve a chunk for a number.
 *   @num: The number.
 *   &returns: The chunk.
 */
struct io_chunk_t r_num_chunk(const struct r_num_t *num)
{
	return (struct io_chunk_t){ num_proc, (void *)num };
}
static void num_proc(struct io_file_t file, void *arg)
{
	r_num_print(arg, file);
}


/**
 * Create a new list.
 *   &returns: The empty list.
 */
struct r_list_t *r_list_new(void)
{
	return NULL;
}

/**
 * Copy a list.
 *   @list: The original list.
 *   &returns: The copied list.
 */
struct r_list_t *r_list_copy(const struct r_list_t *list)
{
	struct r_list_t *copy, **iter;

	copy = r_list_new();
	iter = &copy;

	while(list != NULL) {
		iter = r_list_add(iter, r_expr_copy(list->expr));
		list = list->next;
	}

	return copy;
}

/**
 * Delete a list.
 *   @list: The list.
 */
void r_list_delete(struct r_list_t *list)
{
	struct r_list_t *tmp;

	while(list != NULL) {
		tmp = list;
		list = tmp->next;

		r_expr_delete(tmp->expr);
		free(tmp);
	}
}


/**
 * Add an expression to a list.
 *   @list: The list reference.
 *   @expr: Consumed. The expression.
 *   &returns: The new list reference, not the head of the list.
 */
struct r_list_t **r_list_add(struct r_list_t **list, struct r_expr_t *expr)
{
	while(*list != NULL)
		list = &(*list)->next;

	*list = malloc(sizeof(struct r_list_t));
	(*list)->expr = expr;
	(*list)->next = NULL;

	return list;
}

/**
 * Remove an expression from the list.
 *   @list: The list.
 *   &returns: The removed expression.
 */
struct r_expr_t *r_list_remove(struct r_list_t **list)
{
	struct r_list_t *tmp;
	struct r_expr_t *expr;

	tmp = *list;
	expr = tmp->expr;
	*list = tmp->next;
	free(tmp);

	return expr;
}

/**
 * Purge an expression from the list.
 *   @list: The list.
 */
void r_list_purge(struct r_list_t **list)
{
	r_expr_delete(r_list_remove(list));
}


/**
 * Check if an list has an unknown.
 *   @expr: The expression.
 *   &returns: True if has unknown.
 */
bool r_list_has_unk(struct r_list_t *list)
{
	while(list != NULL) {
		if(r_expr_has_unk(list->expr))
			return true;

		list = list->next;
	} 

	return false;
}
