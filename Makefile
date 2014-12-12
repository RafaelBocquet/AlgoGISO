all:
	gcc -DNDEBUG -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c

opt:
	gcc -O2 -DNDEBUG -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c

debug:
	gcc -g -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c

debug_opt:
	gcc -DNDEBUG -g -O2 -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c
