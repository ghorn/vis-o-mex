PROJ = vis-o-mex

Q ?= @

UNAME := $(shell uname)

AP_PROJECT_ROOT ?= ..

C_SRC = \
	camera_manager.c \
	draw_text.c \
	drawing_functions.c \
	imagery_drawer.c \
	imagery_manager.c \
	imagery_prober.c \
	keyboard_mouse.c \
	object_manager.c \
	path_vis.c \
	simple_aircraft_vis.c \
	simple_model_vis.c \
	spline_utils.c \
	spline_trajectory_vis.c \
	spline_geometry_vis.c \
	talking_visualizer.c \
	tether_vis.c \
	visualizer.c \
	visualizer_telemetry.c \
	wind_vis.c

OBJ = $(C_SRC:.c=.o) 
WARNINGFLAGS ?= -Wall -Wextra -Werror -Wshadow
DEBUGFLAGS ?= -g -DDEBUG # -pg to generate profiling information

## Run with ATLAS if available; likely uses SSE2/3/4 to do matrix math
BLAS ?= `bash -c 'if gcc -latlas 2>&1 | grep -q "cannot find -latlas"; then echo \-lgslcblas; else echo \-lcblas \-latlas; fi'`

LDFLAGS ?= -lm -lgsl -lX11 -lglut -lGL -lGLU $(BLAS)

FEATURE_FLAGS = -DDT=\(3.0/100.0\)

#FEATURE_FLAGS +=-DUSE_SPEECH

OPTFLAGS = -O3

ifeq ($(UNAME),Darwin)
	LDFLAGS += -L/opt/local/lib
	LDFLAGS += -L/usr/local/lib
	INCLUDES += -I/opt/local/include
	INCLUDES += -isystem /usr/local/include
	FEATUREFLAGS += -DOSX
	LDFLAGS += -limlib2
else
	FEATUREFLAGS += -DLINUX
	LDFLAGS += -lImlib2
	OPTFLAGS += -march=native
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
