include colors.mk

DEDFLAGS	:= -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal			   \
			   -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 \
			   -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla      \
			   -fexceptions -Wcast-qual -Wctor-dtor-privacy -Wempty-body -Wformat-security             \
			   -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers            \
			   -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe                   \
		 	   -Winline -Wunreachable-code -Wmissing-include-dirs
CFLAGS		:= -fdiagnostics-color=always -mavx -mavx2 -MMD -MP
SRC_DIR		:= Sources
INC_DIR		:= -I Includes/
OBJ_DIR		:= Objects
EXEC		:= mandelbrot
LIBS		:= -lSDL2
SOURCES     := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS     := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
IS_BUILT    := 0 # Flag to indicate if a successful build has occurred

##@ General
.PHONY:		build
build:      $(OBJ_DIR) $(EXEC)	## Build project
			@if [ $(IS_BUILT) -eq 1 ] ; then                                     \
				echo "$(CLR_CYAN)Project built successfully!$(CLR_END)";         \
			else                                                           		 \
				echo "$(CLR_MAGENTA)Project already have been built!$(CLR_END)"; \
			fi

$(EXEC):    $(OBJECTS)
			@$(CXX) -o $@ $(OBJECTS) $(CFLAGS) $(LIBS)
			$(eval IS_BUILT = 1)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
			@if $(CXX) $(CFLAGS) $(INC_DIR) -c $< -o $@; then              \
				echo "Compiled $(BOLD_GREEN)"$<"$(CLR_END) successfully!"; \
			else                                                           \
				echo "Compile $(CLR_RED)"$<"$(CLR_END) error!";            \
			fi

.PHONY:		run
run:		## Run program
			./$(EXEC)

.PHONY:		help
help:		## Display help
			@awk 'BEGIN {                                                 \
					FS = ":.*##";                                         \
				}                                                         \
				/^[a-zA-Z_0-9-]+:.*?##/ {                                 \
					printf "$(BOLD_BLUE)%-15s$(CLR_END)%s\n", $$1, $$2    \
				}                                                         \
				/^##@/ {                                                  \
					printf "\n$(FONT_BOLD)%s$(CLR_END)\n", substr($$0, 5) \
				}                                                         \
				END { printf "\n"; }'  Makefile

##@ Development

.PHONY:     clean
clean:		## Remove binaries
			@if [ -d $(OBJ_DIR) -o -e $(EXEC) ]; then                     \
				$(RM) -r $(OBJ_DIR);                                      \
				$(RM) $(EXEC);                                            \
				echo "$(CLR_CYAN)Binaries removed!$(CLR_END)";            \
			else                                                          \
				echo "$(CLR_MAGENTA)Binaries already removed!$(CLR_END)"; \
			fi

$(OBJ_DIR):
			@mkdir -p $(OBJ_DIR)

# BAH: Read about this know i go to sleep!
-include $(OBJECTS:.o=.d)
