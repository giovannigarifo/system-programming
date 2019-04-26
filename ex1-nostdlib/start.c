/*-------------------------------------------*/
/* A C program that doesn't start with main */
/*-----------------------------------------*/
// to be compiled with `gcc -o start -nostdlib start.c`

int main();

// entry point of the program
void _start(){

	int ret = main();
}

int main(){
	return 0;
}

