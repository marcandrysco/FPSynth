#include "common.h"


/**
 * Wire structure.
 *   @port: The first port.
 */
struct cir_wire_t {
	struct cir_port_t *port;
};

/**
 * Port structure.
 *   @node: The owner node.
 *   @wire: The parent wire.
 *   @port: The next port on a wire.
 */
struct cir_port_t {
	struct cir_node_t *node;
	struct cir_wire_t *wire;
	struct cir_port_t *next;
};

/**
 * Node type enumerator.
 *   @cir_input_v: Input.
 *   @cir_output_v: Output.
 *   @cir_value_v: Constant value.
 *   @cir_res_v: Resistor.
 *   @cir_cap_v: Capacitor.
 */
enum cir_node_e {
	cir_input_v,
	cir_output_v,
	cir_value_v,
	cir_res_v,
	cir_cap_v
};

/**
 * Node data union.
 *   @flt: Floating-point value.
 */
union cir_node_u {
	double flt;
};

/**
 * Note structure.
 *   @tag: The node tag.
 *   @type: The type.
 *   @data: The data.
 *   @port: The port array.
 *   @cnt: The number of ports.
 */
struct cir_node_t {
	char *tag;
	enum cir_node_e type;
	union cir_node_u data;

	struct cir_port_t *port;
	unsigned int cnt;
};


/*
 * circuit declarations
 */
void cir_connect(struct cir_port_t *left, struct cir_port_t *right);
void cir_disconnect(struct cir_port_t *port);

struct avltree_t cir_nodes(struct cir_node_t *node);
void cir_nodes_iter(struct cir_node_t *node, struct avltree_t *tree);

struct avltree_t cir_wires(struct cir_node_t *node);
void cir_wires_iter(struct cir_wire_t *wire, struct avltree_t *tree);


/**
 * Create a new node.
 *   @tag: The tag.
 *   @type: The type.
 *   @data: The data.
 *   @cnt: The number of ports.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_new(char *tag, enum cir_node_e type, union cir_node_u data, unsigned int cnt)
{
	unsigned int i;
	struct cir_node_t *node;

	node = malloc(sizeof(struct cir_node_t));
	node->tag = tag;
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

	for(i = 0; i < node->cnt; i++) {
		cir_disconnect(&node->port[i]);
		free(node->port[i].wire);
	}

	erase(node->tag);
	free(node->port);
	free(node);
}


/**
 * Create an input node.
 *   @tag: The tag.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_input(char *tag)
{
	return cir_node_new(tag, cir_input_v, (union cir_node_u){ }, 1);
}

/**
 * Create an output node.
 *   @tag: The tag.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_output(char *tag)
{
	return cir_node_new(tag, cir_output_v, (union cir_node_u){ }, 1);
}

/**
 * Create a value node.
 *   @flt: The floating-point value.
 *   &returns: The node.
 */
struct cir_node_t *cir_node_value(double flt)
{
	return cir_node_new(NULL, cir_value_v, (union cir_node_u){ .flt = flt }, 1);
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

	tree = avltree_init(compare_ptr, delete_noop);
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
	unsigned int i;
	struct cir_port_t *port;

	if(avltree_lookup(tree, node) != NULL)
		return;

	avltree_insert(tree, node, node);

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

	tree = avltree_init(compare_ptr, delete_noop);

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
	unsigned int i;
	struct cir_port_t *port;

	if(avltree_lookup(tree, wire) != NULL)
		return;

	avltree_insert(tree, wire, wire);

	for(port = wire->port; port != NULL; port = port->next) {
		for(i = 0; i < port->node->cnt; i++)
			cir_wires_iter(port->node->port[i].wire, tree);
	}
}


/**
 * Load an array of sample data.
 *   @path: The path.
 *   @arr: Ref. The output array.
 *   @len: The length.
 */
void snd_load(const char *path, double **arr, unsigned int *len)
{
	SNDFILE *file;
	SF_INFO info;

	info.format = 0;
	file = sf_open(path, SFM_READ, &info);
	if(file == NULL)
		fatal("Cannot open file '%s'.", path);

	if(info.channels > 1)
		fatal("File '%s' has more than 1 channel.", path);

	*arr = malloc(info.frames * sizeof(double));
	*len = info.frames;

	sf_readf_double(file, *arr, *len);
	sf_close(file);
}


