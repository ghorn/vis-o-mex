PROJ = init_and_draw_simple_aircraft

Q ?= @

UNAME := $(shell uname)

AP_PROJECT_ROOT ?= ..

C_SRC = \
	init_and_draw_simple_aircraft.c

OBJ = $(C_SRC:.c=.o) 

WARNINGFLAGS ?= -Wall -Wextra -Werror
DEBUGFLAGS ?= -g -DDEBUG # -pg to generate profiling information

LDFLAGS ?= -lm

FEATURE_FLAGS = -DDT=0.01

#FEATURE_FLAGS +=-DUSE_SPEECH

ifeq ($(UNAME),Darwin)
	LDFLAGS += -L/opt/local/lib
	LDFLAGS += -L/usr/local/lib
	INCLUDES += -I/opt/local/include
	INCLUDES += -isystem /usr/local/include
	FEATUREFLAGS += -DOSX
else
	FEATUREFLAGS += -DLINUX
	OPTFLAGS += -O3 -march=native
endif

EXTOBJ ?= 


CONFTRON_DIR ?= $(AP_PROJECT_ROOT)/conftron
include $(CONFTRON_DIR)/includes
MATHLIB_DIR ?= $(AP_PROJECT_ROOT)/mathlib
include $(MATHLIB_DIR)/includes

CFLAGS ?= $(WARNINGFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(OPTFLAGS) $(FEATURE_FLAGS) -std=gnu99 
CC ?= gcc

.PHONY: clean settings 

$(PROJ): $(OBJ) $(HDR) ext
	@echo LD $@
	$(Q)$(CC) $(CFLAGS) $(OBJ) $(EXTOBJ) $(LDFLAGS) -o $@ 


ext:
	$(Q)$(MAKE) -C $(MATHLIB_DIR) 

%.o : %.c
	@echo CC $@
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

conftron:
	$(MAKE) -C $(CONFTRON_DIR)

conf: conftron
config: conftron

clean:
	rm -f $(PROJ)
	rm -f $(OBJ)

extclean: clean
	$(Q)$(MAKE) -C $(MATHLIB_DIR) clean

megaclean: extclean
	$(MAKE) -C $(CONFTRON_DIR) clean
