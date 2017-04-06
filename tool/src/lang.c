#include "common.h"


/**
 * Printing type enumerator.
 *   @fl_std_v: Standard printer.
 *   @fl_ml_v: ML-style.
 *   @fl_c_v: C code.
 */
enum fl_print_e {
	fl_std_v,
	fl_ml_v,
	fl_c_v
};

/*
 * local declarations
 */
static void func_chunk(struct io_file_t file, void *arg);

static void expr_chunk(struct io_file_t file, void *arg);


/**
 * Create a blank function.
 *   @in: The number of inputs.
 *   @out: The number of outputs.
 *   @st: The number of states.
 *   &returns: The function.
 */
struct fl_func_t *fl_func_new(unsigned int in, unsigned int out, unsigned int st)
{
	unsigned int i;
	struct fl_func_t *func;

	func = malloc(sizeof(struct fl_func_t));
	func->in = in;
	func->tmp = 0;
	func->out = out;
	func->st = st;

	func->let = malloc(0);
	func->ret = malloc(out * sizeof(void *));
	func->next = malloc(st * sizeof(void *));

	for(i = 0; i < out; i++)
		func->ret[i] = fl_expr_flt(0.0);

	for(i = 0; i < st; i++)
		func->next[i] = fl_expr_flt(0.0);

	return func;
}

/**
 * Copy a function.
 *   @func: The original function.
 *   &returns: Copied function.
 */
struct fl_func_t *fl_func_copy(const struct fl_func_t *func)
{
	unsigned int i;
	struct fl_func_t *copy;

	copy = malloc(sizeof(struct fl_func_t));
	copy->in = func->in;
	copy->tmp = func->tmp;
	copy->out = func->out;
	copy->st = func->st;

	copy->let = malloc(func->tmp * sizeof(void *));
	copy->ret = malloc(func->out * sizeof(void *));
	copy->next = malloc(func->st * sizeof(void *));

	for(i = 0; i < func->tmp; i++)
		copy->let[i] = fl_expr_copy(func->let[i]);

	for(i = 0; i < func->out; i++)
		copy->ret[i] = fl_expr_copy(func->ret[i]);

	for(i = 0; i < func->st; i++)
		copy->next[i] = fl_expr_copy(func->next[i]);

	return copy;
}

/**
 * Delete a function.
 *   @func: The function.
 */
void fl_func_delete(struct fl_func_t *func)
{
	unsigned int i;

	for(i = 0; i < func->tmp; i++)
		fl_expr_delete(func->let[i]);

	for(i = 0; i < func->out; i++)
		fl_expr_delete(func->ret[i]);

	for(i = 0; i < func->st; i++)
		fl_expr_delete(func->next[i]);

	free(func->let);
	free(func->ret);
	free(func->next);
	free(func);
}


/**
 * Add a temporary to the function.
 *   @func: The function.
 *   @expr: The temporary expression.
 */
void fl_func_tmp(struct fl_func_t *func, struct fl_expr_t *expr)
{
	func->let = realloc(func->let, (func->tmp + 1) * sizeof(void *));
	func->let[func->tmp++] = expr;
}


/**
 * Dump a function to standard out.
 *   @func: The function.
 */
void fl_func_dump(struct fl_func_t *func)
{
	fl_func_print(func, io_file_wrap(stdout));
}

/**
 * Print a function.
 *   @func: The funcion.
 *   @file: The file.
 */
void fl_func_print(struct fl_func_t *func, struct io_file_t file)
{
	unsigned int i;

	for(i = 0; i < func->tmp; i++)
		hprintf(file, "let $%u = %C\n", i, fl_expr_chunk(func->let[i]));

	for(i = 0; i < func->out; i++)
		hprintf(file, "ret %%%u = %C\n", i, fl_expr_chunk(func->ret[i]));

	for(i = 0; i < func->st; i++)
		hprintf(file, "st @%u = %C\n", i, fl_expr_chunk(func->next[i]));
}

/**
 * Create a chunk for a funtcion.
 *   @func: The function.
 *   &returns: The chunk.
 */
struct io_chunk_t fl_func_chunk(const struct fl_func_t *func)
{
	return (struct io_chunk_t){ func_chunk, (void *)func };
}
static void func_chunk(struct io_file_t file, void *arg)
{
	fl_func_print(arg, file);
}


