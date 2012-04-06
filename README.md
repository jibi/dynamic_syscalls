dynamic_sys: add syscalls without rebooting 
=============

What is
-------
dynamic_sys is a linux kernel patch that allows you to add new syscalls via
lkm: just write a module with your syscall function and register it.

How does it work
-------
See sources.

Installation
-------
Yes, no diff files. You just have to:
* copy the linux directory in your linux source tree
* edit arch/x86/kernel/Makefile to add dynamic_sys to the list of objects
```
obj-y += dynamic_sys.o
```
* locate syscall table (position may change between different kernel versions)
  and add dynamic_sys entries. 
  For the lastest version (3.3 at the moment) you need to edit
	arch/x86/syscalls/syscall_32.tbl and add
```
349 i386  dynamic_sys0 sys_dynamic_sys0
350 i386  dynamic_sys1 sys_dynamic_sys1
351 i386  dynamic_sys2 sys_dynamic_sys2
352 i386  dynamic_sys3 sys_dynamic_sys3
```

Using
-------
See example.
Long story short: you need a module with your syscall and a call to add_dyn_sc.

```c
long my_syscall(void *arg1) {
	/* do a lot of stuff */
	return 0;
}

add_dyn_sc(my_syscall, NUMBER);
```

Note that dynamic_sys supports syscalls with up to 3 args, and every args must
be declared as 'void *'.

When you need to call from userspace your syscall you just call the syscall
wrapper that matches your syscall args number

```c
syscall(__NR_dynamic_sys1, NUMBER, arg);
```
