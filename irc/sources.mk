SRC	:= main Server Client User IrcException UserManager Service MsgServ Channel ChanServ \
	$(addprefix Commands/, CommandManager JoinCmd KickCmd ModeCmd NickCmd PartCmd PingCmd PongCmd QuitCmd UserCmd TopicCmd InviteCmd PrivmsgCmd) \
	$(addprefix struct/, CommandResponse)