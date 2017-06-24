#ifndef REAL_EVAL_H
#define REAL_EVAL_H

/**
 * Environment structure.
 *   @id: The identifier.
 *   @flt: The floating-point value.
 *   @next: The next environment.
 */
struct r_env_t {
	char *id;
	double flt;

	struct r_env_t *next;
};

/*
 * environment declarations
 */
struct r_env_t *r_env_new(void);
void r_env_delete(struct r_env_t *env);

struct r_env_t *r_env_get(struct r_env_t *env, const char *id);
void r_env_put(struct r_env_t **env, const char *id, double flt);

/*
 * evaluator declarations
 */
char *r_eval_expr(struct r_expr_t *expr, struct r_env_t *env, double *res);

#endif
