/*
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/syscall.h>

#include "mdynsc.h"

asmlinkage long
my_syscall(int val) {
	printk(KERN_INFO "LOLOL: %d", val);

	return 0;
}

static int __init
init (void) {
	if (!find_sysenter_sct_text_ptr()) {
		printk(KERN_INFO "Something went wrong "
			"(maybe your kernel does not use sysenter?)\n");

		return 1;
	}
	
	printk(KERN_INFO "sys_call_table address: 0x%p\n", *sct_ptr);
	printk(KERN_INFO "sys_call_table length: %d\n",*sct_len_ptr);

	memcpy(my_sys_call_table, *sct_ptr, *sct_len_ptr * sizeof(u32));
	my_sys_call_table_len = *sct_len_ptr;

	my_sys_call_table[*sct_len_ptr] = (u32) my_syscall;
	my_sys_call_table_len++;

	patch_sysenter_sct_text(my_sys_call_table, my_sys_call_table_len);

	return 0;
}

static void __exit
exit (void) {
	/* TODO: restore old syscall table */

}

module_init (init);
module_exit (exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jibi");
MODULE_DESCRIPTION("An easy way to test new syscalls");

