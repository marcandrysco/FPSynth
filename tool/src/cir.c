#include "common.h"


/**
 * Create a new node.
 *   @tag: The tag.
 *   @type: The type.
 *   @data: The data.
 *   @cnt: The number of ports.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_new(enum cir_node_e type, union cir_node_u data, unsigned int cnt)
{
	unsigned int i;
	struct cir_node_t *node;

	node = malloc(sizeof(struct cir_node_t));
	node->type = type;
	node->data = data;
	node->cnt = cnt;
	node->port = malloc(cnt * sizeof(struct cir_port_t));

	for(i = 0; i < cnt; i++) {
		node->port[i].node = node;
		node->port[i].wire = malloc(sizeof(struct cir_wire_t));
		node->port[i].wire->port = &node->port[i];
		node->port[i].next = NULL;
	}

	return node;
}

/**
 * Delete a node.
 *   @node: The node.
 */
void cir_node_delete(struct cir_node_t *node)
{
	unsigned int i;

	switch(node->type) {
	case cir_input_v:
	case cir_output_v:
		free(node->data.str);
		break;

	case cir_value_v:
	case cir_res_v:
	case cir_cap_v:
		break;
	}

	for(i = 0; i < node->cnt; i++) {
		cir_disconnect(&node->port[i]);
		free(node->port[i].wire);
	}

	free(node->port);
	free(node);
}

/**
 * Erase a node if not null.
 *   @node: The node.
 */
void cir_node_erase(struct cir_node_t *node)
{
	if(node != NULL)
		cir_node_delete(node);
}


/**
 * Create an input node.
 *   @tag: The tag.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_input(char *tag)
{
	return cir_node_new(cir_input_v, (union cir_node_u){ .str = tag }, 1);
}

/**
 * Create an output node.
 *   @tag: The tag.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_output(char *tag)
{
	return cir_node_new(cir_output_v, (union cir_node_u){ .str = tag }, 1);
}

/**
 * Create a ground node.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_gnd(void)
{
	return cir_node_value(0.0);
}

/**
 * Create a value node.
 *   @flt: The floating-point value.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_value(double flt)
{
	return cir_node_new(cir_value_v, (union cir_node_u){ .flt = flt }, 1);
}

/**
 * Create a resistance node.
 *   @flt: The floating-point resistance.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_res(double flt)
{
	return cir_node_new(cir_res_v, (union cir_node_u){ .flt = flt }, 2);
}

/**
 * Create a capacitor node.
 *   @flt: The floating-point resistance.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_cap(double flt)
{
	return cir_node_new(cir_cap_v, (union cir_node_u){ .flt = flt }, 2);
}


/**
 * Connect two ports together.
 *   @left: The left port.
 *   @right: The right port.
 */
void cir_connect(struct cir_port_t *left, struct cir_port_t *right)
{
	struct cir_port_t *cur;
	struct cir_wire_t *wire, *prev;

	wire = left->wire;
	prev = right->wire;

	while(right != NULL) {
		cur = right;
		right = cur->next;

		cur->wire = wire;
		cur->next = wire->port;
		wire->port = cur;
	}

	free(prev);
}

/**
 * Disconnect a port from a wire.
 *   @port: The port.
 */
void cir_disconnect(struct cir_port_t *port)
{
	struct cir_port_t **iter;

	if((port->wire->port == port) && (port->next == NULL))
		return;

	iter = &port->wire->port;
	while(*iter != port)
		iter = &(*iter)->next;

	*iter = (*iter)->next;

	port->wire = malloc(sizeof(struct cir_wire_t));
	port->wire->port = port;
	port->next = NULL;
}


/**
 * Generate a dot file for the circuit.
 *   @node: The node.
 *   @path: The path.
 */
void cir_dot(struct cir_node_t *node, const char *path)
{
}


/**
 * Compute a tree of all nodes.
 *   @node: The starting node.
 *   &returns: The tree.
 */
struct avltree_t cir_nodes(struct cir_node_t *node)
{
	struct avltree_t tree;

	tree = avltree_init(compare_ptr, free);
	cir_nodes_iter(node, &tree);

	return tree;
}

/**
 * Iterate over all nodes, adding to a tree.
 *   @node: The initial node.
 *   @tree: The tree.
 */
void cir_nodes_iter(struct cir_node_t *node, struct avltree_t *tree)
{
	double *val;
	unsigned int i;
	struct cir_port_t *port;

	if(avltree_lookup(tree, node) != NULL)
		return;

	val = malloc(sizeof(double));
	*val = 0.0;
	avltree_insert(tree, node, val);

	for(i = 0; i < node->cnt; i++) {
		for(port = node->port[i].wire->port; port != NULL; port = port->next)
			cir_nodes_iter(port->node, tree);
	}
}

/**
 * Compute a tree of all wires.
 *   @node: The starting node.
 *   &returns: The tree.
 */
struct avltree_t cir_wires(struct cir_node_t *node)
{
	unsigned int i;
	struct avltree_t tree;

	tree = avltree_init(compare_ptr, free);

	for(i = 0; i < node->cnt; i++)
		cir_wires_iter(node->port[i].wire, &tree);

