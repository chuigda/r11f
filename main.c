#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bytecode.h"
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
    else if (argc == 5 && !strcmp(argv[1], "--disassemble")) {
        r11f_class_t classfile;
        memset(&classfile, 0, sizeof(r11f_class_t));

        FILE *fp = fopen(argv[2], "rb");
        if (!fp) {
            fprintf(stderr, "error: failed to open file %s\n", argv[2]);
            return 1;
        }

        r11f_error_t err = r11f_classfile_read(fp, &classfile);
        fclose(fp);

        if (err != R11F_success) {
            fprintf(
                stderr,
                "error: read file %s: %s\n",
                argv[2],
                r11f_explain_error(err)
            );
            r11f_class_cleanup(&classfile);
            return 1;
        }

        r11f_method_info_t *method = r11f_class_resolve_method(
            &classfile,
            argv[3],
            strlen(argv[3]),
            argv[4],
            strlen(argv[4])
        );
        if (!method) {
            fprintf(
                stderr,
                "error: cannot resolve method %s%s\n",
                argv[3],
                argv[4]
            );
            r11f_class_cleanup(&classfile);
            return 1;
        }

        printf("disassembling %s%s\n", argv[3], argv[4]);
        r11f_disassemble(&classfile, method, 0);
        r11f_class_cleanup(&classfile);
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
            "    %s --disassemble <classfile> <method> <descriptor>\tdisassemble method\n"
            "    %s --drill\trun drill tests\n",
            argv[0],
            argv[0],
            argv[0]
        );
    }
}

void drill_main(void) {
    r11f_classmgr_t *mgr = r11f_classmgr_alloc();
    assert(mgr && "failed to allocate class manager");

    FILE *file = fopen("test/Add.class", "rb");
    assert(file && "failed to open file");

    r11f_class_t *classfile = calloc(1, sizeof(r11f_class_t));
    assert(classfile && "failed to allocate classfile");

    r11f_error_t err = r11f_classfile_read(file, classfile);
    assert(err == R11F_success && "failed to read classfile");

    fclose(file);

    uint32_t classid;
    err = r11f_classmgr_add_class(mgr, classfile, &classid);
    assert(err == R11F_success && "failed to add class");

    r11f_vm_t vm;
    vm.classmgr = mgr;
    err = r11f_vm_invoke(
        &vm,
        "tech/icey/r11f/test/Add",
        "add",
        "(II)I",
        2,
        (uint32_t[]){114, 514}
    );

    if (err != R11F_success) {
        fprintf(stderr, "error: %s\n", r11f_explain_error(err));
        assert(0 && "failed to invoke method");
    }

    r11f_classmgr_free(mgr);
}
