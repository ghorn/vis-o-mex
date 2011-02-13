PROJ = vis-o-mex

Q ?= @
CC ?= gcc

UNAME := $(shell uname)

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
	spline_trajectory_vis.c \
	spline_geometry_vis.c \
	talking_visualizer.c \
	tether_vis.c \
	visualizer.c \
	visualizer_telemetry.c \
	wind_vis.c \
	$(MATHLIBPATH)/spatial_rotations.c \
	$(MATHLIBPATH)/quat.c \
	$(MATHLIBPATH)/xyz.c \
	$(LCMPATH)/../controller/path_follower/traj_spline_data_loader.c

OBJ = $(C_SRC:.c=.o) 

WARNINGFLAGS ?= -Wall -Wextra -Werror
DEBUGFLAGS ?= -g -DDEBUG # -pg to generate profiling information

INCLUDES = \
	-I$(MATHLIBPATH) \
	-I$(LCMPATH) \

## Run with ATLAS if available; likely uses SSE2/3/4 to do matrix math
BLAS ?= `bash -c 'if gcc -latlas 2>&1 | grep -q "cannot find -latlas"; then echo \-lgslcblas; else echo \-lcblas \-latlas; fi'`

LDFLAGS ?= -lm -lgsl -lX11 -lglut -lGL -lGLU -llcm $(BLAS)

FEATURE_FLAGS =

ifeq ($(UNAME),Darwin)
	LDFLAGS += -L/opt/local/lib
	LDFLAGS += -L/usr/local/lib
	INCLUDES += -I/opt/local/include
	INCLUDES += -isystem /usr/local/include
	FEATUREFLAGS += -DOSX
	LDFLAGS += -limlib2
else
	CFLAGS += -DLINUX
	LDFLAGS += -lImlib2
endif

CFLAGS ?= $(WARNINGFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(OPTFLAGS) $(FEATURE_FLAGS) -std=gnu99 


.PHONY: clean settings 

$(PROJ): $(OBJ) $(HDR)
	@echo LD $@
	$(Q)$(CC) $(LDFLAGS) $(OBJ) `ls $(LCMPATH)/*.o` -o $@

%.o : %.c
	@echo CC $@
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

lcm: 
	$(MAKE) -C $(LCMPATH) gen
	$(MAKE) -C $(LCMPATH)

clean:
	rm -f $(PROJ)
	rm -f $(OBJ)

megaclean: clean
	$(MAKE) -C $(LCMPATH) clean

