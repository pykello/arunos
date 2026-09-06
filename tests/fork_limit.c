#include <lib/stdio.h>
#include <lib/syscall.h>

void _start(void)
{
	int i, children[9];

	for (i = 0; i < 9; i++) {
		children[i] = fork();
		if (children[i] == 0) {
			/* Keep all slots occupied until the parent hits the limit. */
			while (getch() != 'x') yield();
			exit(0);
		}
		if (children[i] < 0) {
			printf("FAIL: early fork failure\n");
			while (1) yield();
		}
	}
	if (fork() != -1) {
		printf("FAIL: fork limit\n");
		while (1) yield();
	}
	printf("fork limit returned -1\n");
	for (i = 0; i < 9; i++) wait(children[i]);
	i = fork();
	if (i == 0) exit(0);
	if (i < 0) {
		printf("FAIL: fork after freeing slots\n");
		while (1) yield();
	}
	wait(i);
	printf("fork slots reusable\n");
	while (1) yield();
}
