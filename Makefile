all:
	gcc -DNDEBUG -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c graph.c

opt:
	gcc -O2 -DNDEBUG -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c graph.c

opt3:
	gcc -O3 -DNDEBUG -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c graph.c

debug:
	gcc -g -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c graph.c

debug_opt:
	gcc -DNDEBUG -g -O2 -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c graph.c

debug_opt3:
	gcc -DNDEBUG -g -O3 -std=c99 -W -Wall -Wextra util.c partition.c main.c array.c set.c wl_partition.c graph.c
