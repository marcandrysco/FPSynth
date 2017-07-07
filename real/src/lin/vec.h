#ifndef LIN_VEC_H
#define LIN_VEC_H

/**
 * Expression vector structure.
 *   @arr: The internal array.
 *   @len: The length.
 */
struct rvec_expr_t {
	struct r_expr_t **arr;
	unsigned int len;
};

struct rvec_expr_t *rvec_expr_new(unsigned int len);
void rvec_expr_delete(struct rvec_expr_t *vec);

void rvec_expr_dump(struct rvec_expr_t *vec);

struct rvec_expr_t *rvec_expr_mul(struct rmat_expr_t *mat, struct rvec_expr_t *vec);


/**
 * Variable vector structure.
 *   @arr: The internal array.
 *   @len: The length.
 */
struct rvec_var_t {
	struct r_var_t **arr;
	unsigned int len;
};

/*
 * variable vector declarations
 */
struct rvec_var_t *rvec_var_new(void);
void rvec_var_delete(struct rvec_var_t *var);

int rvec_var_idx(struct rvec_var_t *var, const char *id);
void rvec_var_add(struct rvec_var_t *var, struct r_var_t *elem);

void rvec_var_dump(struct rvec_var_t *var);


/*
 * compound vector declarations
 */
struct rvec_var_t *rvec_gather_sys(struct r_sys_t *sys);

#endif
