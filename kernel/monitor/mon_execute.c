#include <debug.h>
#include <monitor.h>
#include <console.h>
#include <klib.h>
#include <lib/base16.h>
#include <lib/string.h>
#include <proc.h>
#include <kalloc.h>
#include <system.h>
#include <types.h>

static void load_process_image(const char **program, char ***image,
			       int *image_page_count);
static void free_process_image(char **image, int image_page_count);

extern const char *user_programs[][1024];

/* mon_execute. */
int mon_execute(int argc, char **argv)
{
	struct Process *proc = NULL;
	char **process_image = NULL;
	int process_image_page_count = 0;
	int program_index = 0;
	bool loaded = false;

	if (argc < 2) {
		kprintf("execute requires at least one argument.\n");
		return -1;
	}

	program_index = argv[1][0] - '0';
	load_process_image(user_programs[program_index], &process_image,
			   &process_image_page_count);

	proc = proc_create();
	loaded = proc_load(proc, process_image, process_image_page_count);
	free_process_image(process_image, process_image_page_count);

	if (loaded)
		proc_switch(proc);
	else {
		kprintf("couldn't load the process.\n");
	}

	return 0;
}

static void load_process_image(const char **program, char ***image,
			       int *image_page_count)
{
	int i = 0;
	(*image) = kalloc();

	for (i = 0; program[i]; i++) {
		int len = 0;
		(*image)[i] = kalloc();
		b16decode(program[i], strlen(program[i]),
			  (*image)[i], &len);

		(*image_page_count) = i + 1;
	}
}

static void free_process_image(char **image, int image_page_count)
{
	int i = 0;
	for (i = 0; i < image_page_count; i++)
		kfree(image[i]);
	kfree(image);
}
