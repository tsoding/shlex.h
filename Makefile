shlex: shlex.h
	cc -Wall -Wextra -o shlex -x c -DSHLEX_IMPLEMENTATION -DSHLEX_SELF_TEST shlex.h