	return tree;
}

/**
 * Iterate over all wires, adding to a tree.
 *   @wire: The initial wire.
 *   @tree: The tree.
 */
void cir_wires_iter(struct cir_wire_t *wire, struct avltree_t *tree)
{
	double *val;
	unsigned int i;
	struct cir_port_t *port;

	if(avltree_lookup(tree, wire) != NULL)
		return;

	val = malloc(sizeof(double));
	*val = 0.0;
	avltree_insert(tree, wire, val);

	for(port = wire->port; port != NULL; port = port->next) {
		for(i = 0; i < port->node->cnt; i++)
			cir_wires_iter(port->node->port[i].wire, tree);
	}
}


/**
 * Enumerate all nodes from a starting node.
 *   @node: The starting node.
 *   &returns: The node list.
 */
struct cir_node_t **cir_node_enum(struct cir_node_t *node)
{
	unsigned int cnt;
	struct cir_node_t **list;

	list = malloc(cnt = 0);

	cir_node_iter(node, &list, &cnt);

	list = realloc(list, (cnt + 1) * sizeof(void *));
	list[cnt] = NULL;

	return list;
}

/**
 * Iterate over all nodes.
 *   @node: The node.
 *   @list: Ref. The node list.
 *   @cnt: Ref. The list count.
 */
void cir_node_iter(struct cir_node_t *node, struct cir_node_t ***list, unsigned int *cnt)
{
	unsigned int i;
	struct cir_port_t *port;

	for(i = 0; i < *cnt; i++) {
		if((*list)[i] == node)
			return;
	}

	*list = realloc(*list, (*cnt + 1) * sizeof(void *));
	(*list)[(*cnt)++] = node;

	for(i = 0; i < node->cnt; i++) {
		for(port = node->port[i].wire->port; port != NULL; port = port->next)
			cir_node_iter(port->node, list,  cnt);
	}
}

/**
 * Compute the index of a node.
 *   @list: The list.
 *   @node: The node.
 *   &returns: The index if found, negative otherwise.
 */
int cir_node_idx(struct cir_node_t **list, struct cir_node_t *node)
{
	unsigned int i;

	for(i = 0; list[i] != NULL; i++) {
		if(list[i] == node)
			return i;
	}

	return -1;
}


/**
 * Enumerate all wires from a node.
 *   @node: The node.
 *   &returns: The wire list.
 */
struct cir_wire_t **cir_wire_enum(struct cir_node_t *node)
{
	unsigned int i, cnt;
	struct cir_wire_t **list;

	list = malloc(cnt = 0);

	for(i = 0; i < node->cnt; i++)
		cir_wire_iter(node->port[i].wire, &list, &cnt);

	list = realloc(list, (cnt + 1) * sizeof(void *));
	list[cnt] = NULL;

	return list;
}

/**
 * Iterate over all wires.
 *   @wire: The wire.
 *   @list: Ref. The wire list.
 *   @cnt: Ref. The list count.
 */
void cir_wire_iter(struct cir_wire_t *wire, struct cir_wire_t ***list, unsigned int *cnt)
{
	unsigned int i;
	struct cir_port_t *port;

	for(i = 0; i < *cnt; i++) {
		if((*list)[i] == wire)
			return;
	}

	*list = realloc(*list, (*cnt + 1) * sizeof(void *));
	(*list)[(*cnt)++] = wire;

	for(port = wire->port; port != NULL; port = port->next) {
		for(i = 0; i < port->node->cnt; i++)
			cir_wire_iter(port->node->port[i].wire, list, cnt);
	}
}

/**
 * Compute the index of a wire.
 *   @list: The list.
 *   @wire: The wire.
 *   &returns: The index if found, negative otherwise.
 */
int cir_wire_idx(struct cir_wire_t **list, struct cir_wire_t *wire)
{
	unsigned int i;

	for(i = 0; list[i] != NULL; i++) {
		if(list[i] == wire)
			return i;
	}

	return -1;
}


/**
 * Create an environment.
 *   &returns: The environment.
 */
struct cir_env_t *cir_env_new(void)
{
	return NULL;
}

/**
 * Delete an environment.
 *   @env: The environment.
 */
void cir_env_delete(struct cir_env_t *env)
{
	struct cir_env_t *tmp;

	while(env != NULL) {
		tmp = env;
		env = tmp->next;

		r_var_delete(tmp->var);
		free(tmp);
	}
}

/**
 * Retrieve a variable from a environment.
 *   @env: The environment.
 *   @key: The key.
 *   &returns: The variable.
 */
struct r_var_t *cir_env_get(struct cir_env_t *env, void *key)
{
	while(env != NULL) {
		if(env->key == key)
			return env->var;

		env = env->next;
	}

	return NULL;
}

/**
 * Add a variable to the environment.
 *   @env: The environment reference.
 *   @key: The key.
 *   @var: The variable.
 */
void cir_env_add(struct cir_env_t **env, void *key, struct r_var_t *var)
{
	struct cir_env_t *add;

	add = malloc(sizeof(struct cir_env_t));
	add->key = key;
	add->var = var;
	add->next = *env;
	*env = add;
}


