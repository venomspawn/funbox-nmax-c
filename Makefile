CC = gcc
CFLAGS = -c -Wall -O3
LDFLAGS = -L /usr/lib

DIR_SOURCE = source
DIR_HEADER = include
DIR_OUTPUT = output
DIR_MAKEFILE_INC = $(DIR_OUTPUT)

DIRS_PROJECTS = .
DIRS_SOURCES = $(addsuffix /$(DIR_SOURCE)/,$(DIRS_PROJECTS))
DIRS_HEADERS = $(addsuffix /$(DIR_HEADER)/,$(DIRS_PROJECTS))
DIRS_OUTPUTS = $(addsuffix /$(DIR_OUTPUT)/,$(DIRS_PROJECTS))

VPATH := $(DIRS_SOURCES) $(DIRS_HEADERS)

LOOKUP_C := $(addsuffix *.c,$(DIRS_SOURCES))

OBJECTS := $(notdir $(patsubst %.c,%.o,$(wildcard $(LOOKUP_C))))
OBJECT_FILES := $(addprefix $(DIR_OUTPUT)/,$(OBJECTS))

TARGET = nmax

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $@

%.o : %.c
	$(CC) -MD $(addprefix -I,$(DIRS_HEADERS)) $(CFLAGS) $< -o $(DIR_OUTPUT)/$@

include $(wildcard $(DIR_MAKEFILE_INC)/*.d)

.PHONY: clean
clean:
	rm -rf $(DIR_OUTPUT)/*o $(TARGET) $(DIR_MAKEFILE_INC)/*.d
