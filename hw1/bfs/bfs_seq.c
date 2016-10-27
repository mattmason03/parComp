#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void *zmalloc(size_t size) {
    void *ptr = malloc(size);

    if (!ptr) {
        printf("Out of memory\n");
        exit(-__LINE__);
    }
    bzero(ptr, size);
    return ptr;
}

struct raw_edge {
    u_int64_t   from;
    u_int64_t   to;
};

#define BASE_VERTEX_ALLOCATION  (8)
struct vertex {
    // data used for the base graph connections
    u_int64_t   id;
    u_int64_t edge_count;
    u_int64_t allocated_edges;
    struct vertex   **edges;

    // data used for BFS algorithm
    int64_t level;
    struct vertex *prev, *next;
};

// Adjust this up or down depending on the graph size of interest.
// It does not effect BFS speed, but it will effect ID -> vertex mapping speed
#define HASH_MAP_SIZE   (1000001)

struct vertex_id_map {
    u_int64_t id;
    struct vertex *vertex;
    struct vertex_id_map *next;
};

u_int64_t hash_vertex_id(u_int64_t id) {
    return id % HASH_MAP_SIZE;
}

struct vertex_id_map **create_vertex_id_hash() {
    struct vertex_id_map **vertex_id_map;

    vertex_id_map = zmalloc(sizeof(*vertex_id_map) * HASH_MAP_SIZE);
    return vertex_id_map;
}

struct vertex *create_vertex(u_int64_t id) {
    struct vertex *v = zmalloc(sizeof(*v));
    v->id = id;
    v->allocated_edges = BASE_VERTEX_ALLOCATION;
    v->edges = zmalloc(sizeof(*v->edges) * BASE_VERTEX_ALLOCATION);

    return v;
}

void insert_vertex_to_id_hash(struct vertex_id_map **vertex_id_map, struct vertex *v) {
    struct vertex_id_map *vm = zmalloc(sizeof(*vm));
    vm->id = v->id;
    vm->vertex = v;
    u_int64_t hash = hash_vertex_id(v->id);
    vm->next = vertex_id_map[hash];
    vertex_id_map[hash] = vm;
}

struct vertex *locate_vertex_from_id(struct vertex_id_map **vertex_id_map, u_int64_t id) {
    struct vertex_id_map *vm = vertex_id_map[hash_vertex_id(id)];

    while (vm) {
        if (vm->id == id)
            return vm->vertex;
        vm = vm->next;
    }

    return NULL;
}

void _add_edge_to_vertex(struct vertex *from, struct vertex *to) {
    if (from->allocated_edges == from->edge_count) {
        struct vertex **vm = zmalloc(sizeof(*vm) * from->allocated_edges * 2);
        memcpy(vm, from->edges, sizeof(*vm) * from->allocated_edges);
        free(from->edges);
        from->edges = vm;
        from->allocated_edges = from->allocated_edges * 2;
    }
    from->edges[from->edge_count] = to;
    ++from->edge_count;
}

void add_edge_to_vertex(struct vertex *v1, struct vertex *v2) {
    _add_edge_to_vertex(v1, v2);
    _add_edge_to_vertex(v2, v1);
}

struct vertex_id_map **read_graph(char *filename) {
    FILE *fp = fopen(filename, "rb");

