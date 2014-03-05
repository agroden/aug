################################################################################
# base vars
################################################################################
GL_SPEC:=gl
GL_PROFILE:=core
GL_MAJOR_VER:=3
GL_MINOR_VER:=3


################################################################################
# directories
################################################################################
GLFW_DIR:=$(EXT_DIR)/glfw
GLM_DIR:=$(EXT_DIR)/glm
GLLG_DIR:=$(EXT_DIR)/glLoadGen


################################################################################
# file lists
################################################################################
GLLG_OUT:=$(GL_SPEC)_$(GL_PROFILE)_$(GL_MAJOR_VER)_$(GL_MINOR_VER)
# set per OS
GLFW_LIB:=


################################################################################
# tool options
################################################################################
LUA:=lua
GLLG_FLAGS:=$(GL_PROFILE)_$(GL_MAJOR_VER)_$(GL_MINOR_VER) \
-spec=$(GL_SPEC) \
-style=pointer_cpp \
-version=$(GL_MAJOR_VER).$(GL_MINOR_VER) \
-profile=$(GL_PROFILE) \
-stdext=gl_ubiquitous.txt
-stdext=gl_core_post_3_3.txt


################################################################################
# OS specifics
################################################################################
ifeq ($(OS), Windows_NT)
	GLFW_LIB:=win64/lib-mingw/glfw3.dll
	#GLFW_CMAKE_FLAGS+=-G"MSYS Makefiles"
	#GLLG_FLAGS+=-spec=wgl -stdext=wgl_common.txt
	LUA:=lua.exe
else
	uname:=$(shell uname -s)
	ifeq ($(uname), Linux)
		GLFW_LIB:=libglfw.so
		#GLLG_FLAGS+=-spec=glx -stdext=glx_common.txt
	endif
endif


################################################################################
# targets
################################################################################
EXT:=$(OBJ_DIR)/$(GLLG_OUT).o

#$(SRC_DIR)/$(GLLG_OUT).$(SRC_EXT): $(GLLG_DIR)/$(GLLG_OUT).cpp
#	$(CP) $(GLLG_DIR)/$(GLLG_OUT).$(SRC_EXT) $(SRC_DIR)

$(OBJ_DIR)/$(GLLG_OUT).o: $(GLLG_DIR)/$(GLLG_OUT).cpp $(OBJ_DIR)
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INC)

# glLoadGen source file
$(GLLG_DIR)/$(GLLG_OUT).cpp: $(GLLG_DIR)/$(GLLG_OUT).hpp

# glLoadGen header
$(GLLG_DIR)/$(GLLG_OUT).hpp:
	cd $(GLLG_DIR) && $(LUA) LoadGen.lua $(GLLG_FLAGS)

# glLoadGen clean
clean_glLoadGen:
	$(RM) -rf $(GLLG_DIR)/$(GLLG_OUT).*

clean_ext: clean_glLoadGen