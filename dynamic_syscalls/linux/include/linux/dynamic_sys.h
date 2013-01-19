/*
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO. 
*/

#define DEFAULT_DYN_SYS_CALLS_LEN 8
#define MAX_DYN_SYS_CALLS_LEN 64

int add_dyn_sc(long (*sc)(void), unsigned int id);
int del_dyn_sc(unsigned int id);
