#===================== BASE =====================#

CXX				:= c++
CXXFLAGS		+= -Wall -Wextra -Werror -MP -MMD -std=c++98 -g
MAKEFLAGS		+= --no-print-directory
BUILD_DIR		:= .build/
HEADERS			:= inc/ inc/IrcException/ inc/IrcException/ConcreteIrcException/ inc/struct/ inc/Commands/ inc/IrcException/ConcreteIrcException/NickException
NAME			:= ircserv
DISPLAY_TRACKER	:= .display_done

#==================== SOURCE ====================#

SRC_DIR			:= ./src/
include			sources.mk
SRC				:= $(addsuffix .cpp, $(SRC))

#==================== BONUS =====================#

BOT_NAME		:= weatherBot
BOT_DIR			:= ./bot/
SRC_BOT			:= bot_main connexionUtils WeatherBot
SRC_BOT			:= $(addsuffix .cpp, $(SRC_BOT))

#==================== OBJECT ====================#

OBJ				:= $(SRC:%.cpp=$(BUILD_DIR)%.o)
OBJ_BONUS		:= $(SRC_BOT:%.cpp=$(BUILD_DIR)%.o)
OBJ_DIR			:= $(sort $(shell dirname $(OBJ)) $(shell dirname $(OBJ_BONUS)))

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

$(NAME):	$(DISPLAY_TRACKER) $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $@
	@echo "$(GREEN)$(NAME) ready ✅️$(INIT)"

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(HEADERS)) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

tester:	$(OBJ)
	@ar rcs $(NAME).a $(OBJ)

bonus: $(BOT_NAME)

$(BOT_NAME): $(NAME) $(OBJ_BONUS)
	@$(CXX) $(CXXFLAGS) $(OBJ_BONUS) -o $(BOT_NAME)
	@echo "$(GREEN)$(BOT_NAME) ready ✅️$(INIT)"

$(BUILD_DIR)%.o:	$(BOT_DIR)%.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(HEADERS)) -c $< -o $@

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
	@if [ -f $(BOT_NAME) ]; then\
		echo "$(RED)Removing : $(MAGENTA)$(BOT_NAME)$(INIT)";\
		rm -f $(BOT_NAME);\
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

.PHONY: all clean fclean re bonus

-include $(DEP)