/**
 * Compute the size of a function.
 *   @func: The function.
 *   &returns: The size.
 */
unsigned int fl_func_size(struct fl_func_t *func)
{
	unsigned int i, size = 0;

	for(i = 0; i < func->tmp; i++)
		size += fl_expr_size(func->let[i]);

	for(i = 0; i < func->out; i++)
		size += fl_expr_size(func->ret[i]);

	for(i = 0; i < func->st; i++)
		size += fl_expr_size(func->next[i]);

	return size;
}

/**
 * Compute the number of terminals of a function.
 *   @func: The function.
 *   &returns: The number of terminals.
 */
unsigned int fl_func_nterms(struct fl_func_t *func)
{
	unsigned int i, nterms = 0;

	for(i = 0; i < func->tmp; i++)
		nterms += fl_expr_nterms(func->let[i]);

	for(i = 0; i < func->out; i++)
		nterms += fl_expr_nterms(func->ret[i]);

	for(i = 0; i < func->st; i++)
		nterms += fl_expr_nterms(func->next[i]);

	return nterms;
}

/**
 * Compute a hash for the function.
 *   @func: The function.
 *   &returns: The hash.
 */
uint64_t fl_func_hash(struct fl_func_t *func)
{
	uint64_t hash = 0;
	unsigned int i;

	for(i = 0; i < func->tmp; i++)
		hash = mash64(hash, fl_expr_hash(func->let[i]));

	for(i = 0; i < func->out; i++)
		hash = mash64(hash, fl_expr_hash(func->ret[i]));

	for(i = 0; i < func->st; i++)
		hash = mash64(hash, fl_expr_hash(func->next[i]));

	return hash;
}

/**
 * Retrieve a random terminal expression.
 *   @func: The function.
 *   @tmp: Ref. The maximum available temporary.
 *   @rand: Optional. The random number generator.
 *   &returns: The expression reference.
 */
struct fl_expr_t **fl_func_rand(struct fl_func_t *func, unsigned int *tmp, struct m_rand_t *rand)
{
	struct fl_expr_t **expr;
	unsigned int i, rnd;

	rnd = m_rand_u32(rand) % fl_func_nterms(func);

	*tmp = 0;
	for(i = 0; i < func->tmp; i++) {
		*tmp = i;
		expr = fl_expr_byidx(&func->let[i], &rnd);
		if(expr != NULL)
			return expr;
	}

	for(i = 0; i < func->out; i++) {
		expr = fl_expr_byidx(&func->ret[i], &rnd);
		if(expr != NULL)
			return expr;
	}

	for(i = 0; i < func->st; i++) {
		expr = fl_expr_byidx(&func->next[i], &rnd);
		if(expr != NULL)
			return expr;
	}

	__builtin_unreachable();
}


/**
 * Compare two functions.
 *   @left: The left function.
 *   @right: The right function.
 *   &returns: Their order.
 */
int fl_func_cmp(const struct fl_func_t *left, const struct fl_func_t *right)
{
	int cmp;
	unsigned int i;

	if(left->tmp < right->tmp)
		return -1;
	else if(left->tmp > right->tmp)
		return 1;
	else if(left->out < right->out)
		return -1;
	else if(left->out > right->out)
		return 1;
	else if(left->st < right->st)
		return -1;
	else if(left->st > right->st)
		return 1;

	for(i = 0; i < left->tmp; i++) {
		cmp = fl_expr_cmp(left->let[i], right->let[i]);
		if(cmp != 0)
			return cmp;
	}

	for(i = 0; i < left->out; i++) {
		cmp = fl_expr_cmp(left->ret[i], right->ret[i]);
		if(cmp != 0)
			return cmp;
	}

	for(i = 0; i < left->st; i++) {
		cmp = fl_expr_cmp(left->next[i], right->next[i]);
		if(cmp != 0)
			return cmp;
	}

	return 0;
}


/**
 * Evaluate a function.
 *   @func: The function.
 *   @in: The input.
 *   @out: The output.
 *   @st: The state.
 */
