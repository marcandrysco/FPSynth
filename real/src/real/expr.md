# Expressions

Real expressions are built using this expressions API. By composing `struct
r_expr_t`, arbitrary expressions may be built using any of the available
operators.

## Expression

All expression are stored in a tagged union data structure `struct r_expr_t`
consisting only of a type enumerator and a data union.

    struct r_expr_t {
      enum r_expr_e type;
      union r_expr_u data;
    };

    enum r_expr_e {
      r_unk_v, r_var_v,                             // variables
      r_flt_v, r_num_v, r_const_v,                  // constants
      r_neg_v,                                      // unary operators
      r_add_v, r_sub_v, r_mul_v, r_div_v,           // binary operators
      r_sum_v                                       // list operators
    };

    union r_expr_u {
      double flt;
      char *name;
      struct r_num_t *num;
      struct r_var_t *var;
      struct r_op2_t op2;
      struct r_expr_t *expr;
      struct r_list_t *list;
    };

The enumerator `r_expr_e` defines the type of expression that falls into a few
broad categories: a concrete value, a variable, or an operation. All possible
expression types are outlinese in the followed table:

	* `r_unk_v`: An unknown variable. It represents a single use
    variable that is not shared with any other expression.
	* `r_flt_v`: A floating-point value. It is a constant, double-precision
    floating-point number.
	* `r_num_v`: A big integer value.
	* `r_const_v`: A named constant. It represents a constant that is not
    necessarily expressible as a number, such as pi.
	* `r_var_v`: A variable. The referenced variable structure is reference
    counted so that it may be used multiple times in an expression.
	* `r_neg_v`: The negation operator on `expr`.
	* `r_add_v`: The addition operator on `op2`.
	* `r_sub_v`: The subtraction operator on `op2`.
	* `r_mul_v`: The multiplication operator on `op2`.
	* `r_div_v`: The division operator on `op2`.
	* `r_sum_v`: The addition operatoro on a list of expressions `list`.

The data union defines all possible data to be stored by the expression. 
