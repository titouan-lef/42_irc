#===================== BASE =====================#

CC				:=	c++
CFLAGS			+=	-Wall -Wextra -Werror -MP -MMD -std=c++98 -g
BUILD_DIR		:=	.build/
HEADERS			:=	./inc/
NAME			:=	ircserv

#==================== SOURCE ====================#

SRC_DIR			:=	./src/
SRC				:=	main Server Client User
SRC				:=	$(addsuffix .cpp, $(SRC))

#==================== OBJECT ====================#

OBJ				:=	$(SRC:%.cpp=$(BUILD_DIR)%.o)
OBJ_DIR			:=	$(sort $(shell dirname $(OBJ)))

#================== DEPENDENCY ==================#

DEP				:=	$(SRC:%.cpp=$(BUILD_DIR)%.d)

#==================== COLOR =====================#

INIT			:=	\e[0m
BLACK			:=	\e[30m
RED				:=	\e[31m
GREEN			:=	\e[32m
YELLOW			:=	\e[33m
BLUE			:=	\e[34m
MAGENTA			:=	\e[35m
CYAN			:=	\e[36m

#===================== RULE =====================#

DISPLAY_TRACKER	:=	.display_done

ifndef VERBOSE
.SILENT:
endif

all				: $(NAME)

bonus			: $(NAME)

$(DISPLAY_TRACKER):
				echo
				echo "      ::::::::  :::::::::  :::::::::   :::::::   ::::::::"
				echo "    :+:    :+: :+:    :+: :+:    :+: :+:   :+: :+:    :+:"
				echo "   +:+        +:+    +:+ +:+    +:+ +:+   +:+ +:+    +:+ "
				echo "  +#+        +#++:++#+  +#++:++#+  +#+   +:+  +#++:++#   "
				echo " +#+        +#+        +#+        +#+   +#+        +#+   "
				echo "#+#    #+# #+#        #+#        #+#   #+# #+#    #+#    "
				echo "########  ###        ###         #######   ########      "
				echo
				sleep 0.5
				touch $(DISPLAY_TRACKER)

$(NAME)			:	$(DISPLAY_TRACKER) $(OBJ)
				$(CC) -I$(HEADERS) $(CFLAGS) $(OBJ) -o $(NAME)
				echo "$(GREEN)$(NAME) ready ✅️$(INIT)"

$(BUILD_DIR)%.o	:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
				echo "$(GREEN)Compiling : $(MAGENTA)$<$(INIT)"
				$(CC) -I$(HEADERS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)		:
				mkdir -p $@

clean			:
				if [ -f $(NAME) ];\
				then echo "$(BLUE)Cleanning $(NAME) ✨🧹$(INIT)";\
				fi;
				if [ -d $(BUILD_DIR) ];\
				then echo "$(RED)Removing directory : $(BLUE)$(BUILD_DIR)$(INIT)";\
				rm -rf $(BUILD_DIR);\
				fi;
				rm -f $(DISPLAY_TRACKER)

fclean			:
				if [ -f $(NAME) ];\
				then echo "$(BLUE)Cleanning $(NAME) ✨🧹$(INIT)";\
				echo "$(RED)Removing : $(MAGENTA)$(NAME)$(INIT)";\
				rm -f $(NAME);\
				$(MAKE)	clean --no-print-directory;\
				fi;

re				:
				rm -f $(DISPLAY_TRACKER)
				$(MAKE) $(DISPLAY_TRACKER) --no-print-directory
				$(MAKE)	fclean --no-print-directory
				touch $(DISPLAY_TRACKER)
				$(MAKE) all --no-print-directory

.PHONY	=	all clean fclean re bonus

ifndef VERBOSE
.SILENT:
endif

-include $(DEP)
