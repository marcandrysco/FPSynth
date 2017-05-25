#ifndef REAL_EXPR_H
#define REAL_EXPR_H

/**
 * Integer structure.
 *   @mpz: The multi-precision integer.
 */
struct r_num_t {
	mpz_t mpz;
};

/**
 * Two operand structure.
 *   @left, right: The left and right expressions.
 */
struct r_op2_t {
	struct r_expr_t *left, *right;
};

/**
 * Real expression enumerator.
 *   @r_unk_v: Unknown.
 *   @r_flt_v: Floating-point number.
 *   @r_num_v: Integer number.
 *   @r_const_v: Constant.
 *   @r_var_v: Variable.
 *   @r_neg_v: Negation.
 *   @r_add_v: Addition.
 *   @r_sub_v: Subtraction.
 *   @r_mul_v: Multiplication.
 *   @r_div_v: Division.
 *   @r_sum_v: Sum a list.
 */
enum r_expr_e {
	r_unk_v,
	r_flt_v,
	r_num_v,
	r_const_v,
	r_var_v,
	r_neg_v,
	r_add_v,
	r_sub_v,
	r_mul_v,
	r_div_v,
	r_sum_v,
};

/**
 * Real data union.
 *   @flt: Double-precision, floating-point number.
 *   @name: Constant name.
 *   @num: Integer number.
 *   @var: Variable.
 *   @op2: Two-opreand expression.
 *   @expr: Single expression.
 *   @list: List of expressions.
 */
union r_expr_u {
	double flt;
	char *name;
	struct r_num_t *num;
	struct r_var_t *var;
	struct r_op2_t op2;
	struct r_expr_t *expr;
	struct r_list_t *list;
};

/**
 * Real expression structure.
 *   @type: The type.
 *   @data: The data.
 */
struct r_expr_t {
	enum r_expr_e type;
	union r_expr_u data;
};

/**
 * List structure.
 *   @expr: The expression.
 *   @next: The next node in the list.
 */
struct r_list_t {
	struct r_expr_t *expr;
	struct r_list_t *next;
};


/*
 * expression declarations
 */
struct r_expr_t *r_expr_new(enum r_expr_e type, union r_expr_u data);
struct r_expr_t *r_expr_copy(struct r_expr_t *expr);
void r_expr_delete(struct r_expr_t *expr);

struct r_expr_t *r_expr_unk(void);
struct r_expr_t *r_expr_flt(double flt);
struct r_expr_t *r_expr_num(struct r_num_t *num);
struct r_expr_t *r_expr_const(char *name);
struct r_expr_t *r_expr_var(struct r_var_t *var);
struct r_expr_t *r_expr_single(enum r_expr_e type, struct r_expr_t *expr);
struct r_expr_t *r_expr_list(enum r_expr_e type, struct r_list_t *list);

struct r_expr_t *r_expr_zero(void);
struct r_expr_t *r_expr_one(void);
struct r_expr_t *r_expr_neg(struct r_expr_t *expr);
struct r_expr_t *r_expr_sub(struct r_expr_t *left, struct r_expr_t *right);
struct r_expr_t *r_expr_add(struct r_expr_t *left, struct r_expr_t *right);
struct r_expr_t *r_expr_mul(struct r_expr_t *left, struct r_expr_t *right);
struct r_expr_t *r_expr_div(struct r_expr_t *left, struct r_expr_t *right);
struct r_expr_t *r_expr_sum(struct r_list_t *list);

bool r_expr_is_zero(struct r_expr_t *expr);
bool r_expr_is_one(struct r_expr_t *expr);
bool r_expr_has_unk(struct r_expr_t *expr);

void r_expr_print(const struct r_expr_t *expr, struct io_file_t file);
struct io_chunk_t r_expr_chunk(const struct r_expr_t *expr);


/**
 * Swapt two expressions.
 *   @left: Ref. The left expression.
 *   @right: Ref. The right expression.
 */
static inline void r_expr_swap(struct r_expr_t **left, struct r_expr_t **right)
{
	memswap(left, right, sizeof(struct r_expr_t *));
}

/**
 * Set an expression at the destination.
 *   @dest: Ref. The destination expression reference.
 *   @src: The source expression reference.
 */
static inline void r_expr_set(struct r_expr_t **dest, struct r_expr_t *src)
{
	r_expr_delete(*dest);
	*dest = src;
}

/**
 * Replace a possibly null expression at the destination.
 *   @dest: Ref. The destination expression reference.
 *   @src: The source expression reference.
 */
static inline void r_expr_replace(struct r_expr_t **dest, struct r_expr_t *src)
{
	if(*dest != NULL)
		r_expr_delete(*dest);

	*dest = src;
}


/*
 * number declarations
 */
struct r_num_t *r_num_new(int val);
struct r_num_t *r_num_copy(struct r_num_t *num);
void r_num_delete(struct r_num_t *num);

void r_num_print(const struct r_num_t *num, struct io_file_t file);
struct io_chunk_t r_num_chunk(const struct r_num_t *num);

/*
 * list declarations
 */
struct r_list_t *r_list_new(void);
struct r_list_t *r_list_copy(const struct r_list_t *list);
void r_list_delete(struct r_list_t *list);

struct r_list_t **r_list_add(struct r_list_t **list, struct r_expr_t *expr);
struct r_expr_t *r_list_remove(struct r_list_t **list);
void r_list_purge(struct r_list_t **list);

bool r_list_has_unk(struct r_list_t *list);

#endif
