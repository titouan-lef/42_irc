#===================== BASE =====================#

CXX				:= c++
CXXFLAGS		+= -Wall -Wextra -Werror -MP -MMD -g
MAKEFLAGS		+= --no-print-directory
BUILD_DIR		:= .build/
IRCHEADERS		:= inc/ inc/IrcException/ inc/IrcException/ConcreteIrcException/ inc/struct/ inc/Commands/ inc/IrcException/ConcreteIrcException/NickException
IRCHEADERS		:= $(addprefix ../irc/, $(IRCHEADERS))

HEADERS			:= 
NAME			:= tester

#==================== SOURCE ====================#

SRC_DIR			:=
SRC				:= Tester runTest
SRC				:= $(addsuffix .cpp, $(SRC))

#==================== OBJECT ====================#

OBJ				:= $(SRC:%.cpp=$(BUILD_DIR)%.o)
OBJ_DIR			:= $(sort $(shell dirname $(OBJ)))

#================== DEPENDENCY ==================#

DEP				:= $(SRC:%.cpp=$(BUILD_DIR)%.d)

#==================== COLOR =====================#

INIT			:= \e[0m
BLACK			:= \e[30m
RED				:= \e[31m
GREEN			:= \e[32m
YELLOW			:= \e[33m
BLUE			:= \e[34m
MAGENTA			:= \e[35m
CYAN			:= \e[36m

#===================== RULE =====================#

all:	run

ircserv.a:
	@$(MAKE) tester -C ../irc >/dev/null
	@mv ../irc/ircserv.a ircserv.a
	@$(MAKE) fclean -C ../irc >/dev/null

ircserv:
	@$(MAKE) -C ../irc >/dev/null
	@mv ../irc/ircserv ircserv
	@$(MAKE) fclean -C ../irc >/dev/null

$(NAME):	ircserv.a ircserv $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) ircserv.a -o $@

run:	$(NAME)
	@./ircserv 4343 password > /dev/null 2>&1 & echo $$! > ircserv.pid
	@valgrind ./$(NAME) || true
	@pid=$$(cat ircserv.pid); if kill -0 $$pid 2>/dev/null; then kill $$pid; fi
	@rm -f ircserv.pid

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(HEADERS) $(IRCHEADERS)) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "$(BLUE)Cleanning $(NAME) ✨🧹$(INIT)"
	@if [ -d $(BUILD_DIR) ]; then\
		echo "$(RED)Removing directory : $(BLUE)$(BUILD_DIR)$(INIT)";\
		rm -rf $(BUILD_DIR);\
	fi;

fclean:	clean
	@if [ -f $(NAME) ]; then\
		echo "$(RED)Removing : $(MAGENTA)$(NAME)$(INIT)";\
		rm -f $(NAME);\
	fi;
	@rm -f ircserv.a
	@rm -f ircserv

re:	fclean $(DISPLAY_TRACKER) all

.PHONY	= all clean fclean re

-include $(DEP)