    if (!fp) {
        printf("Failed to open:");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    u_int64_t   filesize = ftell(fp);

    u_int64_t edge_count = filesize / (2 * sizeof(u_int64_t));

    struct raw_edge *raw_edges = zmalloc(sizeof(*raw_edges) * edge_count);

    fseek(fp, 0, SEEK_SET);

    u_int64_t total_read = 0;

    char *base_ptr = (char *) raw_edges;
    while (true) {
        int read_size = fread(base_ptr, 1, filesize, fp);

        total_read += read_size;
        base_ptr += read_size;

        if (total_read == filesize)
            break;

        if (feof(fp)) {
            printf("Failed to read file\n");
            fclose(fp);
            free(raw_edges);
            return NULL;
        }
    }
    fclose(fp);

    struct vertex_id_map **graph = create_vertex_id_hash();

    u_int64_t edge_index;

    for (edge_index = 0; edge_index < edge_count; edge_index++) {
        struct vertex *from_v, *to_v;

        from_v = locate_vertex_from_id(graph, raw_edges[edge_index].from);
        if (!from_v) {
            from_v = create_vertex(raw_edges[edge_index].from);
            insert_vertex_to_id_hash(graph, from_v);
        }

        to_v = locate_vertex_from_id(graph, raw_edges[edge_index].to);
        if (!to_v) {
            to_v = create_vertex(raw_edges[edge_index].to);
            insert_vertex_to_id_hash(graph, to_v);
        }

        add_edge_to_vertex(from_v, to_v);
    }

    return graph;
}

void for_all_vertices(struct vertex_id_map **map, void (*func)(struct vertex *, void *), void *arg) {
    u_int64_t i;
    struct vertex_id_map *vm;

    for (i = 0; i < HASH_MAP_SIZE; i++) {
        vm = map[i];
        while (vm) {
            func(vm->vertex, arg);
            vm = vm->next;
        }
    }
}


#define UNVISITED   (-1)
void _clear_bfs_state(struct vertex *v, void *arg) {
    v->level = UNVISITED;
    v->next = NULL;
    v->prev = NULL;
}

void clear_bfs_state(struct vertex_id_map **graph) {
    for_all_vertices(graph, _clear_bfs_state, NULL);
}

void bfs(struct vertex_id_map **graph, int root) {
    u_int64_t i;
    clear_bfs_state(graph);
    struct vertex *start = NULL, *end = NULL;

    struct vertex *v = locate_vertex_from_id(graph, root);

    if (!v)
        return;

    v->level = 0;
    start = v;
    end = v;

    while (start) {
        // dequeue
        v = start;
        start = start->next;
        if (v == end)
            end = NULL;
        v->prev = v->next = NULL;

        for (i = 0; i < v->edge_count; i++) {
            struct vertex *nv = v->edges[i];
            if (nv->level == UNVISITED ||
                nv->level > (v->level + 1)) {
                // Label
                nv->level = v->level + 1;
                // enqueue
                if (!nv->next && !nv->prev && start != nv && end != nv) {
                    nv->next = NULL;
                    nv->prev = end;
                    if (end)
                        end->next = nv;
                    end = nv;
                    if (!start)
                        start = nv;
                }
            }
        }
    }
}

struct _gather_statistics_s {
    u_int64_t *vertices;
    u_int64_t *reached_vertices;
    u_int64_t *edges;
    int64_t *max_level;
};
void _grather_statistics(struct vertex *v, void *v_arg) {
    struct _gather_statistics_s *arg = v_arg;

    ++(*arg->vertices);
    *(arg->edges) += v->edge_count;
    if (v->level != UNVISITED)
        ++(*arg->reached_vertices);
    if (v->level > *(arg->max_level))
        (*arg->max_level) = v->level;
}

void grather_statistics(struct vertex_id_map **graph,
    u_int64_t *vertices,
    u_int64_t *reached_vertices,
    u_int64_t *edges,
    int64_t *max_level) {
    struct _gather_statistics_s arg;

    *vertices = 0;
    *reached_vertices = 0;
    *edges = 0;
    *max_level = UNVISITED;
    arg.vertices = vertices;
    arg.reached_vertices = reached_vertices;
    arg.edges = edges;
    arg.max_level = max_level;

    for_all_vertices(graph, _grather_statistics, &arg);
}

int main(int argc, char *argv[]) {
    u_int64_t vertices, reached_vertices, edges;
    int64_t max_level;

    if (argc != 3) {
        printf("usage: %s graph_file root_id", argv[0]);
        exit(-__LINE__);
    }

    struct vertex_id_map **graph = read_graph(argv[1]);

    if (!graph)
        exit(-1);

    u_int64_t root = atoi(argv[2]);
    bfs(graph, root);

    grather_statistics(graph, &vertices, &reached_vertices, &edges, &max_level);

    printf("Graph vertices: %ld with total edges %ld.  Reached vertices from %ld is %ld and max level is %ld\n",
        vertices, edges, root, reached_vertices, max_level);

    return 0;
}