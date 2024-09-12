ifndef CC
	CC = gcc
endif

ifndef AR
	AR = ar
endif

ifndef RANLIB
	RANLIB = ranlib
endif

ifndef CFLAGS
	CFLAGS = -Wall -Wextra \
		-Wno-pedantic \
		-Wno-cast-function-type \
		-Wno-char-subscripts \
		-g \
		-DR11F_LITTLE_ENDIAN \
		$(EXTRA_CFLAGS)
endif

ifndef WIN32
	SHARED_LIB_NAME = libr11f.so
	EXECUTABLE_NAME = r11f.exe
else
	SHARED_LIB_NAME = libr11f.dll
	EXECUTABLE_NAME = r11f
endif

define LOG
	@printf '\t%s\t%s\n' $1 $2
endef

define COMPILE
	$(call LOG,CC,$1)
	@$(CC) $(CFLAGS) $1 \
		-Iconfig -I./include -I./src/include \
		-fPIC -c -o $2
endef

HEADER_FILES = $(wildcard include/*.h) $(wildcard src/include/*.h)
SOURCE_FILES = $(wildcard src/*.c)
OBJECT_FILES = $(patsubst src/%.c,%.o,$(SOURCE_FILES))

.PHONY: all
all: libr11f-phony r11f-phony

.PHONY: libr11f-phony libr11f-log
libr11f-phony: libr11f-log $(SHARED_LIB_NAME)

libr11f-log:
	@echo Building shared library $(SHARED_LIB_NAME)

$(SHARED_LIB_NAME): $(HEADER_FILES) $(OBJECT_FILES)
	$(call LOG,LINK,$@)
	@$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

.PHONY: r11f-phony r11f-log
r11f-phony: r11f-log $(EXECUTABLE_NAME)

r11f-log:
	@echo Building executable $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): main.o $(SHARED_LIB_NAME)
	$(call LOG,LINK,$@)
	@$(CC) $(CFLAGS) -L. -lr11f -o $@ $^

main.o: main.c $(HEADER_FILES)
	$(call COMPILE,$<,$@)

%.o: src/%.c $(HEADER_FILES)
	$(call COMPILE,$<,$@)

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.so
	rm -f *.dll
	rm -f *.exe
	rm -f r11f
