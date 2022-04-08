test-machina: proto_machina.c
	gcc -g -Wall -Wextra -Wpedantic $^ -o $@
