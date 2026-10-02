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

$(NAME):	$(DISPLAY_TRACKER) $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $@
	@printf "$(GREEN)$(NAME) ready ✅️$(INIT)\n"

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
	@printf "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)\n"
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(HEADERS)) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

tester:	$(OBJ)
	@ar rcs $(NAME).a $(OBJ)

bonus: $(BOT_NAME)

$(BOT_NAME): $(NAME) $(OBJ_BONUS)
	@$(CXX) $(CXXFLAGS) $(OBJ_BONUS) -o $(BOT_NAME)
	@printf "$(GREEN)$(BOT_NAME) ready ✅️$(INIT)\n"

$(BUILD_DIR)%.o:	$(BOT_DIR)%.cpp | $(OBJ_DIR)
	@printf "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)\n"
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(HEADERS)) -c $< -o $@

clean:
	@printf "$(BLUE)Cleanning $(NAME) ✨🧹$(INIT)\n"
	@if [ -d $(BUILD_DIR) ]; then\
		printf "$(RED)Removing directory : $(BLUE)$(BUILD_DIR)$(INIT)\n";\
		rm -rf $(BUILD_DIR);\
	fi;
	@rm -f $(DISPLAY_TRACKER)

fclean:	clean
	@if [ -f $(NAME) ]; then\
		printf "$(RED)Removing : $(MAGENTA)$(NAME)$(INIT)\n";\
		rm -f $(NAME);\
	fi;
	@if [ -f $(BOT_NAME) ]; then\
		printf "$(RED)Removing : $(MAGENTA)$(BOT_NAME)$(INIT)\n";\
		rm -f $(BOT_NAME);\
	fi;

re:	fclean $(DISPLAY_TRACKER) all

$(DISPLAY_TRACKER):
	@printf "\n"
	@printf "***********************************************************************************\n"
	@printf "*                                                                                 *\n"
	@printf "*             _____                    _____                    _____             *\n"
	@printf "*            /\    \                  /\    \                  /\    \            *\n"
	@printf "*           /::\    \                /::\    \                /::\    \           *\n"
	@printf "*           \:::\    \              /::::\    \              /::::\    \          *\n"
	@printf "*            \:::\    \            /::::::\    \            /::::::\    \         *\n"
	@printf "*             \:::\    \          /:::/\:::\    \          /:::/\:::\    \        *\n"
	@printf "*              \:::\    \        /:::/__\:::\    \        /:::/  \:::\    \       *\n"
	@printf "*              /::::\    \      /::::\   \:::\    \      /:::/    \:::\    \      *\n"
	@printf "*     ____    /::::::\    \    /::::::\   \:::\    \    /:::/    / \:::\    \     *\n"
	@printf "*    /\   \  /:::/\:::\    \  /:::/\:::\   \:::\____\  /:::/    /   \:::\    \    *\n"
	@printf "*   /::\   \/:::/  \:::\____\/:::/  \:::\   \:::|    |/:::/____/     \:::\____\   *\n"
	@printf "*   \:::\  /:::/    \::/    /\::/   |::::\  /:::|____|\:::\    \      \::/    /   *\n"
	@printf "*    \:::\/:::/    / \/____/  \/____|:::::\/:::/    /  \:::\    \      \/____/    *\n"
	@printf "*     \::::::/    /                 |:::::::::/    /    \:::\    \                *\n"
	@printf "*      \::::/____/                  |::|\::::/    /      \:::\    \               *\n"
	@printf "*       \:::\    \                  |::| \::/____/        \:::\    \              *\n"
	@printf "*        \:::\    \                 |::|  ~|               \:::\    \             *\n"
	@printf "*         \:::\    \                |::|   |                \:::\    \            *\n"
	@printf "*          \:::\____\               \::|   |                 \:::\____\           *\n"
	@printf "*           \::/    /                \:|   |                  \::/    /           *\n"
	@printf "*            \/____/                  \|___|                   \/____/            *\n"
	@printf "*                                                                                 *\n"
	@printf "***********************************************************************************\n"
	@sleep 0.5
	@touch $(DISPLAY_TRACKER)

.PHONY: all clean fclean re bonus tester

-include $(DEP)
