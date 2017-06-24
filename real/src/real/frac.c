#include "../common.h"


/**
 * Fractional real number.
 *   @mpq: GMP rational.
 */
struct r_frac_t {
	mpq_t mpq;
};


/**
 * Create a new fractional real number.
 *   &returns: The fraction.
 */
struct r_frac_t *r_frac_new(void)
{
	struct r_frac_t *frac;

	frac = malloc(sizeof(struct r_frac_t));
	mpq_init(frac->mpq);
	mpq_set_ui(frac->mpq, 0, 1);

	return frac;
}

/**
 * Create a fractional real number from an integer.
 *   @val: The initial value.
 *   &returns: The fraction.
 */
struct r_frac_t *r_frac_int(int val)
{
	struct r_frac_t *frac;

	frac = malloc(sizeof(struct r_frac_t));
	mpq_init(frac->mpq);
	mpq_set_si(frac->mpq, val, 1);

	return frac;
}

/**
 * Create a fractional real number from a double.
 *   @val: The initial value.
 *   &returns: The fraction.
 */
struct r_frac_t *r_frac_flt(double flt)
{
	struct r_frac_t *frac;

	frac = malloc(sizeof(struct r_frac_t));
	mpq_init(frac->mpq);
	mpq_set_d(frac->mpq, flt);

	return frac;
}

/**
 * Delete a fractional real number.
 *   @frac: The fraction.
 */
void r_frac_delete(struct r_frac_t *frac)
{
	mpq_clear(frac->mpq);
	free(frac);
}


/**
 * Add fractional real numbers.
 *   @left: The left fraction.
 *   @right: The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_add(const struct r_frac_t *left, const struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_new();
	mpq_add(res->mpq, left->mpq, right->mpq);

	return res;
}

/**
 * Add and clear fractional real numbers.
 *   @left: Consumed. The left fraction.
 *   @right: Consumed. The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_add_clr(struct r_frac_t *left, struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_add(left, right);
	r_frac_delete(left);
	r_frac_delete(right);

	return res;
}


/**
 * Subtract fractional real numbers.
 *   @left: The left fraction.
 *   @right: The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_sub(const struct r_frac_t *left, const struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_new();
	mpq_sub(res->mpq, left->mpq, right->mpq);

	return res;
}

/**
 * Subtract and clear fractional real numbers.
 *   @left: Consumed. The left fraction.
 *   @right: Consumed. The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_sub_clr(struct r_frac_t *left, struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_sub(left, right);
	r_frac_delete(left);
	r_frac_delete(right);

	return res;
}


/**
 * Multiply fractional real numbers.
 *   @left: The left fraction.
 *   @right: The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_mul(const struct r_frac_t *left, const struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_new();
	mpq_mul(res->mpq, left->mpq, right->mpq);

	return res;
}

/**
 * Multiply and clear fractional real numbers.
 *   @left: Consumed. The left fraction.
 *   @right: Consumed. The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_mul_clr(struct r_frac_t *left, struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_mul(left, right);
	r_frac_delete(left);
	r_frac_delete(right);

	return res;
}


/**
 * Divide fractional real numbers.
 *   @left: The left fraction.
 *   @right: The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_div(const struct r_frac_t *left, const struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_new();
	mpq_div(res->mpq, left->mpq, right->mpq);

	return res;
}

/**
 * Divide and clear fractional real numbers.
 *   @left: Consumed. The left fraction.
 *   @right: Consumed. The right fraction.
 *   &returns: The result fraction.
 */
struct r_frac_t *r_frac_div_clr(struct r_frac_t *left, struct r_frac_t *right)
{
	struct r_frac_t *res;

	res = r_frac_div(left, right);
	r_frac_delete(left);
	r_frac_delete(right);

	return res;
}
