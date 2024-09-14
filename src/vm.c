#include "vm.h"

#include <error.h>
#include <stdio.h>
#include <string.h>
#include "bytecode.h"
#include "class.h"
#include "clsmgr.h"
#include "frame.h"

static void imp_execute(r11f_vm_t *vm, void *output);

R11F_EXPORT
r11f_error_t r11f_vm_invoke(r11f_vm_t *vm,
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

    imp_execute(vm, output);

    return R11F_success;
}

static void imp_execute(r11f_vm_t *vm, void *output) {
    while (vm->current_frame) {
        uint8_t insc = vm->current_frame->code[vm->current_frame->pc];
        switch (insc) {
            case R11F_iload_0:
                vm->current_frame->stack[vm->current_frame->sp + 1] =
                    vm->current_frame->locals[0];
                vm->current_frame->sp++;
                vm->current_frame->pc += 1;
                break;
            case R11F_iload_1:
                vm->current_frame->stack[vm->current_frame->sp + 1] =
                    vm->current_frame->locals[1];
                vm->current_frame->sp++;
                vm->current_frame->pc += 1;
                break;
            case R11F_iadd: {
                int a = vm->current_frame->stack[vm->current_frame->sp];
                int b = vm->current_frame->stack[vm->current_frame->sp - 1];
                vm->current_frame->sp -= 1;
                vm->current_frame->stack[vm->current_frame->sp] = a + b;
                vm->current_frame->pc += 1;
                break;
            }
            case R11F_ireturn: {
                int32_t result = vm->current_frame->stack[vm->current_frame->sp];

                vm->current_frame = vm->current_frame->parent;
                if (!vm->current_frame) {
                    if (output) {
                        *(int32_t*)output = result;
                    }
                }
                else {
                    vm->current_frame->stack[vm->current_frame->sp] = result;
                    vm->current_frame->sp++;
                }
                break;
            }
            default: {
                fprintf(stderr, "unknown instruction: %02x\n", insc);
                vm->current_frame = NULL;
                break;
            }
        }
    }
}