/*
 * (x[n] - 2x[n-1] + x[n-2]) h^2 - K sin(x[n]) = 0
 */
void test1(void)
{
	double *in, *ref, *cmp;
	unsigned int i, k, len;

	snd_load("sample.flac", &in, &len);
	ref = malloc(len * sizeof(double));
	cmp = malloc(len * sizeof(double));

	//memcpy(in, (double[]){ 0,3, 5, 6}, 4*sizeof(double));
	//len = 4;

	ref[0] = 1.6*in[0];
	for(i = 1; i < len; i++)
		ref[i] = 1.6 * in[i] + ref[i-1];

	//printf("ref: %f %f %f %f\n", ref[0], ref[1], ref[2], ref[3]);

	struct fl_gen_t *gen;
	struct m_rand_t rand = m_rand_init(0);
	struct fl_weight_t weight = {
		.add = 8.0f,
		.sub = 2.0f,
		.mul = 8.0f,
		.div = 1.0f
	};

	fl_weight_norm(&weight);

	gen = fl_gen_new();
	fl_gen_const(gen, 1.6);
	fl_gen_add(gen, fl_inst_new(fl_func_new(1, 1, 1)));

	double s[1];

	for(k = 0; k < 100000; k++) {
		struct fl_inst_t *inst;
		double diff, max = 0.0;

		inst = fl_gen_trial(gen, &weight, &rand);
		if(inst == NULL)
			continue;

		s[0] = 0.0;
		for(i = 0; i < len; i++) {
			fl_func_eval(inst->func, &in[i], &cmp[i], s);
			if(isnan(cmp[i]))
				break;

			diff = fabs(cmp[i] - ref[i]);
			max = fmax(diff, max);
			if(max > 0.001)
				break;
		}

		if(i == len) {
			printf("match: %g\n", max);
			//printf("HERE! %g : %f %f %f %f\n", max, cmp[0], cmp[1], cmp[2], cmp[3]);
			fl_func_dump(inst->func);
		}
	}

	fl_gen_delete(gen);

	free(in);
	free(ref);
	free(cmp);
}


/**
 * Main entry point.
 *   @argc: The number of argument.
 *   @argv: The argument array.
 *   &returns: Always zero.
 */
int main(int argc, char **argv)
{
	test1();

	/*
	struct cir_node_t *in, *out;

	in = cir_node_input(mprintf("In"));
	out = cir_node_output(mprintf("Out"));

	cir_connect(&in->port[0], &out->port[0]);

	{
		struct avltree_t tree;
		
		tree = cir_nodes(in);
		avltree_destroy(&tree);
	}

	cir_node_delete(in);
	cir_node_delete(out);
	*/

	/*
	struct fl_func_t *func;

	func = fl_func_new(1, 1, 0);
	unsigned int tmp;
	struct fl_expr_t **expr = fl_func_rand(func, &tmp);
	fl_func_dump(func);
	fl_expr_set(expr, fl_expr_flt(2));
	fl_func_dump(func);
	fl_func_delete(func);
	*/

	/*
	struct fl_gen_t *gen;
	struct m_rand_t rand = m_rand_init(0);

	gen = fl_gen_new();
	fl_gen_add(gen, fl_inst_new(fl_func_new(2, 1, 0)));

	//uint64_t tm = sys_utime();

	struct fl_weight_t weight = {
		.add = 8.0f,
		.sub = 2.0f,
		.mul = 8.0f,
		.div = 1.0f
	};
	fl_weight_norm(&weight);

	for(int i = 0; i < 5000; i++)
		fl_gen_trial(gen, &weight, &rand);

	//printf("%.1f\n", (sys_utime() - tm) / 1e3);

	printf("len: %d\n", gen->len);
	for(int i = 0; i < gen->len; i++)
		//printf("hash: %016lx\n", gen->arr[i]->hash),
		fl_func_dump(gen->arr[i]->func),printf("\n");

	fl_gen_delete(gen);
	*/

	if(hax_memcnt != 0)
		fprintf(stderr, "allocated memory: %d\n", hax_memcnt), exit(1);

	return 0;
}
