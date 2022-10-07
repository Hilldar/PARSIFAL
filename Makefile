#
# Authors:
# Riccardo Farinelli <rfarinelli@fe.infn.it>
# Lia Lavezzi        <lia.lavezzi@to.infn.it>
#
# All rights reserved
# For the licensing terms see $PARSIFAL/LICENSE
#
SRC  = $(PARSIFAL)/src
LIB  = $(PARSIFAL)/lib
BIN  = $(PARSIFAL)/bin
LOG  = $(PARSIFAL)/log
DATA = $(PARSIFAL)/data
INC  = $(addprefix -I,$(SRC))

vpath %.C $(SRC)

CC      = g++
CCFLAGS = -c  -Wno-deprecated -g  #-std=c++11 -Wall 

LD      = g++
#LDFLAGS = -lGui -lTreePlayer -Wall -std=c++11 -fopenmp
LDFLAGS = -lGui -lTreePlayer -Wall#-w # -Wall  -w toglie i warning
# YOU SHOULD ACTIVE -Wall  every time !!!!

ROOTINCS = $(shell root-config --cflags) # will be used as it is
ROOTLIBS = $(shell root-config --glibs) -lMinuit  # will be used as it is
LIBS     = $(LIB)                        # will get an -L for each entry


EXEC2	     = Simulate
COMPONENTS2  = main/main_sim main/Parsifal Geometry/Geometry Particle/Particle Ionization/Ionization ElectronDrift/ElectronDrift DetectorGain/DetectorGain Readout/Readout Signal/Signal Reconstruction/Reconstruction
BIN_EXEC2    = $(addprefix $(BIN)/,$(EXEC2) )

EXEC3        = Analysis
COMPONENTS3  = main/main_ana
BIN_EXEC3    = $(addprefix $(BIN)/,$(EXEC3) )

default: all

# compile sources
$(LIB)/%.o: %.C
	@echo .
	@echo ... compiling source: $< to $@
	$(CC) $(CCFLAGS) $< $(ROOTINCS) $(INC) -o $@

# 2
$(BIN_EXEC2):
	$(LD) $(LDFLAGS) $^ $(ROOTLIBS) $(addprefix -L, $(LIBS)) -o $@
$(BIN)/$(EXEC2): $(addprefix $(LIB)/, $(addsuffix .o, $(COMPONENTS2) ) )

# 3
$(BIN_EXEC3):
	$(LD) $(LDFLAGS) $^ $(ROOTLIBS) $(addprefix -L, $(LIBS)) -o $@
$(BIN)/$(EXEC3): $(addprefix $(LIB)/, $(addsuffix .o, $(COMPONENTS3) ) )

.PHONY : clean
clean:
	@echo ... cleaning
	rm -rf $(LIB)
	rm -rf $(LIB)/*
	rm -rf $(BIN)

$(LIB):
	mkdir -p $(LIB)
	mkdir -p $(LIB)/Geometry
	mkdir -p $(LIB)/Particle
	mkdir -p $(LIB)/Ionization
	mkdir -p $(LIB)/ElectronDrift
	mkdir -p $(LIB)/DetectorGain
	mkdir -p $(LIB)/Readout
	mkdir -p $(LIB)/Reconstruction
	mkdir -p $(LIB)/Signal
	mkdir -p $(LIB)/main
$(BIN):
	mkdir -p $(BIN)
$(LOG):
	mkdir -p $(LOG)
$(DATA):
	mkdir -p $(DATA)
installdirs: $(LIB) $(BIN) $(LOG) $(DATA)
tum: installdirs $(BIN_EXEC2)
fush: installdirs $(BIN_EXEC3)
all: tum fush
