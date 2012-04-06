/*
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO. 
*/

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/module.h>

#include <linux/dynamic_sys.h>

long (**dyn_sys_calls)(void);
size_t dyn_sys_calls_len;
size_t max_dyn_sys_call_id;

static int resize_dyn_sys_calls(size_t len); 

SYSCALL_DEFINE1(dynamic_sys0, size_t, sc_id)
{
	long (*ptr)(void);
	if (sc_id >= dyn_sys_calls_len || dyn_sys_calls[sc_id] == NULL)
		return 0;

	ptr = dyn_sys_calls[sc_id];

	return ptr();
}

SYSCALL_DEFINE2(dynamic_sys1, unsigned int, sc_id, void *, arg1)
{
	long (*ptr)(void *);
	if (sc_id >= dyn_sys_calls_len || dyn_sys_calls[sc_id] == NULL)
		return 1;
	
	ptr = (long (*)(void *)) dyn_sys_calls[sc_id];

	return ptr(arg1);
}

SYSCALL_DEFINE3(dynamic_sys2, unsigned int, sc_id, void *, arg1, void*, arg2)
{
	long (*ptr)(void *, void *);
	if (sc_id >= dyn_sys_calls_len || dyn_sys_calls[sc_id] == NULL)
		return 0;

	ptr = (long (*)(void *, void *)) dyn_sys_calls[sc_id];

	return ptr(arg1, arg2);
}

SYSCALL_DEFINE4(dynamic_sys3, unsigned int, sc_id, void *, arg1, void*, arg2, void*, arg3)
{
	long (*ptr)(void *, void *, void*);
	if (sc_id >= dyn_sys_calls_len || dyn_sys_calls[sc_id] == NULL)
		return 0;

	ptr = (long (*)(void *, void *, void *)) dyn_sys_calls[sc_id];

	return ptr(arg1, arg2, arg3);
}

int add_dyn_sc(long (*sc)(void), unsigned int id)
{
	if (id >= MAX_DYN_SYS_CALLS_LEN)
		return 1; 
	if (id >= dyn_sys_calls_len && resize_dyn_sys_calls(id + 1))
		return 2;
	if (dyn_sys_calls[id] != NULL)
		return 3;

	printk(KERN_INFO "Adding new dynamic syscall (id %d)\n", id);
	dyn_sys_calls[id] = sc;
	
	if (id > max_dyn_sys_call_id)
		max_dyn_sys_call_id = id;

	return 0;
}
EXPORT_SYMBOL_GPL(add_dyn_sc);

int del_dyn_sc(unsigned int id) {
	if (id >= MAX_DYN_SYS_CALLS_LEN)
		return 1; 
	if (id >= dyn_sys_calls_len)
		return 2;
	if(dyn_sys_calls[id] == NULL)
		return 3;

	printk(KERN_INFO "Deleting dynamic syscall (id %d)\n", id);
	dyn_sys_calls[id] = NULL;

	return 0;
}
EXPORT_SYMBOL_GPL(del_dyn_sc);

static int resize_dyn_sys_calls(size_t len) {
	int i;

	long(**tmp)(void) = (long (**)(void)) 
		kmalloc(sizeof(long (*)(void)) * len, GFP_ATOMIC); 

	for(i = 0; i < len; i++)
		tmp[i] = NULL;

	for(i = 0; i < dyn_sys_calls_len; i++)
		tmp[i] = dyn_sys_calls[i]; 

	kfree(dyn_sys_calls);

	dyn_sys_calls = tmp;
	dyn_sys_calls_len = len;

	return 0;
}

static int __init
init_dynamic_sys(void) {
	int i;
	printk(KERN_INFO "Initializing dynamic syscalls");

	dyn_sys_calls_len = DEFAULT_DYN_SYS_CALLS_LEN;
	max_dyn_sys_call_id = 0;
	dyn_sys_calls = (long (**)(void)) 
		kmalloc(sizeof(long (*)(void)) * dyn_sys_calls_len, GFP_ATOMIC); 

	for(i = 0; i < dyn_sys_calls_len; i++)
		dyn_sys_calls[i] = NULL;

	return 0;
}

late_initcall(init_dynamic_sys);

