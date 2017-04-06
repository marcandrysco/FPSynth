#include "common.h"



/**
 * Normalize weights.
 *   @weight: The weights.
 */
void fl_weight_norm(struct fl_weight_t *weight)
{
	weight->div += weight->mul += weight->sub += weight->add;
	weight->add /= weight->div;
	weight->sub /= weight->div;
	weight->mul /= weight->div;
	weight->div /= weight->div;
}


/**
 * Create a new generator.
 *   &returns: The generator.
 */
struct fl_gen_t *fl_gen_new(void)
{
	struct fl_gen_t *gen;

	gen = malloc(sizeof(struct fl_gen_t));
	gen->tree = avltree_init((compare_f)fl_inst_cmp, delete_noop);
	gen->val = malloc(2 * sizeof(double));
	gen->val[0] = 0.0;
	gen->val[1] = 1.0;
	gen->nvals = 2;
	gen->arr = malloc(0);
	gen->len = 0;

	return gen;
}

/**
 * Delete a generator.
 *   @gen: The generator.
 */
void fl_gen_delete(struct fl_gen_t *gen)
{
	unsigned int i;

	for(i = 0; i < gen->len; i++)
		fl_inst_delete(gen->arr[i]);

	avltree_destroy(&gen->tree);
	free(gen->val);
	free(gen->arr);
	free(gen);
}


/**
 * Check if the generator already contains the given instance.
 *   @gen: The generator.
 *   @inst: The instance.
 *   &returns: True if already found.
 */
bool fl_gen_find(struct fl_gen_t *gen, struct fl_inst_t *inst)
{
	return avltree_lookup(&gen->tree, inst) != NULL;
	unsigned int i;

	for(i = 0; i < gen->len; i++) {
		if(fl_inst_cmp(gen->arr[i], inst) == 0)
			return true;
	}

	return false;
}

/**
 * Add an instance to the generator.
 *   @gen: The generator.
 *   @inst: The instance.
 */
void fl_gen_add(struct fl_gen_t *gen, struct fl_inst_t *inst)
{
	gen->arr = realloc(gen->arr, (gen->len + 1) * sizeof(void *));
	gen->arr[gen->len++] = inst;
	avltree_insert(&gen->tree, inst, inst);
}

/**
 * Add a constant to the generator.
 *   @gen: The generator.
 *   @val: The value.
 */
void fl_gen_const(struct fl_gen_t *gen, double val)
{
	unsigned int i;

	for(i = 0; i < gen->nvals; i++) {
		if(gen->val[i] == val)
			return;
	}

	gen->val = realloc(gen->val, (gen->nvals + 1) * sizeof(double));
	gen->val[gen->nvals++] = val;
}


struct fl_expr_t *fl_gen_expr(struct fl_func_t *func, unsigned int tmp, struct m_rand_t *rand)
{
	unsigned int v;

	v = m_rand_u32(rand) % (tmp + func->in + func->st);
	if(v < tmp)
		return fl_expr_var(v);
	else
		v -= tmp;

	if(v < func->in)
		return fl_expr_in(v);
	else
		v -= func->in;

	if(v < func->st)
		return fl_expr_st(v);
	else
		v -= func->st;

	__builtin_unreachable();
}

/**
 * Trial a random modification.
 *   @gen: The generator.
 *   @weight: The weights to apply.
 *   @rand: Optional. The random number generator.
 *   &returns: The instance if unique, null on duplicate.
 */
struct fl_inst_t *fl_gen_trial(struct fl_gen_t *gen, const struct fl_weight_t *weight, struct m_rand_t *rand)
{
	unsigned int tmp;
	struct fl_inst_t *inst;
	struct fl_func_t *func;
	struct fl_expr_t **expr;

	func = fl_func_copy(gen->arr[m_rand_u32(rand) % gen->len]->func);
	if(m_rand_d(rand) < 0.1) {
		fl_func_tmp(func, fl_expr_flt(0.0));
	}
	else {
		expr = fl_func_rand(func, &tmp, rand);

		if(m_rand_d(rand) < 0.2) {
			fl_expr_set(expr, fl_expr_flt(gen->val[m_rand_u32(rand) % gen->nvals]));
		}
		else if(m_rand_d(rand) < 0.5) {
			fl_expr_set(expr, fl_gen_expr(func, tmp, rand));
		}
		else {
			float v = m_rand_d(rand);
			struct fl_expr_t *(*op)(struct fl_expr_t *, struct fl_expr_t *);

			if(v < weight->add)
				op = fl_expr_add;
			else if(v < weight->sub)
				op = fl_expr_sub;
			else if(v < weight->mul)
				op = fl_expr_mul;
			else
				op = fl_expr_div;

			fl_expr_set(expr, (*op)(fl_gen_expr(func, tmp, rand), fl_gen_expr(func, tmp, rand)));
		}
	}

	inst = fl_inst_new(func);

	if(fl_gen_find(gen, inst)) {
		fl_inst_delete(inst);
		return NULL;
	}
	else {
		fl_gen_add(gen, inst);
		return inst;
	}
}


/**
 * Create an instance.
 *   @func: Consumed. The function.
 *   &returns: The instance.
 */
struct fl_inst_t *fl_inst_new(struct fl_func_t *func)
{
	struct fl_inst_t *inst;

	inst = malloc(sizeof(struct fl_inst_t));
	inst->hash = fl_func_hash(func);
	inst->size = fl_func_size(func);
	inst->nterms = fl_func_nterms(func);
	inst->func = func;

	return inst;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */
void fl_inst_delete(struct fl_inst_t *inst)
{
	fl_func_delete(inst->func);
	free(inst);
}


/**
 * Efficiently compare two instances.
 *   @left: The left instance.
 *   @right: The right instance.
 *   &returns: Their order.
 */
int fl_inst_cmp(struct fl_inst_t *left, struct fl_inst_t *right)
{
	if(left->hash < right->hash)
		return -40;
	else if(left->hash > right->hash)
		return 40;
	else if(left->size < right->size)
		return -30;
	else if(left->size > right->size)
		return 30;
	else if(left->nterms < right->nterms)
		return -20;
	else if(left->nterms > right->nterms)
		return 20;
	else
		return fl_func_cmp(left->func, right->func);
}
