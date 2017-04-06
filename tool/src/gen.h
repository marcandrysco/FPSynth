#ifndef GEN_H
#define GEN_H

/**
 * Generator weights.
 *   @add, sub, mul, div: The binary operator weights.
 */
struct fl_weight_t {
	float add, sub, mul, div;
};

/*
 * weight declarations
 */
void fl_weight_norm(struct fl_weight_t *weight);


/**
 * Generator structure.
 *   @tree: Fast lookup tree.
 *   @val: Constant value array.
 *   @nvals: The number of constants.
 *   @arr: The array.
 *   @len: The length.
 */
struct fl_gen_t {
	struct avltree_t tree;

	double *val;
	unsigned int nvals;

	struct fl_inst_t **arr;
	unsigned int len;
};

/*
 * generator declarations
 */
struct fl_gen_t;

struct fl_gen_t *fl_gen_new(void);
void fl_gen_delete(struct fl_gen_t *gen);

bool fl_gen_find(struct fl_gen_t *gen, struct fl_inst_t *inst);
void fl_gen_add(struct fl_gen_t *gen, struct fl_inst_t *inst);
void fl_gen_const(struct fl_gen_t *gen, double val);

struct fl_inst_t *fl_gen_trial(struct fl_gen_t *gen, const struct fl_weight_t *weight, struct m_rand_t *rand);


/**
 * Instance structure.
 *   @hash: The hash.
 *   @size, nterms: The size and number of terminals.
 *   @func: The function.
 */
struct fl_inst_t {
	uint64_t hash;
	unsigned int size, nterms;
	struct fl_func_t *func;
};

/*
 * instance declarations
 */
struct fl_inst_t;

struct fl_inst_t *fl_inst_new(struct fl_func_t *func);
void fl_inst_delete(struct fl_inst_t *inst);

int fl_inst_cmp(struct fl_inst_t *left, struct fl_inst_t *right);

#endif