void fl_func_eval(struct fl_func_t *func, const double *in, double *out, double *st)
{
	unsigned int i;
	double var[func->tmp];

	for(i = 0; i < func->tmp; i++)
		var[i] = fl_expr_eval(func->let[i], in, var, st);

	for(i = 0; i < func->out; i++)
		out[i] = fl_expr_eval(func->ret[i], in, var, st);

	for(i = 0; i < func->st; i++)
		st[i] = fl_expr_eval(func->next[i], in, var, st);
}


/**
 * Two-operand structure.
 *   @left, right: The left and right operands.
 */
struct fl_op2_t {
	struct fl_expr_t *left, *right;
};


/**
 * Expression type enumerator.
 *   @fl_in_v: Input.
 *   @fl_var_v: Variable.
 *   @fl_st_v: State.
 *   @fl_flt_v: Constant float.
 *   @fl_add_v: Addition.
 *   @fl_sub_v: Subtraction.
 *   @fl_mul_v: Multiplication.
 *   @fl_div_v: Division.
 */
enum fl_expr_e {
	fl_in_v,
	fl_var_v,
	fl_st_v,
	fl_flt_v,
	fl_add_v,
	fl_sub_v,
	fl_mul_v,
	fl_div_v
};

/**
 * Expression data union.
 *   @id: Identifier.
 *   @flt: Floating-point value.
 *   @op1: One-operand.
 *   @op2: Two-operands.
 */
union fl_expr_u {
	unsigned int id;
	double flt;
	struct fl_expr_t *op1;
	struct fl_op2_t op2;
};

/**
 * Expression structure.
 *   @type: The type.
 *   @data: The data.
 */
struct fl_expr_t {
	enum fl_expr_e type;
	union fl_expr_u data;
};


/**
 * Create a new expression.
 *   @type: The type.
 *   @data: The data.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_new(enum fl_expr_e type, union fl_expr_u data)
{
	struct fl_expr_t *expr;

	expr = malloc(sizeof(struct fl_expr_t));
	expr->type = type;
	expr->data = data;

	return expr;
}

/**
 * Copy an expression.
 *   @expr: The original expression.
 *   &returns: Copied expression.
 */
struct fl_expr_t *fl_expr_copy(const struct fl_expr_t *expr)
{
	switch(expr->type) {
	case fl_in_v: return fl_expr_in(expr->data.id);
	case fl_var_v: return fl_expr_var(expr->data.id);
	case fl_st_v: return fl_expr_st(expr->data.id);
	case fl_flt_v: return fl_expr_flt(expr->data.flt);
	case fl_add_v: return fl_expr_add(fl_expr_copy(expr->data.op2.left), fl_expr_copy(expr->data.op2.right));
	case fl_sub_v: return fl_expr_sub(fl_expr_copy(expr->data.op2.left), fl_expr_copy(expr->data.op2.right));
	case fl_mul_v: return fl_expr_mul(fl_expr_copy(expr->data.op2.left), fl_expr_copy(expr->data.op2.right));
	case fl_div_v: return fl_expr_div(fl_expr_copy(expr->data.op2.left), fl_expr_copy(expr->data.op2.right));
	}

	__builtin_unreachable();
}

/**
 * Delete an expression.
 *   @expr: The expression.
 */
void fl_expr_delete(struct fl_expr_t *expr)
{
	switch(expr->type) {
	case fl_in_v:
	case fl_var_v:
	case fl_st_v:
	case fl_flt_v:
		break;

	case fl_add_v:
	case fl_sub_v:
	case fl_mul_v:
	case fl_div_v:
		fl_expr_delete(expr->data.op2.left);
		fl_expr_delete(expr->data.op2.right);
		break;
	}

	free(expr);
}


