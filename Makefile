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


################################################################################
# file lists
################################################################################
SRC:=$(wildcard $(SRC_DIR)/*.$(SRC_EXT))
OBJ:=$(addprefix $(OBJ_DIR)/, $(notdir $(SRC)))
OBJ:=$(OBJ:.$(SRC_EXT)=.o)
INC:=-I$(EXT_DIR)/glm
LIB:=


################################################################################
# tool options
################################################################################
CXXFLAGS:=-Wall -std=c++11
LDFLAGS:=-Wl,--as-needed
LD:=g++
CP:=cp

################################################################################
# OS specifics
################################################################################
ifeq ($(OS), Windows_NT)
	LIB+=-lglew32 -lglfw3dll -lopengl32
	INC+=-I$(EXT_DIR)/glew/include -I$(EXT_DIR)/glfw/include
	LDFLAGS+=-L$(EXT_DIR)/glew/lib -L$(EXT_DIR)/glfw/win32/lib-mingw
else
	uname:=$(shell uname -s)
	ifeq ($(uname), Linux)
		GLFW_LIB:=-lX11 -lpthread -lXrandr -lXi -lXxf86vm  -lm
		LIB+=$(GLFW_LIB)
		LIB+=-lglfw3 -lGLEW -lGL -lGLU
	else ifeq ($(UNAME),Darwin)
		LIB+=-framework OpenGL
	endif
endif


################################################################################
# targets
################################################################################
all: $(OUT_DIR)/$(PRJ) post_build

debug: CXXFLAGS+=-ggdb -DDEBUG
debug: all
ifdef UNT_DIR
	@if [ -d $(UNT_DIR) ]; then cd $(UNT_DIR); make debug; fi
endif

$(OUT_DIR)/$(PRJ): $(OBJ) pre_build
	@mkdir -p $(OUT_DIR)
	$(LD) -o $@ $(LDFLAGS) $(OBJ) $(LIB)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(OBJ_DIR)
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INC)

clean:
	$(RM) -rf $(OBJ_DIR)
	$(RM) -rf $(BIN_DIR)
	$(RM) -rf $(LIB_DIR)
ifdef UNT_DIR
	@if [ -d $(UNT_DIR) ]; then cd $(UNT_DIR); make clean; fi
endif

pre_build:

post_build:
ifeq ($(OS), Windows_NT)
	$(CP) $(EXT_DIR)/glew/lib/glew32.dll $(OUT_DIR)
	$(CP) $(EXT_DIR)/glfw/win32/lib-mingw/glfw3.dll $(OUT_DIR)
endif
