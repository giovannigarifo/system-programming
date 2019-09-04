/*-------------------------------------------*/
/* A C program that doesn't start with main */
/*-----------------------------------------*/
// to be compiled with `gcc -o start -nostdlib start.c`

/* this codes works on 64 bit linux. */

#define SYSCALL_EXIT 60
#define SYSCALL_WRITE 1

int main(void);
void sys_exit(int error_code);
int sys_write(unsigned fd, const char *buf, unsigned count);

// entry point of the program
void _start(){

	int ret = main();
	sys_exit(12);
}

int main(){
	const char hwText[] = "Hello world!\n";
	sys_write(1, hwText, sizeof(hwText));
	return 0;
}

void sys_exit(int error_code)
{
    asm volatile
    (
        "syscall"
        : 
        : "a"(SYSCALL_EXIT), "D"(error_code)
        : "rcx", "r11", "memory"
    );
}

int sys_write(unsigned fd, const char *buf, unsigned count)
{
    unsigned ret;

    asm volatile
    (
        "syscall"
        : "=a"(ret)
        : "a"(SYSCALL_WRITE), "D"(fd), "S"(buf), "d"(count)
        : "rcx", "r11", "memory"
    );

    return ret;
}