/**
 * Create a input expression.
 *   @id: The identifier.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_in(unsigned int id)
{
	return fl_expr_new(fl_in_v, (union fl_expr_u){ .id = id });
}

/**
 * Create a variable expression.
 *   @id: The identifier.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_var(unsigned int id)
{
	return fl_expr_new(fl_var_v, (union fl_expr_u){ .id = id });
}

/**
 * Create a state expression.
 *   @id: The identifier.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_st(unsigned int id)
{
	return fl_expr_new(fl_st_v, (union fl_expr_u){ .id = id });
}

/**
 * Create a constant floating-point expression.
 *   @flt: The float-point value.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_flt(double flt)
{
	return fl_expr_new(fl_flt_v, (union fl_expr_u){ .flt = flt });
}

/**
 * Create an addition floating-point expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_add(struct fl_expr_t *left, struct fl_expr_t *right)
{
	return fl_expr_new(fl_add_v, (union fl_expr_u){ .op2 = { left, right } });
}

/**
 * Create a subtraction floating-point expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_sub(struct fl_expr_t *left, struct fl_expr_t *right)
{
	return fl_expr_new(fl_sub_v, (union fl_expr_u){ .op2 = { left, right } });
}

/**
 * Create a multiplication floating-point expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_mul(struct fl_expr_t *left, struct fl_expr_t *right)
{
	return fl_expr_new(fl_mul_v, (union fl_expr_u){ .op2 = { left, right } });
}

/**
 * Create a division floating-point expression.
 *   @left: Consumed. The left expression.
 *   @right: Consumed. The right expression.
 *   &returns: The expression.
 */
struct fl_expr_t *fl_expr_div(struct fl_expr_t *left, struct fl_expr_t *right)
{
	return fl_expr_new(fl_div_v, (union fl_expr_u){ .op2 = { left, right } });
}


/**
 * Print an expression.
 *   @expr: The expression.
 *   @file: The file.
 */
void fl_expr_print(struct fl_expr_t *expr, struct io_file_t file)
{
	switch(expr->type) {
	case fl_in_v:
		hprintf(file, "&%u", expr->data.id);
		break;

	case fl_var_v:
		hprintf(file, "$%u", expr->data.id);
		break;

	case fl_st_v:
		hprintf(file, "@%u", expr->data.id);
		break;

	case fl_flt_v:
		hprintf(file, "%g", expr->data.flt);
		break;

	case fl_add_v:
		hprintf(file, "+(%C,%C)", fl_expr_chunk(expr->data.op2.left), fl_expr_chunk(expr->data.op2.right));
		break;

	case fl_sub_v:
		hprintf(file, "-(%C,%C)", fl_expr_chunk(expr->data.op2.left), fl_expr_chunk(expr->data.op2.right));
		break;

	case fl_mul_v:
		hprintf(file, "*(%C,%C)", fl_expr_chunk(expr->data.op2.left), fl_expr_chunk(expr->data.op2.right));
		break;

	case fl_div_v:
		hprintf(file, "/(%C,%C)", fl_expr_chunk(expr->data.op2.left), fl_expr_chunk(expr->data.op2.right));
		break;
	}
}

/**
 * Create a chunk for an expression.
 *   @expr: The expression.
 *   &returns: The chunk.
 */
struct io_chunk_t fl_expr_chunk(const struct fl_expr_t *expr)
{
	return (struct io_chunk_t){ expr_chunk, (void *)expr };
}
static void expr_chunk(struct io_file_t file, void *arg)
{
	fl_expr_print(arg, file);
}


/**
 * Compute the size of an expression.
 *   @expr: The expression.
 *   &returns: The size.
 */
unsigned int fl_expr_size(struct fl_expr_t *expr)
{
	switch(expr->type) {
	case fl_in_v:
	case fl_var_v:
	case fl_st_v:
	case fl_flt_v:
		return 1;

	case fl_add_v:
	case fl_sub_v:
	case fl_mul_v:
	case fl_div_v:
		return fl_expr_size(expr->data.op2.left) + fl_expr_size(expr->data.op2.right) + 1;
	}

	__builtin_unreachable();
}

/**
 * Compute the number of terminals in an expression.
 *   @expr: The expression.
 *   &returns: The number of terminal.
 */
unsigned int fl_expr_nterms(struct fl_expr_t *expr)
{
	switch(expr->type) {
	case fl_in_v:
	case fl_var_v:
	case fl_st_v:
	case fl_flt_v:
		return 1;

	case fl_add_v:
	case fl_sub_v:
	case fl_mul_v:
	case fl_div_v:
		return fl_expr_nterms(expr->data.op2.left) + fl_expr_nterms(expr->data.op2.right);
	}

	__builtin_unreachable();
}

