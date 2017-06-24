#ifndef REAL_FRAC_H
#define REAL_FRAC_H

/*
 * fraction declarations
 */
struct r_frac_t;

struct r_frac_t *r_frac_new(void);
struct r_frac_t *r_frac_int(int val);
struct r_frac_t *r_frac_flt(double flt);
void r_frac_delete(struct r_frac_t *frac);

struct r_frac_t *r_frac_add(const struct r_frac_t *left, const struct r_frac_t *right);
struct r_frac_t *r_frac_add_clr(struct r_frac_t *left, struct r_frac_t *right);

struct r_frac_t *r_frac_sub(const struct r_frac_t *left, const struct r_frac_t *right);
struct r_frac_t *r_frac_sub_clr(struct r_frac_t *left, struct r_frac_t *right);

struct r_frac_t *r_frac_mul(const struct r_frac_t *left, const struct r_frac_t *right);
struct r_frac_t *r_frac_mul_clr(struct r_frac_t *left, struct r_frac_t *right);

struct r_frac_t *r_frac_div(const struct r_frac_t *left, const struct r_frac_t *right);
struct r_frac_t *r_frac_div_clr(struct r_frac_t *left, struct r_frac_t *right);

#endif
