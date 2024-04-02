include colors.mk

CXX 		:=	clang++
DEDFLAGS	:=	-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal				\
				-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192	\
				-Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla		\
				-fexceptions -Wcast-qual -Wctor-dtor-privacy -Wempty-body -Wformat-security				\
				-Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers			\
				-Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe					\
				-Winline -Wunreachable-code -Wmissing-include-dirs

CXXFLAGS	=	-mavx -mavx2 -fsanitize=address -O3 -fsave-optimization-record

GCC_FLAGS			:=
GCC_DEBUG_FLAGS		:= -g3
CLANG_FLAGS			:= -stdlib=libc++
CLANG_DEBUG_FLAGS	:= -g

ifeq ($(CXX), g++)
	CXXFLAGS += $(GCC_FLAGS)
else ifeq ($(CXX), clang++)
	CXXFLAGS += $(CLANG_FLAGS)
endif

LDFLAGS		:= $(CXXFLAGS)
DEPFLAGS	 = -MMD -MP -MF $(OUT_NAME)
LDIBS		:= -lSDL2 -lSDL2_ttf

SRC_DIR	:= Sources
INC_DIR	:= -I Includes/

OBJ_DIR		:= objects
DEP_DIR		:= headers
TMP_DIRS	:= $(OBJ_DIR) $(DEP_DIR)

EXEC		:= mandelbrot
SOURCES		:= $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS		:= $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
IS_BUILT	:= 0 # Flag to indicate if a successful build has occurred

##@ General
.PHONY:		build
build: $(EXEC)	## Build project
			@if [ $(IS_BUILT) -eq 1 ] ; then											\
				printf "$(CLR_CYAN)Project built successfully!$(CLR_END)\n";			\
			else																		\
				printf "$(CLR_MAGENTA)Project already have been built!$(CLR_END)\n";	\
			fi

$(EXEC): $(OBJECTS)
			@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(LDIBS)
			$(eval IS_BUILT = 1)

$(OBJ_DIR)/%.o : BASENAME = $(basename $(notdir $@))
				 OUT_NAME = $(DEP_DIR)/$(BASENAME).d
$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(TMP_DIRS)
			@if $(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@ $(DEPFLAGS); then		\
				printf "Compiled $(BOLD_GREEN)"$<"$(CLR_END) successfully!\n";	\
			else																\
				printf "Compile $(CLR_RED)"$<"$(CLR_END) error!\n";				\
			fi

.PHONY: run
run:		## Run program
			./$(EXEC)

.PHONY: help
help:		## Display help
			@awk 'BEGIN {													\
					FS = ":.*##";											\
				}															\
				/^[a-zA-Z_0-9-]+:.*?##/ {									\
					printf "$(BOLD_BLUE)%-15s$(CLR_END)%s\n", $$1, $$2		\
				}															\
				/^##@/ {													\
					printf "\n$(FONT_BOLD)%s$(CLR_END)\n", substr($$0, 5)	\
				}															\
				END { printf "\n"; }'  Makefile

##@ Development

.PHONY: clean
clean:		## Remove binaries
			@if [ "$(wildcard $(TMP_DIRS))" -o -e $(EXEC) ]; then					\
				$(RM) -r $(OBJ_DIR);												\
				$(RM) -r $(DEP_DIR);												\
				$(RM) $(EXEC);														\
				printf "$(CLR_CYAN)Binaries removed!$(CLR_END)\n";					\
			else																	\
				printf "$(CLR_MAGENTA)Binaries already removed!$(CLR_END)\n";		\
			fi

.PHONY: debug
debug:		## Build debug version
debug:		GCC_FLAGS   += $(GCC_DEBUG_FLAGS)
debug:		CLANG_FLAGS += $(CLANG_DEBUG_FLAGS)
debug:		clean build

$(TMP_DIRS):
			@mkdir -p $@

-include $(OBJECTS:$(OBJ_DIR)/%.o=$(DEP_DIR)/%.d)
