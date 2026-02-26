NAME		=	request

LIB			=	./lib/librequest.a

RED     = \033[0;31m
GREEN   = \033[0;32m
YELLOW  = \033[0;33m
BLUE    = \033[0;34m
MAGENTA = \033[0;35m
CYAN    = \033[0;36m
RESET   = \033[0m

INC			=	-I./include -I../interfaces -I../server/include -I../response/includes

MAIN		=	./src/main.cpp

SRC			=	./src/Request.cpp ./src/setRequest.cpp ./src/uRequest.cpp \
				./src/showRequest.cpp ./src/eRequest.cpp ./src/postRequest.cpp \
				./src/SessionManager.cpp

CXX			=	c++ -std=c++98 -Wall -Wextra -Werror

DIR			=	obj/

LIBDIR		=	./lib/

OBJ			=	$(patsubst %.cpp, $(DIR)/%.o, $(notdir $(SRC)))

$(DIR)%.o	: 	./src/%.cpp
			@mkdir -p $(dir $@)
			@$(CXX) -c $< -o $@ $(INC)

all			:
				@echo "$(CYAN)Use the following commands:$(RESET)"
				@echo "$(CYAN)\tmake exec:\tto create the test executable$(RESET)"
				@echo "$(CYAN)\tmake lib:\tto create the request library$(RESET)"
				@echo "$(CYAN)\tmake clean:\tto remove object files$(RESET)"
				@echo "$(CYAN)\tmake fclean:\tto remove the executable and library$(RESET)"
				@echo "$(CYAN)\tmake re:\tto remove everything and recreate the executable and library$(RESET)"
				@echo "$(CYAN)\tmake run:\tto recompile and launch the executable$(RESET)"

exec		:	$(NAME)

$(NAME)		:	$(LIB)
				@$(CXX) $(MAIN) $< -o $@ $(INC)
				@echo "$(GREEN)Request executable created$(RESET)"

lib			:	$(LIB)

$(LIB)		:	$(OBJ)
				@mkdir -p $(LIBDIR)
				@ar -rcs $@ $^
# 				@echo "$(GREEN)Request library created$(RESET)"

clean		:
				@rm -rf $(DIR)
				@echo "$(YELLOW)Request object files removed$(RESET)"

fclean		:	clean
				@rm -rf $(NAME) $(LIBDIR)
				@echo "$(RED)Request executable and library removed$(RESET)"

re			:	fclean exec
				@echo "$(GREEN)Request executable and library recreated$(RESET)"

run			:	re
				./$(NAME)

.PHONY		:	all clean fclean re