/**
 * Compute the system of equations given a root.
 *   @root: The root node.
 *   &returns: The system.
 */
struct r_sys_t *cir_system(struct cir_node_t *root)
{
	unsigned int i, idx;
	struct cir_port_t *port;
	struct cir_wire_t **wire;
	struct cir_node_t **node;
	struct cir_env_t *env;
	struct r_list_t *list;
	struct r_sys_t *sys, **iter;

	wire = cir_wire_enum(root);
	node = cir_node_enum(root);
	env = cir_env_new();

	sys = r_sys_new();
	iter = &sys;

	idx = 0;
	for(i = 0; wire[i] != NULL; i++) {
		list = r_list_new();

		cir_env_add(&env, wire[i], r_var_new(mprintf("v%u", i)));

		for(port = wire[i]->port; port != NULL; port = port->next) {
			struct r_var_t *var;

			var = r_var_new(mprintf("i%u", idx++));
			cir_env_add(&env, port, var);
			r_list_add(&list, r_expr_var(r_var_copy(var)));
		}

		if(!r_list_has_unk(list))
			iter = r_sys_add(iter, r_rel_eq(r_expr_sum(list), r_expr_flt(0.0)));
		else
			r_list_delete(list);
	}

	for(i = 0; node[i] != NULL; i++) {
		switch(node[i]->type) {
		case cir_input_v:
			iter = r_sys_add(iter, r_rel_eq(r_expr_var(r_var_copy(cir_env_get(env, node[i]->port[0].wire))), r_expr_const(strdup(node[i]->data.str))));
			break;

		case cir_output_v:
			iter = r_sys_add(iter, r_rel_eq(r_expr_var(r_var_copy(cir_env_get(env, node[i]->port[0].wire))), r_expr_var(r_var_new(strdup(node[i]->data.str)))));
			iter = r_sys_add(iter, r_rel_eq(r_expr_var(r_var_copy(cir_env_get(env, &node[i]->port[0]))), r_expr_flt(0.0)));
			break;

		case cir_value_v:
			iter = r_sys_add(iter, r_rel_eq(r_expr_var(r_var_copy(cir_env_get(env, node[i]->port[0].wire))), r_expr_flt(node[i]->data.flt)));
			break;

		case cir_res_v:
			{
				struct r_var_t *left, *right, *src, *dest;

				left = r_var_copy(cir_env_get(env, &node[i]->port[0]));
				right = r_var_copy(cir_env_get(env, &node[i]->port[1]));

				iter = r_sys_add(iter, r_rel_eq(r_expr_add(r_expr_var(left), r_expr_var(right)), r_expr_zero()));

				src = r_var_copy(cir_env_get(env, node[i]->port[0].wire));
				dest = r_var_copy(cir_env_get(env, node[i]->port[1].wire));

				iter = r_sys_add(iter, r_rel_eq(r_expr_mul(r_expr_flt(node[i]->data.flt), r_expr_var(r_var_copy(left))), r_expr_sub(r_expr_var(dest), r_expr_var(src))));
			}
			break;

		case cir_cap_v:
			{
				struct r_var_t *in, *out, *src, *dest, *diff;

				diff = r_var_new(strdup("w"));
				in = cir_env_get(env, &node[i]->port[0]);
				out = cir_env_get(env, &node[i]->port[1]);
				src = cir_env_get(env, node[i]->port[0].wire);
				dest = cir_env_get(env, node[i]->port[1].wire);

				// y[n] = 2/dt x[n] - s[n-1]
				// s[n] = y[n] + 2/dt x[n]
				iter = r_sys_add(iter, r_rel_eq(r_expr_vardup(diff), r_expr_sub(r_expr_vardup(src), r_expr_vardup(dest))));
				iter = r_sys_add(iter, r_rel_eq(r_expr_add(r_expr_vardup(in), r_expr_vardup(out)), r_expr_zero()));
				struct r_var_t *t1;
				iter = r_sys_add(iter, r_rel_eq(r_expr_sub(r_expr_vardup(out), r_expr_vardup(in)),
					r_expr_mul(
						r_expr_flt(node[i]->data.flt),
						r_expr_var(t1 = r_var_new(strdup("t1")))
					)
				));
				iter = r_sys_add(iter, r_rel_eq(
					r_expr_var(r_var_copy(t1)),
					r_expr_sub(
						r_expr_mul(
							r_expr_div(r_expr_flt(2.0), r_expr_const(strdup("dt"))),
							r_expr_vardup(diff)
						),
						r_expr_const(strdup("s1'"))
					)
				));
				iter = r_sys_add(iter, r_rel_eq(r_expr_var(r_var_new(strdup("s1"))),
					r_expr_add(
						r_expr_vardup(out),
						r_expr_mul(
							r_expr_div(r_expr_flt(2.0), r_expr_const(strdup("dt"))),
							r_expr_vardup(diff)
						)
					)
				));

				r_var_delete(diff);
			}
			break;
		}
	}

	free(wire);
	free(node);
	cir_env_delete(env);

	return sys;
}
