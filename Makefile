################################################################################
# common makefile
################################################################################
PRJ:=aug
SRC_EXT:=cpp


################################################################################
# directories
################################################################################
SRC_DIR:=$(CURDIR)/src
OBJ_DIR:=$(CURDIR)/obj
BIN_DIR:=$(CURDIR)/bin
LIB_DIR:=$(CURDIR)/lib
EXT_DIR:=$(CURDIR)/ext
OUT_DIR:=$(BIN_DIR)
# add for unit tests
#UNT_DIR:=$(CURDIR)/unit_tests
include $(EXT_DIR)/ext.mk


################################################################################
# file lists
################################################################################
SRC:=$(wildcard $(SRC_DIR)/*.$(SRC_EXT))
OBJ:=$(addprefix $(OBJ_DIR)/, $(notdir $(SRC)))
OBJ:=$(OBJ:.$(SRC_EXT)=.o)
INC:=-I$(GLM_DIR) -I$(GLLG_DIR) -I$(GLFW_DIR)/include
LIB:=


################################################################################
# tool options
################################################################################
CXX:=g++
LD:=$(CXX)
CXXFLAGS:=-Wall -std=c++11
LDFLAGS:=
CP:=cp
MKDIR:=mkdir
RM:=rm
.DEFAULT_GOAL:=all
.PHONY:=pre_build post_build all debug clean allclean


################################################################################
# OS specifics
################################################################################
ifeq ($(OS), Windows_NT)
	PRJ:=$(PRJ).exe
	LIB+=-lglfw3dll -lopengl32
	LDFLAGS+=-L$(GLFW_DIR)/lib/win32/lib-mingw
else
	uname:=$(shell uname -s)
	ifeq ($(uname), Linux)
		LIB+=-lglfw -lGL -lGLU
		LIB+=-lX11 -lpthread -lXrandr -lXi -lXxf86vm  -lm
		LDFLAGS+=-L$(GLFW_DIR)/lib
		LDFLAGS+=-Wl,-rpath=$(GLFW_DIR)/lib
	else ifeq ($(UNAME),Darwin)
		LIB+=-framework OpenGL
	endif
endif


################################################################################
# Targets
################################################################################
PRE_BUILD:=$(SRC_DIR)/$(GLLG_OUT).$(SRC_EXT)
POST_BUILD:=
all: $(EXT) pre_build $(OUT_DIR)/$(PRJ) post_build

debug: CXXFLAGS+=-ggdb -DDEBUG
debug: all
ifdef UNT_DIR
	@if [ -d $(UNT_DIR) ]; then cd $(UNT_DIR); make debug; fi
endif

$(SRC_DIR)/$(GLLG_OUT).$(SRC_EXT):
	$(CP) $(GLLG_DIR)/$(GLLG_OUT).$(SRC_EXT) $(SRC_DIR)

pre_build: $(PRE_BUILD)
	@echo "PRE BUILD"

$(OUT_DIR):
	$(MKDIR) -p $(OUT_DIR)

$(OUT_DIR)/$(PRJ): pre_build $(OBJ) $(OUT_DIR) 
	$(LD) -o $@ $(LDFLAGS) $(OBJ) $(LIB)

post_build: $(POST_BUILD)
	@echo "POST BUILD"
ifeq ($(OS), Windows_NT)
	$(CP) $(GLFW_DIR)/lib/win32/lib-mingw/glfw3.dll $(OUT_DIR)
endif

$(OBJ_DIR):
	$(MKDIR) -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT) $(OBJ_DIR)
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INC)

clean:
	$(RM) -rf $(OBJ_DIR)
	$(RM) -rf $(BIN_DIR)
	$(RM) -rf $(LIB_DIR)
ifdef UNT_DIR
	@if [ -d $(UNT_DIR) ]; then cd $(UNT_DIR); make clean; fi
endif

allclean: clean clean_ext
	$(RM) -f $(SRC_DIR)/$(GLLG_OUT).$(SRC_EXT)
