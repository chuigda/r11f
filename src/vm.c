#include "vm.h"

#include <assert.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include "alloc.h"
#include "bytecode.h"
#include "class.h"
#include "class/cpool.h"
#include "clsfile.h"
#include "clsmgr.h"
#include "forward.h"
#include "frame.h"

static r11f_error_t vm_execute(r11f_vm_t *vm, void *output);
static r11f_error_t vm_invokestatic(r11f_vm_t *vm);
static r11f_error_t vm_get_class(r11f_vm_t *vm,
                                 char const *class_name,
                                 size_t class_name_len,
                                 r11f_class_t **output);
static void get_class_name(r11f_class_t *class,
                           r11f_constant_methodref_info_t *methodref_info,
                           char const **out_class_name,
                           uint16_t *out_class_name_len);

R11F_EXPORT
r11f_error_t r11f_vm_invoke_static(r11f_vm_t *vm,
                            char const *class_name,
                            char const *method_name,
                            char const *method_descriptor,
                            uint16_t argc,
                            uint32_t argv[],
                            void *output) {
    r11f_class_t *clazz = r11f_classmgr_find_class(vm->classmgr, class_name);
    if (!clazz) {
        return R11F_ERR_class_not_found;
    }

    r11f_method_info_t *method_info = r11f_class_resolve_method(
        clazz,
        method_name,
        strlen(method_name),
        method_descriptor,
        strlen(method_descriptor)
    );
    if (!method_info) {
        return R11F_ERR_method_not_found;
    }

    if (method_info->access_flags & R11F_ACC_ABSTRACT) {
        return R11F_ERR_cannot_invoke_abstract_method;
    }

    if (method_info->access_flags & R11F_ACC_NATIVE) {
        return R11F_ERR_cannot_invoke_native_method;
    }

    if (!(method_info->access_flags & R11F_ACC_STATIC)) {
        return R11F_ERR_cannot_invoke_non_static_method;
    }

    r11f_frame_t *frame = r11f_frame_alloc(clazz, method_info);
    if (!frame) {
        return R11F_ERR_out_of_memory;
    }

    for (uint16_t i = 0; i < argc; i++) {
        frame->locals[i] = argv[i];
    }
    vm->current_frame = frame;

    return vm_execute(vm, output);
}

static r11f_error_t vm_execute(r11f_vm_t *vm, void *output) {
    while (vm->current_frame) {
        r11f_frame_t *frame = vm->current_frame;
        r11f_stack_value_t *stack = frame->stack;
        uint32_t *locals = frame->locals;

        uint8_t insc = frame->code[frame->pc];
        switch (insc) {
            case R11F_iload_0:
                stack[frame->sp + 1].i32 = *(int32_t*)(locals + 0);
                frame->sp++;
                frame->pc += 1;
                break;
            case R11F_iload_1:
                stack[frame->sp + 1].i32 = *(int32_t*)(locals + 1);
                frame->sp++;
                frame->pc += 1;
                break;
            case R11F_iadd: {
                int32_t a = stack[frame->sp].i32;
                int32_t b = stack[frame->sp - 1].i32;
                stack[frame->sp - 1].i32 = a + b;
                frame->sp--;
                frame->pc++;
                break;
            }
            case R11F_ireturn: {
                vm->current_frame = frame->parent;
                if (vm->current_frame) {
                    vm->current_frame->stack[vm->current_frame->sp].i32
                        = stack[frame->sp].i32;
                    vm->current_frame->sp++;
                }
                else {
                    *(int32_t*)output = stack[frame->sp].i32;
                }
                r11f_free(frame);
                break;
            }
            case R11F_invokestatic: {
                r11f_error_t err = vm_invokestatic(vm);
                if (err != R11F_success) {
                    return err;
                }
                break;
            }
            default: {
                fprintf(stderr, "unknown instruction: %02x\n", insc);
                return R11F_ERR_malformed_classfile;
            }
        }
    }

    return R11F_success;
}

static r11f_error_t vm_invokestatic(r11f_vm_t *vm) {
    assert(vm->current_frame->code[vm->current_frame->pc] == R11F_invokestatic);

    uint16_t methodref_index =
        (vm->current_frame->code[vm->current_frame->pc + 1] << 8)
        |  vm->current_frame->code[vm->current_frame->pc + 2];
    vm->current_frame->pc += 3;

    r11f_constant_methodref_info_t *methodref_info =
        vm->current_frame->clazz->constant_pool[methodref_index];

    char const *class_name;
    uint16_t class_name_len;
    get_class_name(vm->current_frame->clazz,
                   methodref_info,
                   &class_name,
                   &class_name_len);

    r11f_class_t *clazz;
    r11f_error_t err = vm_get_class(vm, class_name, class_name_len, &clazz);
    if (err != R11F_success) {
        return err;
    }

    r11f_method_info_t *method_info = r11f_class_resolve_method(
        clazz,
        class_name,
        class_name_len,
        class_name,
        class_name_len
    );
    if (!method_info) {
        return R11F_ERR_method_not_found;
    }

    if (method_info->access_flags & R11F_ACC_ABSTRACT) {
        return R11F_ERR_cannot_invoke_abstract_method;
    }

    if (method_info->access_flags & R11F_ACC_NATIVE) {
        return R11F_ERR_cannot_invoke_native_method;
    }

    if (!(method_info->access_flags & R11F_ACC_STATIC)) {
        return R11F_ERR_cannot_invoke_non_static_method;
    }

    r11f_frame_t *frame = r11f_frame_alloc(clazz, method_info);
    if (!frame) {
        return R11F_ERR_out_of_memory;
    }

    return R11F_success;
}

static r11f_error_t vm_get_class(r11f_vm_t *vm,
                                 char const *class_name,
                                 size_t class_name_len,
                                 r11f_class_t **output) {
    r11f_class_t *clazz = r11f_classmgr_find_class(vm->classmgr, class_name);
    if (clazz) {
        *output = clazz;
        return R11F_success;
    }

    char file_name[class_name_len + 7];
    strncpy(file_name, class_name, class_name_len);
    file_name[class_name_len] = '\0';
    strcat(file_name, ".class");

    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        return R11F_ERR_class_not_found;
    }

    r11f_class_t *class = r11f_alloc(sizeof(r11f_class_t));
    if (!class) {
        fclose(fp);
        return R11F_ERR_out_of_memory;
    }

    r11f_error_t err = r11f_classfile_read(fp, class);
    fclose(fp);
    if (err != R11F_success) {
        r11f_free(class);
        return err;
    }

    uint32_t classid;
    err = r11f_classmgr_add_class(vm->classmgr, class, &classid);
    if (err != R11F_success) {
        r11f_class_cleanup(class);
        r11f_free(class);
        return err;
    }

    // TODO: if there's a static initializer, invoke it
    *output = class;
    return R11F_success;
}

static void get_class_name(r11f_class_t *clazz,
                           r11f_constant_methodref_info_t *methodref_info,
                           char const **out_class_name,
                           uint16_t *out_class_name_len) {
    r11f_constant_class_info_t *class_info =
        clazz->constant_pool[methodref_info->class_index];
    r11f_constant_utf8_info_t *utf8_info =
        clazz->constant_pool[class_info->name_index];
    *out_class_name = (char const*)utf8_info->bytes;
    *out_class_name_len = utf8_info->length;
}
