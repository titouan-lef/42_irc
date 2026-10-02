#===================== BASE =====================#

CXX				:= c++
CXXFLAGS		+= -Wall -Wextra -Werror -MP -MMD -g
MAKEFLAGS		+= --no-print-directory
BUILD_DIR		:= .build/
IRC_FOLDER		:= ../irc/
IRCHEADERS		:= inc/ inc/IrcException/ inc/IrcException/ConcreteIrcException/ inc/struct/ inc/Commands/ inc/IrcException/ConcreteIrcException/NickException
IRCHEADERS		:= $(addprefix $(IRC_FOLDER), $(IRCHEADERS))

HEADERS			:= ./
NAME			:= tester

FILE_PID		:= ircserv.pid
IRC_EXE			:= ircserv
IRC_ARCHIVE		:= ircserv.a

#==================== SOURCE ====================#

SRC_DIR			:= ./
SRC				:= Tester runTest
SRC				:= $(addsuffix .cpp, $(SRC))

#==================== OBJECT ====================#

OBJ				:= $(SRC:%.cpp=$(BUILD_DIR)%.o)
OBJ_DIR			:= $(sort $(shell dirname $(OBJ)))

#================== DEPENDENCY ==================#

DEP				:= $(SRC:%.cpp=$(BUILD_DIR)%.d)

#==================== COLOR =====================#

INIT			:= \033[0m
BLACK			:= \033[30m
RED				:= \033[31m
GREEN			:= \033[32m
YELLOW			:= \033[33m
BLUE			:= \033[34m
MAGENTA			:= \033[35m
CYAN			:= \033[36m

#===================== RULE =====================#

all:	$(NAME)
	@./$(IRC_EXE) 4343 password > /dev/null 2>&1 & echo $$! > $(FILE_PID)
	@valgrind --leak-check=full --track-fds=yes ./$(NAME) 4343 password || true
	@pid=$$(cat $(FILE_PID)); if kill -0 $$pid 2>/dev/null; then kill $$pid; fi
	@rm -f $(FILE_PID)

$(NAME):	$(IRC_ARCHIVE) $(IRC_EXE) $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) $(IRC_ARCHIVE) -o $@

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@printf "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)\n"
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(HEADERS) $(IRCHEADERS)) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

$(IRC_ARCHIVE):
	@printf "$(YELLOW)Creation of the archive...$(INIT)\n"
	@$(MAKE) tester -C $(IRC_FOLDER) >/dev/null
	@mv $(IRC_FOLDER)/$(IRC_ARCHIVE) $(IRC_ARCHIVE)
	@$(MAKE) fclean -C $(IRC_FOLDER) >/dev/null

$(IRC_EXE):
	@printf "$(YELLOW)Creation of the server...$(INIT)\n"
	@$(MAKE) -C $(IRC_FOLDER) >/dev/null
	@mv $(IRC_FOLDER)/$(IRC_EXE) $(IRC_EXE)
	@$(MAKE) fclean -C $(IRC_FOLDER) >/dev/null

clean:
	@printf "$(BLUE)Cleanning $(NAME) ✨🧹$(INIT)\n"
	@if [ -d $(BUILD_DIR) ]; then\
		printf "$(RED)Removing directory : $(BLUE)$(BUILD_DIR)$(INIT)\n";\
		rm -rf $(BUILD_DIR);\
	fi;

fclean:	clean
	@if [ -f $(NAME) ]; then\
		printf "$(RED)Removing : $(MAGENTA)$(NAME)$(INIT)\n";\
		rm -f $(NAME);\
	fi;
	@rm -f $(IRC_ARCHIVE)
	@rm -f $(IRC_EXE)

re:	fclean $(DISPLAY_TRACKER) all

.PHONY	= all clean fclean re

-include $(DEP)
