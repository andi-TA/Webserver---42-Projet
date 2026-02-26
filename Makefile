NAME		=	webserv

RED     = \033[0;31m
GREEN   = \033[0;32m
YELLOW  = \033[0;33m
BLUE    = \033[0;34m
MAGENTA = \033[0;35m
CYAN    = \033[0;36m
RESET   = \033[0m

LIB			=	./server/lib/libserver.a \
				./request/lib/librequest.a \
				./response/lib/libresponse.a

INC			=	-I./interfaces \
				-I./request/include \
				-I./response/includes \
				-I./server/include

MAIN		=	main.cpp

CXX			=	c++ 

all			:	$(NAME)

help		:
				@echo "$(CYAN)Use the following commands:$(RESET)"
				@echo "$(CYAN)\tmake all:\tto create the executable$(RESET)"
				@echo "$(CYAN)\tmake clean:\tto remove all object files$(RESET)"
				@echo "$(CYAN)\tmake fclean:\tto remove all executable and library$(RESET)"
				@echo "$(CYAN)\tmake re:\tto remove everything and recreate the executable and library$(RESET)"
				@echo "$(CYAN)\tmake run:\tto recompile and launch the executable$(RESET)"

$(NAME)		:
				@mkdir -p ./server/obj ./request/obj ./response/obj
				@make --no-print-directory lib -C ./server/
				@make --no-print-directory lib -C ./request/
				@make --no-print-directory lib -C ./response/
				@$(CXX) $(MAIN) -Wl,--start-group $(LIB) -Wl,--end-group -o $@ $(INC)
				@echo "$(GREEN)Webserv compiled$(RESET)"

clean		:
				@rm -rf $(DIR)
				@make --no-print-directory clean -C ./server/
				@make --no-print-directory clean -C ./request/
				@make --no-print-directory clean -C ./response/
				@echo "$(YELLOW)Webserver object files removed$(RESET)"

fclean		:	clean
				@rm -rf $(NAME) ./etc/www/webserver/uploads/*
				@make --no-print-directory fclean -C ./server/
				@make --no-print-directory fclean -C ./request/
				@make --no-print-directory fclean -C ./response/
				@echo "$(RED)Webserver executable and library removed$(RESET)"

re			:	fclean all

.PHONY		:	all clean fclean re help