#include <syscall.h>

int 
main() { 
	syscall(__NR_dynamic_sys0, 0); 
}