/**
 * Compute a hash for the expression.
 *   @expr: The expression.
 *   &returns: The hash.
 */
uint64_t fl_expr_hash(struct fl_expr_t *expr)
{
	uint64_t hash = expr->type;

	switch(expr->type) {
	case fl_in_v:
	case fl_var_v:
	case fl_st_v:
		hash = mash64(hash, expr->data.id);
		break;

	case fl_flt_v:
		mash64buf(&hash, &expr->data.flt, sizeof(double));

		break;

	case fl_add_v:
	case fl_sub_v:
	case fl_mul_v:
	case fl_div_v:
		hash = mash64(hash, mash64(fl_expr_hash(expr->data.op2.left), fl_expr_hash(expr->data.op2.right)));

		break;
	}

	return hash;
}


/**
 * Retrieve a terminal by index.
 *   @expr: The expression reference.
 *   @idx: Ref. The index.
 *   &returns: The expression or null.
 */
struct fl_expr_t **fl_expr_byidx(struct fl_expr_t **expr, unsigned int *idx)
{
	switch((*expr)->type) {
	case fl_in_v:
	case fl_var_v:
	case fl_st_v:
	case fl_flt_v:
		if((*idx)-- == 0)
			return expr;
		else
			return NULL;

	case fl_add_v:
	case fl_sub_v:
	case fl_mul_v:
	case fl_div_v:
		{
			struct fl_expr_t **ret;

			ret = fl_expr_byidx(&(*expr)->data.op2.left, idx);
			if(ret == NULL)
				ret = fl_expr_byidx(&(*expr)->data.op2.right, idx);

			return ret;
		}
	}

	__builtin_unreachable();
}


/**
 * Compare two expressions.
 *   @left: The left expression.
 *   @right: The right expression.
 *   &returns: Their order.
 */
int fl_expr_cmp(const struct fl_expr_t *left, const struct fl_expr_t *right)
{
	if(left->type != right->type)
		return left->type - right->type;

	switch(left->type) {
	case fl_in_v:
	case fl_var_v:
	case fl_st_v:
		if(left->data.id > right->data.id)
			return -1;
		else if(left->data.id < right->data.id)
			return 1;
		else
			return 0;

	case fl_flt_v:
		if(left->data.flt > right->data.flt)
			return -1;
		else if(left->data.flt < right->data.flt)
			return 1;
		else
			return 0;

	case fl_add_v:
	case fl_sub_v:
	case fl_mul_v:
	case fl_div_v:
		{
			int cmp;

			cmp = fl_expr_cmp(left->data.op2.left, right->data.op2.left);
			if(cmp == 0)
				cmp = fl_expr_cmp(left->data.op2.right, right->data.op2.right);

			return cmp;
		}
	}

	return 0;
}

/**
 * Compare two expression pointers.
 *   @left: The left expression pointer.
 *   @right: The right expression pointer.
 *   &returns: Their order.
 */
int fl_expr_compare(const void *left, const void *right)
{
	return fl_expr_cmp(left, right);
}


/**
 * Evaluate an expression.
 *   @expr: The expression.
 *   @in: The input values.
 *   @var: The variable values.
 *   @st: The state values.
 *   &returns: The value.
 */
double fl_expr_eval(struct fl_expr_t *expr, const double *in, const double *var, const double *st)
{
	switch(expr->type) {
	case fl_in_v:
		return in[expr->data.id];

	case fl_var_v:
		return var[expr->data.id];

	case fl_st_v:
		return st[expr->data.id];

	case fl_flt_v:
		return expr->data.flt;

	case fl_add_v:
		return fl_expr_eval(expr->data.op2.left, in, var, st) + fl_expr_eval(expr->data.op2.right, in, var, st);

	case fl_sub_v:
		return fl_expr_eval(expr->data.op2.left, in, var, st) - fl_expr_eval(expr->data.op2.right, in, var, st);

	case fl_mul_v:
		return fl_expr_eval(expr->data.op2.left, in, var, st) * fl_expr_eval(expr->data.op2.right, in, var, st);

	case fl_div_v:
		return fl_expr_eval(expr->data.op2.left, in, var, st) / fl_expr_eval(expr->data.op2.right, in, var, st);
	}

	__builtin_unreachable();
}
