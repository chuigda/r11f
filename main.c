#include <error.h>
#include <stdio.h>
#include <string.h>
#include "clsfile.h"
#include "cfdump.h"

int main(int argc, char *argv[]) {
    if (argc >= 3 && !strcmp(argv[1], "--dump")) {
        for (int i = 2; i < argc; i++) {
            r11f_classfile_t classfile;
            memset(&classfile, 0, sizeof(r11f_classfile_t));

            FILE *fp = fopen(argv[i], "rb");
            if (!fp) {
                fprintf(stderr, "error: failed to open file %s\n", argv[i]);
                continue;
            }

            r11f_error_t err = r11f_classfile_read(fp, &classfile);
            if (err != R11F_ERR_none) {
                fprintf(
                    stderr,
                    "error: read file %s: %s\n",
                    argv[i],
                    r11f_explain_error(err)
                );
                r11f_classfile_cleanup(&classfile);
                fclose(fp);
                continue;
            }

            r11f_classfile_dump(argv[i], &classfile);
            r11f_classfile_cleanup(&classfile);
            fclose(fp);
        }
    }
    else {
        fprintf(stderr, "usage: %s --dump <classfile>...\n", argv[0]);
    }
}
