all:
	gcc -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c

opt:
	gcc -O2 -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c

debug:
	gcc -g -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c

debug_opt:
	gcc -DNDEBUG -g -O2 -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c
