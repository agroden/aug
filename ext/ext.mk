################################################################################
# directories
################################################################################
GLEW_DIR:=$(EXT_DIR)/glew
GLFW_DIR:=$(EXT_DIR)/glfw
GLM_DIR:=$(EXT_DIR)/glm

################################################################################
# file lists
################################################################################
GLEW_LIB:=libglew
GLFW_LIB:=libglfw3


################################################################################
# OS specifics
################################################################################
ifeq ($(OS), Windows_NT)
	GLEW_LIB+=.dll
	GLFW_LIB+=dll.a
else
	uname:=$(shell uname -s)
	ifeq ($(uname), Linux)
		GLEW_LIB:=libGLEW.so
		GLFW_LIB+=.so
	endif
endif


################################################################################
# Targets
################################################################################
EXT:=$(GLEW_DIR)/lib/$(GLEW_LIB)

# glew extensions
$(GLEW_DIR)/src/glew.c:
	cd $(GLEW_DIR) && make extensions

# glew lib
$(GLEW_DIR)/lib/$(GLEW_LIB): $(GLEW_DIR)/src/glew.c
	echo $@
	cd $(GLEW_DIR); make

clean_glew:
	cd $(GLEW_DIR)/auto && make destroy
	cd $(GLEW_DIR) && make clean

clean_ext: clean_glew