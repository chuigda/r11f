ifndef JAVAC
	JAVAC=javac
endif

JAVAC_FLAGS=--release 8

SOURCE_FILES = $(wildcard *.java)
CLASS_FILES = $(patsubst %.java,%.class,$(SOURCE_FILES))

define LOG
	printf '\t%s\t%s\n' $1 $2
endef

define COMPILE
	@$(call LOG,JAVAC,$1)
	@$(JAVAC) $(JAVAC_FLAGS) $1
endef

.PHONY: all classes-phony classes-log
all: classes-phony

classes-phony: classes-log $(CLASS_FILES)

classes-log:
	@echo Building classes

%.class: %.java
	$(call COMPILE,$<)

clean:
	@rm -f $(filter-out Object.class,$(CLASS_FILES))
