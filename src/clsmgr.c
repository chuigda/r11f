#include "clsmgr.h"

#include <error.h>
#include <stdlib.h>
#include <string.h>
#include "clsfile.h"
#include "clsfile/cpool.h"
#include "defs.h"

typedef struct st_imp_hashtable_node hashtable_node_t;

struct st_imp_hashtable_node {
    hashtable_node_t *next;

    uint32_t classid;
    uint16_t class_name_len;
    char const *class_name;
    r11f_classfile_t *classfile;
};

struct st_r11f_classmgr {
    size_t hash_size;
    uint32_t next_classid;

    hashtable_node_t *hash_table_name;
    hashtable_node_t *hash_table_id;
    hashtable_node_t nodes[];
};

static size_t bkdr_hash(char const *str);
static size_t bkdr_hash2(char const *str, size_t len);

R11F_EXPORT r11f_classmgr_t *r11f_classmgr_alloc(void) {
    return r11f_classmgr_alloc_hash_size(1024);
}

R11F_EXPORT r11f_classmgr_t *r11f_classmgr_alloc_hash_size(size_t hash_size) {
    r11f_classmgr_t *mgr = calloc(
        1,
        sizeof(r11f_classmgr_t) + 2 * hash_size * sizeof(hashtable_node_t)
    );
    if (!mgr) {
        return NULL;
    }

    mgr->hash_size = hash_size;
    mgr->hash_table_name = mgr->nodes;
    mgr->hash_table_id = mgr->nodes + hash_size;

    return mgr;
}

R11F_EXPORT r11f_error_t r11f_classmgr_add_class(r11f_classmgr_t *mgr,
                                                 r11f_classfile_t *classfile,
                                                 uint32_t *classid) {
    uint16_t name_index = classfile->this_class;
    r11f_constant_utf8_info_t *info = classfile->constant_pool[name_index];
    size_t class_name_len = info->length;
    char const *class_name = (char const*)info->bytes;
    *classid = mgr->next_classid;

    size_t hash = bkdr_hash2(class_name, class_name_len) % mgr->hash_size;
    hashtable_node_t *name_table_root_node =
        &mgr->hash_table_name[hash];
    hashtable_node_t *id_table_root_node =
        &mgr->hash_table_id[*classid % mgr->hash_size];

    hashtable_node_t *new_name_node = NULL;
    hashtable_node_t *new_id_node = NULL;
    if (name_table_root_node->classfile) {
        new_name_node = malloc(sizeof(hashtable_node_t));
        if (!new_name_node) {
            return R11F_ERR_out_of_memory;
        }
    }
    if (id_table_root_node->classfile) {
        new_id_node = malloc(sizeof(hashtable_node_t));
        if (!new_id_node) {
            free(new_name_node);
            return R11F_ERR_out_of_memory;
        }
    }

    hashtable_node_t *updated_name_node = name_table_root_node;
    hashtable_node_t *updated_id_node = id_table_root_node;
    if (new_name_node) {
        new_name_node->next = name_table_root_node->next;
        name_table_root_node->next = new_name_node;
        updated_name_node = new_name_node;
    }
    if (new_id_node) {
        new_id_node->next = id_table_root_node->next;
        id_table_root_node->next = new_id_node;
        updated_id_node = new_id_node;
    }

    updated_name_node->classid = *classid;
    updated_name_node->class_name_len = class_name_len;
    updated_name_node->class_name = class_name;
    updated_name_node->classfile = classfile;

    updated_id_node->classid = *classid;
    updated_id_node->class_name_len = class_name_len;
    updated_id_node->class_name = class_name;
    updated_id_node->classfile = classfile;

    mgr->next_classid++;
    return R11F_success;
}

R11F_EXPORT r11f_classfile_t *r11f_classmgr_find_class(r11f_classmgr_t *mgr,
                                                       char const *name) {
    size_t hash = bkdr_hash(name) % mgr->hash_size;
    hashtable_node_t *node = &mgr->hash_table_name[hash];
    for (; node; node = node->next) {
        if (node->classfile &&
            !strncmp(node->class_name, name, node->class_name_len)) {
            return node->classfile;
        }
    }

    return NULL;
}

R11F_EXPORT r11f_classfile_t *r11f_classmgr_find_class_id(r11f_classmgr_t *mgr,
                                                          uint32_t classid) {
    size_t hash = classid % mgr->hash_size;
    hashtable_node_t *node = &mgr->hash_table_id[hash];
    for (; node; node = node->next) {
        if (node->classfile && node->classid == classid) {
            return node->classfile;
        }
    }

    return NULL;
}

R11F_EXPORT void r11f_classmgr_free(r11f_classmgr_t *mgr) {
    free(mgr);
}

static size_t bkdr_hash(char const *str) {
    size_t hash = 0;
    for (size_t i = 0; str[i]; i++) {
        hash = hash * 13 + str[i];
    }
    return hash;
}

static size_t bkdr_hash2(char const *str, size_t len) {
    size_t hash = 0;
    for (size_t i = 0; i < len; i++) {
        hash = hash * 13 + str[i];
    }
    return hash;
}
