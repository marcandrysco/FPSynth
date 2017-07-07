#ifndef CIR_H
#define CIR_H

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
 *   @str: The string.
 *   @flt: Floating-point value.
 */
union cir_node_u {
	char *str;
	double flt;
};

/**
 * Note structure.
 *   @type: The type.
 *   @data: The data.
 *   @port: The port array.
 *   @cnt: The number of ports.
 */
struct cir_node_t {
	enum cir_node_e type;
	union cir_node_u data;

	struct cir_port_t *port;
	unsigned int cnt;
};


/*
 * circuit declarations
 */
struct cir_node_t *cir_node_new(enum cir_node_e type, union cir_node_u data, unsigned int cnt);
void cir_node_delete(struct cir_node_t *node);
void cir_node_erase(struct cir_node_t *node);

struct cir_node_t *cir_node_input(char *tag);
struct cir_node_t *cir_node_output(char *tag);
struct cir_node_t *cir_node_gnd(void);
struct cir_node_t *cir_node_value(double flt);
struct cir_node_t *cir_node_res(double flt);
struct cir_node_t *cir_node_cap(double flt);

void cir_connect(struct cir_port_t *left, struct cir_port_t *right);
void cir_disconnect(struct cir_port_t *port);

struct avltree_t cir_nodes(struct cir_node_t *node);
void cir_nodes_iter(struct cir_node_t *node, struct avltree_t *tree);

struct avltree_t cir_wires(struct cir_node_t *node);
void cir_wires_iter(struct cir_wire_t *wire, struct avltree_t *tree);

struct cir_node_t **cir_node_enum(struct cir_node_t *node);
void cir_node_iter(struct cir_node_t *node, struct cir_node_t ***list, unsigned int *cnt);
int cir_node_idx(struct cir_node_t **list, struct cir_node_t *node);

struct cir_wire_t **cir_wire_enum(struct cir_node_t *node);
void cir_wire_iter(struct cir_wire_t *wire, struct cir_wire_t ***list, unsigned int *cnt);
int cir_wire_idx(struct cir_wire_t **list, struct cir_wire_t *wire);


/**
 * Variable environment structure.
 *   @key: The key.
 *   @var: The variable.
 *   @next: The next environment.
 */
struct cir_env_t {
	void *key;
	struct r_var_t *var;

	struct cir_env_t *next;
};

/*
 * environment declarations
 */
struct cir_env_t *cir_env_new(void);
void cir_env_delete(struct cir_env_t *env);

struct r_var_t *cir_env_get(struct cir_env_t *env, void *key);
void cir_env_add(struct cir_env_t **env, void *key, struct r_var_t *var);

/*
 * high-level declarations
 */
struct r_sys_t *cir_system(struct cir_node_t *root);

#endif
