include colors.mk
DEDFLAGS	:= -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal			   \
			   -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 \
			   -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla      \
			   -fexceptions -Wcast-qual -Wctor-dtor-privacy -Wempty-body -Wformat-security             \
			   -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers            \
			   -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe                   \
		 	   -Winline -Wunreachable-code -Wmissing-include-dirs
CFLAGS		:= -fdiagnostics-color=always -mavx -mavx2
SRC_DIR		:= Sources
INC_DIR		:= -I Includes/
OBJ_DIR		:= Objects
EXEC		:= mandelbrot
LIBS		:= -lSDL2

SOURCES     := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS     := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all:        dir  $(EXEC)

$(EXEC):    $(OBJECTS)
			@if $(CXX) -o $@ $(CFLAGS) $(OBJECTS) $(LIBS); then\
				echo "Compiled $(BOLD_GREEN)successfully!$(CLR_END)";           \
			else                                                                \
				echo "Compile $(CLR_RED)error!$(CLR_END)";                      \
			fi

# @echo "Linking $(CLR_GREEN)succeeded$(CLR_END)!"

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
			@if $(CXX) $(CFLAGS) $(INC_DIR) -c $< -o $@; then              \
				echo "Compiled $(BOLD_GREEN)"$<"$(CLR_END) successfully!"; \
			else                                                           \
				echo "Compile $(CLR_RED)"$<"$(CLR_END) error!";            \
			fi

##@ General
.PHONY:		help
help:		## Display help
			@awk 'BEGIN {                                                                          \
					FS = ":.*##";                                                                  \
					printf "$(FONT_BOLD)\nUsage:\n$(CLR_END)make $(BOLD_BLUE)<target>$(CLR_END)\n" \
				}                                                                                  \
				/^[a-zA-Z_0-9-]+:.*?##/ {                                                          \
					printf "$(BOLD_BLUE)%-15s$(CLR_END)%s\n", $$1, $$2                             \
				}                                                                                  \
				/^##@/ {                                                                           \
					printf "\n$(FONT_BOLD)%s$(CLR_END)\n", substr($$0, 5)                          \
				} '  Makefile

.PHONY:		run
run:		## Run program
			./$(EXEC)

##@ Development
.PHONY:     dir
dir:		## Сreate folder for object files
			@mkdir -p $(OBJ_DIR)

# BAH: is it normal to use -f especially in this case?
.PHONY:     clean
clean:		## Remove binaries
			@if [ -d $(OBJ_DIR) -o -e $(EXEC) ]; then                     \
				rm -rf $(OBJ_DIR);                                        \
				rm -f $(EXEC);                                            \
				echo "$(CLR_CYAN)Binaries remove complete!$(CLR_END)";    \
			else                                                          \
				echo "$(CLR_MAGENTA)Binaries already removed!$(CLR_END)"; \
			fi
# $(EXEC)
