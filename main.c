#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "clsfile.h"
#include "class.h"
#include "cfdump.h"
#include "clsmgr.h"
#include "error.h"
#include "vm.h"

void drill_main(void);

int main(int argc, char *argv[]) {
    if (argc >= 3 && !strcmp(argv[1], "--dump")) {
        for (int i = 2; i < argc; i++) {
            r11f_class_t classfile;
            memset(&classfile, 0, sizeof(r11f_class_t));

            FILE *fp = fopen(argv[i], "rb");
            if (!fp) {
                fprintf(stderr, "error: failed to open file %s\n", argv[i]);
                continue;
            }

            r11f_error_t err = r11f_classfile_read(fp, &classfile);
            if (err != R11F_success) {
                fprintf(
                    stderr,
                    "error: read file %s: %s\n",
                    argv[i],
                    r11f_explain_error(err)
                );
                r11f_class_cleanup(&classfile);
                fclose(fp);
                continue;
            }

            r11f_class_dump(stderr, argv[i], &classfile);
            r11f_class_cleanup(&classfile);
            fclose(fp);
        }
    }
    else if (argc == 2 && !strcmp(argv[1], "--drill")) {
        drill_main();
    }
    else {
        fprintf(
            stderr,
            "R11F: JVM bytecode disassembler and interpreter\n"
            "usage:\n"
            "    %s --dump <classfile>...\tdisassemble class files\n"
            "    %s --drill\trun drill tests\n",
            argv[0],
            argv[0]
        );
    }
}

void drill_main(void) {
    r11f_classmgr_t *mgr = r11f_classmgr_alloc();
    r11f_vm_t vm;
    vm.classmgr = mgr;
    vm.current_frame = NULL;

    int32_t output;
    fprintf(stderr, "r11f_vm_invoke(&vm, \"com/example/Add\", \"add\", \"(II)I\", 2, (uint32_t[]){114, 514}, &output)\n");
    r11f_error_t err = r11f_vm_invoke_static(
        &vm,
        "com/example/Add",
        "add",
        "(II)I",
        2,
        (uint32_t[]){114, 514},
        &output
    );

    if (err != R11F_success) {
        fprintf(stderr, "error: %s\n", r11f_explain_error(err));
        assert(0 && "failed to invoke method");
    }

    fprintf(stderr, "output: %d\n", output);

    r11f_memstat_t stat = r11f_memstat_get();
    fprintf(
        stderr,
        "heap_mem_used: %zu\n"
        "alloc_count: %zu\n"
        "dealloc_count: %zu\n"
        "fail_count: %zu\n",
        stat.heap_mem_used,
        stat.alloc_count,
        stat.dealloc_count,
        stat.fail_count
    );

    r11f_classmgr_free(mgr);

    stat = r11f_memstat_get();
    fprintf(
        stderr,
        "heap_mem_used: %zu\n"
        "alloc_count: %zu\n"
        "dealloc_count: %zu\n"
        "fail_count: %zu\n",
        stat.heap_mem_used,
        stat.alloc_count,
        stat.dealloc_count,
        stat.fail_count
    );
}
