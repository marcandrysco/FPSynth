#ifndef LANG_H
#define LANG_H

/**
 * Function structure.
 *   @in, tmp, out, st: The number of inputs, temporaries, outputs, and
 *     states.
 *   @let, ret, next: The set of let, return, and next expressions.
 */
struct fl_func_t {
	unsigned int in, tmp, out, st;
	struct fl_expr_t **let, **ret, **next;
};

/*
 * function declarations
 */
struct fl_func_t;

struct fl_func_t *fl_func_new(unsigned int in, unsigned int out, unsigned int st);
struct fl_func_t *fl_func_copy(const struct fl_func_t *func);
void fl_func_delete(struct fl_func_t *func);

void fl_func_tmp(struct fl_func_t *func, struct fl_expr_t *expr);

void fl_func_dump(struct fl_func_t *func);
void fl_func_print(struct fl_func_t *func, struct io_file_t file);
struct io_chunk_t fl_func_chunk(const struct fl_func_t *func);

unsigned int fl_func_size(struct fl_func_t *func);
unsigned int fl_func_nterms(struct fl_func_t *func);
uint64_t fl_func_hash(struct fl_func_t *func);
struct fl_expr_t **fl_func_rand(struct fl_func_t *func, unsigned int *tmp, struct m_rand_t *rand);

int fl_func_cmp(const struct fl_func_t *left, const struct fl_func_t *right);

void fl_func_eval(struct fl_func_t *func, const double *in, double *out, double *st);

/*
 * expression declarations
 */
enum fl_expr_e;
union fl_expr_u;
struct fl_expr_t;

struct fl_expr_t *fl_expr_new(enum fl_expr_e type, union fl_expr_u data);
struct fl_expr_t *fl_expr_copy(const struct fl_expr_t *expr);
void fl_expr_delete(struct fl_expr_t *expr);

struct fl_expr_t *fl_expr_in(unsigned int id);
struct fl_expr_t *fl_expr_var(unsigned int id);
struct fl_expr_t *fl_expr_st(unsigned int id);
struct fl_expr_t *fl_expr_flt(double flt);
struct fl_expr_t *fl_expr_add(struct fl_expr_t *left, struct fl_expr_t *right);
struct fl_expr_t *fl_expr_sub(struct fl_expr_t *left, struct fl_expr_t *right);
struct fl_expr_t *fl_expr_mul(struct fl_expr_t *left, struct fl_expr_t *right);
struct fl_expr_t *fl_expr_div(struct fl_expr_t *left, struct fl_expr_t *right);

void fl_expr_print(struct fl_expr_t *expr, struct io_file_t file);
struct io_chunk_t fl_expr_chunk(const struct fl_expr_t *expr);

unsigned int fl_expr_size(struct fl_expr_t *expr);
unsigned int fl_expr_nterms(struct fl_expr_t *expr);
uint64_t fl_expr_hash(struct fl_expr_t *expr);
struct fl_expr_t **fl_expr_byidx(struct fl_expr_t **expr, unsigned int *idx);

int fl_expr_cmp(const struct fl_expr_t *left, const struct fl_expr_t *right);
int fl_expr_compare(const void *left, const void *right);

double fl_expr_eval(struct fl_expr_t *expr, const double *in, const double *var, const double *st);

/**
 * Set an expression.
 *   @dest: Ref. The destination.
 *   @src: The source.
 */
static inline void fl_expr_set(struct fl_expr_t **dest, struct fl_expr_t *src)
{
	fl_expr_delete(*dest);
	*dest = src;
}

#endif
