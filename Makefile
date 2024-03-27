include colors.mk
CFLAGS		:= -fdiagnostics-color=always -mavx -mavx2
SRC_DIR		:= Sources
INC_DIR		:= -I Includes/
OBJ_DIR		:= Objects
TARGET		:= mandelbrot
LIBS		:= -lSDL2

SOURCES     := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS     := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all:        dir  $(TARGET)

$(TARGET):  $(OBJECTS)
			@$(CXX) -o $@ $(CFLAGS) $(OBJECTS) $(LIBS)
			@echo "Linked   $(BOLD_GREEN)"$<"$(CLR_END) successfully!"

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
			@$(CXX) $(CFLAGS) $(INC_DIR) -c $< -o $@
			@echo "Compiled $(BOLD_GREEN)"$<"$(CLR_END) successfully!"
##@ General
.PHONY:		help
help:		## Display help
			@awk 'BEGIN {\
					FS = ":.*##";\
					printf "Mandelbrot program version 111";\
					printf "$(FONT_BOLD)\nUsage:\n$(CLR_END)make $(BOLD_BLUE)<target>$(CLR_END)\n"\
				}\
				/^[a-zA-Z_0-9-]+:.*?##/ {\
					printf "$(BOLD_BLUE)%-15s$(CLR_END)%s\n", $$1, $$2\
				}\
				/^##@/ {\
        			printf "\n$(FONT_BOLD)%s$(CLR_END)\n", substr($$0, 5)\
    			} '  Makefile

.PHONY:		run
run:		## Run program
			./$(TARGET)

##@ Development
.PHONY:     dir
dir:		## Сreate folder for object files
			@mkdir -p $(OBJ_DIR)

.PHONY:     clean
clean:		## Remove object files
			@rm -r $(OBJ_DIR)
			@echo "Object files cleanup complete!"
