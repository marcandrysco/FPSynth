#ifndef REAL_FUNC_H
#define REAL_FUNC_H

/**
 * Variable structure.
 *   @id: The identifier.
 *   @refcnt: The reference count.
 */
struct r_var_t {
	char *id;
	unsigned int refcnt;
};

/**
 * Function structure.
 *   @param: The parameter list.
 *   @expr: The function body.
 */
struct r_func_t {
	struct r_param_t *param;
	struct r_expr_t *expr;
};

/**
 * Parameter structure.
 *   @var: The variable.
 *   @next: The next parameters.
 */
struct r_param_t {
	struct r_var_t *var;
	struct r_param_t *next;
};


/*
 * function declarations
 */
struct r_func_t *r_func_new(struct r_expr_t *expr, struct r_param_t *param);
struct r_func_t *r_func_copy(const struct r_func_t *func);
void r_func_delete(struct r_func_t *func);

void r_func_print(const struct r_func_t *func, struct io_file_t file);
struct io_chunk_t r_func_chunk(const struct r_func_t *func);

/*
 * parameter declarations
 */
struct r_param_t *r_param_new(struct r_var_t *var);
struct r_param_t *r_param_copy(const struct r_param_t *param);
void r_param_delete(struct r_param_t *param);

void r_param_append(struct r_param_t **param, struct r_var_t *var);

/*
 * variable declarations
 */
struct r_var_t *r_var_new(char *id);
struct r_var_t *r_var_copy(struct r_var_t *var);
void r_var_delete(struct r_var_t *var);

#endif
