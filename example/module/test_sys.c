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
#include <linux/syscalls.h>

#include <linux/dynamic_sys.h>

long my_syscall(void) {
	printk(KERN_INFO "My syscall\n");
	return 0;
}

static int __init
init (void)
{
	int ret;

	ret = add_dyn_sc(my_syscall, 0);
	if(ret == 0)
		printk(KERN_INFO "dynamic_sys: added new syscall.\n");
	else
		printk(KERN_INFO "dynamic_sys: fail (error: %d)\n", ret);

  return 0;
}

static void __exit
exit (void)
{
	del_dyn_sc(0);
}

module_init (init);
module_exit (exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("jcjh");
MODULE_DESCRIPTION ("test syscall");

