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

# glfw config makefile
$(GLFW_DIR)/Makefile:
	cd $(GLFW_LIB) && cmake . -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/home/agroden/code/glfw/lib -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_INSTALL=OFF

# glfw lib
$(GLFW_DIR)/lib/$(GLFW_LIB): $(GLFW_DIR)/Makefile
	@mkdir -p $(GLFW_DIR)/lib
	cd $(GLFW_DIR) && \
		make && \
		$(CP) $(GLFW_DIR)/src/$(GLFW_LIB) $(GLFW_DIR)/lib/$(GLFW_DIR)/lib

clean_glew:
	cd $(GLEW_DIR)/auto && make destroy
	cd $(GLEW_DIR) && make clean

clean_glfw:
	cd $(GLFW_DIR) && make clean
	$(RM) -rf $(GLFW_DIR)/*.cmake
	$(RM) -rf $(GLFW_DIR)/CMakeFiles

clean_ext: clean_glew clean_glfw