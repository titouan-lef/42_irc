#===================== BASE =====================#

CXX				:= c++
CXXFLAGS		+= -Wall -Wextra -Werror -MP -MMD -std=c++98 -g
MAKEFLAGS		+= --no-print-directory
BUILD_DIR		:= .build/
HEADERS			:= ./inc/
NAME			:= ircserv
DISPLAY_TRACKER	:= .display_done

#==================== SOURCE ====================#

SRC_DIR			:= ./src/
SRC				:= main Server Client User
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

all:	$(NAME)

$(NAME):	$(OBJ) $(DISPLAY_TRACKER)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "$(GREEN)$(NAME) ready ✅️$(INIT)"

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) -I$(HEADERS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "$(BLUE)Cleanning $(NAME) ✨🧹$(INIT)"
	@if [ -d $(BUILD_DIR) ]; then\
		echo "$(RED)Removing directory : $(BLUE)$(BUILD_DIR)$(INIT)";\
		rm -rf $(BUILD_DIR);\
	fi;
	@rm -f $(DISPLAY_TRACKER)

fclean:	clean
	@if [ -f $(NAME) ]; then\
		echo "$(RED)Removing : $(MAGENTA)$(NAME)$(INIT)";\
		rm -f $(NAME);\
	fi;

re:	fclean $(DISPLAY_TRACKER) all

$(DISPLAY_TRACKER):
	@echo
	@echo "***********************************************************************************"
	@echo "*                                                                                 *"
	@echo "*             _____                    _____                    _____             *"
	@echo "*            /\    \                  /\    \                  /\    \            *"
	@echo "*           /::\    \                /::\    \                /::\    \           *"
	@echo "*           \:::\    \              /::::\    \              /::::\    \          *"
	@echo "*            \:::\    \            /::::::\    \            /::::::\    \         *"
	@echo "*             \:::\    \          /:::/\:::\    \          /:::/\:::\    \        *"
	@echo "*              \:::\    \        /:::/__\:::\    \        /:::/  \:::\    \       *"
	@echo "*              /::::\    \      /::::\   \:::\    \      /:::/    \:::\    \      *"
	@echo "*     ____    /::::::\    \    /::::::\   \:::\    \    /:::/    / \:::\    \     *"
	@echo "*    /\   \  /:::/\:::\    \  /:::/\:::\   \:::\____\  /:::/    /   \:::\    \    *"
	@echo "*   /::\   \/:::/  \:::\____\/:::/  \:::\   \:::|    |/:::/____/     \:::\____\   *"
	@echo "*   \:::\  /:::/    \::/    /\::/   |::::\  /:::|____|\:::\    \      \::/    /   *"
	@echo "*    \:::\/:::/    / \/____/  \/____|:::::\/:::/    /  \:::\    \      \/____/    *"
	@echo "*     \::::::/    /                 |:::::::::/    /    \:::\    \                *"
	@echo "*      \::::/____/                  |::|\::::/    /      \:::\    \               *"
	@echo "*       \:::\    \                  |::| \::/____/        \:::\    \              *"
	@echo "*        \:::\    \                 |::|  ~|               \:::\    \             *"
	@echo "*         \:::\    \                |::|   |                \:::\    \            *"
	@echo "*          \:::\____\               \::|   |                 \:::\____\           *"
	@echo "*           \::/    /                \:|   |                  \::/    /           *"
	@echo "*            \/____/                  \|___|                   \/____/            *"
	@echo "*                                                                                 *"
	@echo "***********************************************************************************"
	@sleep 0.5
	@touch $(DISPLAY_TRACKER)

.PHONY	= all clean fclean re

-include $(DEP)
