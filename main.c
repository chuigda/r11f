#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clsfile.h"
#include "class.h"
#include "cfdump.h"
#include "clsmgr.h"
#include "error.h"

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

            r11f_class_dump(argv[i], &classfile);
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
    assert(mgr && "failed to allocate class manager");

    FILE *file = fopen("test/Add.class", "rb");
    assert(file && "failed to open file");

    r11f_class_t *classfile = calloc(1, sizeof(r11f_class_t));
    assert(classfile && "failed to allocate classfile");

    r11f_error_t err = r11f_classfile_read(file, classfile);
    assert(err == R11F_success && "failed to read classfile");

    uint32_t classid;
    err = r11f_classmgr_add_class(mgr, classfile, &classid);
    assert(err == R11F_success && "failed to add class");

    r11f_class_t *found = r11f_classmgr_find_class(mgr, "tech/icey/r11f/test/Add");
    assert(found && "failed to find class by name");

    found = r11f_classmgr_find_class_id(mgr, classid);
    assert(found && "failed to find class by id");

    r11f_classmgr_free(mgr);
    fclose(file);

    fprintf(stderr, "drill tests passed\n");
}
