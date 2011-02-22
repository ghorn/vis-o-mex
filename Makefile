PROJ = vis-o-mex

Q ?= @

UNAME := $(shell uname)

AP_PROJECT_ROOT ?= ..
MATHLIB_PATH ?= $(AP_PROJECT_ROOT)/mathlib

C_SRC = \
	aircraft_vis.c \
	camera_manager.c \
	draw_text.c \
	drawing_functions.c \
	imagery_drawer.c \
	imagery_manager.c \
	imagery_prober.c \
	keyboard_mouse.c \
	simple_model_vis.c \
	spline_utils.c \
	spline_trajectory_vis.c \
	spline_geometry_vis.c \
	talking_visualizer.c \
	tether_vis.c \
	visualizer.c \
	visualizer_telemetry.c \
	wind_vis.c \
	$(MATHLIB_PATH)/spatial_rotations.c \
	$(MATHLIB_PATH)/quat.c \
	$(MATHLIB_PATH)/xyz.c

OBJ = $(C_SRC:.c=.o) 

WARNINGFLAGS ?= -Wall -Wextra -Werror
DEBUGFLAGS ?= -g -DDEBUG # -pg to generate profiling information

INCLUDES = \
	-I$(MATHLIB_PATH) \

## Run with ATLAS if available; likely uses SSE2/3/4 to do matrix math
BLAS ?= `bash -c 'if gcc -latlas 2>&1 | grep -q "cannot find -latlas"; then echo \-lgslcblas; else echo \-lcblas \-latlas; fi'`

LDFLAGS ?= -lm -lgsl -lX11 -lglut -lGL -lGLU $(BLAS)

FEATURE_FLAGS = -DDT=\(3.0/100.0\)

#FEATURE_FLAGS +=-DUSE_SPEECH

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
	OPTFLAGS += -O3 -march=native
endif

EXTOBJ ?= 
AUTOBUILD_DIR ?= $(AP_PROJECT_ROOT)/autobuild
include $(AUTOBUILD_DIR)/includes

CFLAGS ?= $(WARNINGFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(OPTFLAGS) $(FEATURE_FLAGS) -std=gnu99 
CC ?= gcc

.PHONY: clean settings 

$(PROJ): $(OBJ) $(HDR)
	@echo LD $@
	$(Q)$(CC) $(CFLAGS) $(OBJ) $(EXTOBJ) $(LDFLAGS) -o $@ 

%.o : %.c
	@echo CC $@
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

autobuild:
	$(MAKE) -C $(AUTOBUILD_DIR)

clean:
	rm -f $(PROJ)
	rm -f $(OBJ)

megaclean: clean
	$(MAKE) -C $(AUTOBUILD_DIR) clean

