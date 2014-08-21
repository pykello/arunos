#include <kalloc.h>
#include <ksyscall.h>
#include <lib/base16.h>
#include <lib/string.h>
#include <proc.h>
#include <system.h>
#include <vm.h>

static void decode_process_image(const char **program, char ***image,
				 int *image_page_count);
static void free_process_image(char **image, int image_page_count);

extern const char *user_programs[][1024];

/* syscall_exec loads and runs the program with given index. */
int syscall_exec(int id)
{
	bool loaded = proc_load_program(current_process, id);
	if (loaded) {
		proc_start(current_process);
	}

	return -1;
}

/*
 * proc_load_program loads user_programs[program_index] into the
 * given process.
 */
bool proc_load_program(struct Process *proc, int program_index)
{
	bool loaded = false;
	char **process_image = NULL;
	int process_image_page_count = 0;

	decode_process_image(user_programs[program_index], &process_image,
			     &process_image_page_count);
	loaded = proc_load(proc, process_image, process_image_page_count);
	free_process_image(process_image, process_image_page_count);

	return loaded;
}

static void decode_process_image(const char **program, char ***image,
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
