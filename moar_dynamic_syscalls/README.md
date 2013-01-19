moar_dynamic_sys: add syscalls without rebooting (and without recompiling)
=============

What is
-------
moar_dynamic_sys is a linux kernel module that allows you to add new syscalls
without rebooting or recompiling your kernel.

Just write your syscall, recompile the module and load it.

How does it work
-------
See sources (sysenter entry code patching, only x86_32).

Using
-------
Edit my_syscall function in mdynsc.c

```c
asmlinkage long
my_syscall(int val) {

/* ... */

	return 0;
}
```
compile and load the module


```sh
$ make
$ sudo insmod ./mdynsc.ko
```

and use your new syscall.

TODO
-------
Write something to autoadd syscalls to the table.

