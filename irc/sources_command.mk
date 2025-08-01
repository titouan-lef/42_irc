CMD_NAME := Join Kick List Mode Names Nick Part Pass Ping Pong Quit User Topic Invite Privmsg
CMD_FULLNAME := $(addsuffix Cmd, $(CMD_NAME))

SRC_CMD := CommandManager ACommand $(CMD_FULLNAME)

SRC += $(addprefix Commands/, $(SRC_CMD))