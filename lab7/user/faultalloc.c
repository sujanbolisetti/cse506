// test user-level fault handler -- alloc pages to fix faults

#include <inc/lib.h>

void
handler(struct UTrapframe *utf)
{
	int r;
	void *addr = (void*)utf->utf_fault_va;
	cprintf("Divide by Zero\n");
	exit();
	//cprintf("fault %x\n", addr);
	//if ((r = sys_page_alloc(0, ROUNDDOWN(addr, PGSIZE),
	//			PTE_P|PTE_U|PTE_W)) < 0)
	//	panic("allocating at %x in page fault handler: %e", addr, r);
	//snprintf((char*) addr, 100, "this string was faulted in at %x", addr);
}

void
umain(int argc, char **argv)
{
	// we can reuse the current page fault handler set up only but in the trap we will catch the trap separately.
	set_pgfault_handler(handler);
	//set_proc_exep_handler(handler);
	int a,b;
	a=1;
	b=0;
	//asm volatile("int $3");
	int x=0;
	int c = a/b;
	//cprintf("%s\n", (char*)0xDeadBeef);
	//cprintf("%s\n", (char*)0xCafeBffe);
	
	/*
	 envid_t childEnv=0;
        childEnv = fork();
        cprintf("came inside faultalloc\n");
        if(childEnv < 0)
                cprintf("fork has failed\n");
        if(childEnv == 0)
        {
                exec("/home/cse506/lab7/hw-aupasana/obj/user/pingpong");
                //return 0;
        }
        else
                cprintf("hope child have executed succesfully\n");
*/
}

/*

#include <inc/lib.h>

void
handler(struct UTrapframe *utf)
{
        int r;
        //void *addr = (void*)utf->utf_fault_va;

        cprintf("Divide By Zero");
        //if ((r = sys_page_alloc(0, ROUNDDOWN(addr, PGSIZE),
          //                   PTE_P|PTE_U|PTE_W)) < 0)
         //      panic("allocating at %x in page fault handler: %e", addr, r);
        //snprintf((char*) addr, 100, "this string was faulted in at %x", addr);
}

void
umain(int argc, char **argv)
{
        set_proc_exep_handler(handler);
        int a,b;
        a=0;
        b=1;
        int c = b/a;
        //cprintf("%s\n", (char*)0xDeadBeef);
        //cprintf("%s\n", (char*)0xCafeBffe);
}
*/
