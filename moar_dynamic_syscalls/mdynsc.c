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

u32 *sct_len_ptr = NULL;
u32 **sct_ptr = NULL;

u32 my_sys_call_table[1024];
u32 my_sys_call_table_len;

#define WP_BIT (1 << 16)
#define BYTESCAN_LEN 1024

static void
set_wp(int val) {
	unsigned int cr0;

	cr0 = read_cr0();
	val ? (cr0 |= WP_BIT) : (cr0 &= ~WP_BIT);
	write_cr0(cr0);
}

asmlinkage long
my_syscall(int val) {
	printk(KERN_INFO "LOLOL: %d", val);

	return 0;
}

static int
find_sysenter_sct_text_ptr(void) {
	u32 sysenter_entry;
	u8 *ptr;
	unsigned int i;

	/* get from msr register sysenter_entry pointer */
	rdmsrl(MSR_IA32_SYSENTER_EIP, sysenter_entry);
	ptr = (char *) sysenter_entry;

	for (i = 0; i < BYTESCAN_LEN; i++) {

		/* scan for: cmpl $(NR_syscalls), %eax
		 * get: syscall table size */
		if (!memcmp(ptr + i, "\x3d", 1)) {
			sct_len_ptr = (u32 *)(ptr + i + 1);
			break;
		}
	}

	for (; i < BYTESCAN_LEN + 1; i++) {

		/*  scan for: call *sys_call_table(,%eax,4)
		 *  get: syscall table address */
		if (!memcmp(ptr + i, "\xff\x14\x85", 3)) {
			sct_ptr = (u32 **)(ptr + i + 3);
			break;
		}
	}

	return ((sct_ptr == NULL || sct_len_ptr == NULL) ? 0 : 1);
}

static void
patch_sysenter_sct_text(u32 *sct, u32 len) {
	asm volatile ("cli");
	set_wp(0);

	*sct_ptr = sct;
	*sct_len_ptr = len;

	set_wp(1);
	asm volatile ("sti");
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

