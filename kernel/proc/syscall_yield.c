#include <ksyscall.h>
#include <proc.h>
#include <system.h>

int syscall_yield(void)
{
	schedule();
	return 0;
}
