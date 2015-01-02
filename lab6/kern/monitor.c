
#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/dwarf.h>
#include <kern/kdebug.h>
#include <kern/dwarf_api.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{"backtrace", "Stack Backtrace",mon_backtrace}
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}
// lab1 exercise 

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	uint64_t rbp,rip,temp2;
	uint32_t argument,argument2,argument3;
        unsigned long long *temp;
	int return_value=0;
    // getting the arguments from the edi etc. registers
    // came to know this from the assembly code of the kernel.asm
        __asm __volatile("movq %%rbp,%0\n\t"
			 "movl %%edi,%1\n\t"
			 "movl %%esi,%2\n\t"
			 "movl %%edx,%3"
              : "=r" (rbp),"=r" (argument),"=r" (argument2), "=r" (argument3)::"cc","memory");
	read_rip(rip);
	uintptr_t ripPtr = rip;
	struct Ripdebuginfo info={"<unknown>",0,"<unknown>",9};
        return_value= debuginfo_rip(ripPtr, &info);
        int rip_offset = ripPtr-info.rip_fn_addr;
	cprintf("Stack backtrace:\n");
        cprintf("  rbp %016llx rip %016llx\n",rbp,rip);
	if(return_value==0)
	{
                cprintf("   %s:%d: %s+%016x args:%d %016x %016x %016x\n",info.rip_file,info.rip_line,info.rip_fn_name,rip_offset,info.rip_fn_narg,argument,argument2,argument3);
        }

	 __asm __volatile("movq 8(%0),%1\n\t"
                    : "=r"(rip):"0"(rbp):"cc","memory");
	 temp=(unsigned long long *)rbp;
     //the present rbp contains the location of the previous rbp
     // when you dereference it you can traverse up the stack
        while(*temp!=0x0)
        {
		temp2 = (unsigned long long)*temp;
		ripPtr = rip;
		return_value=debuginfo_rip(ripPtr, &info);
		rip_offset = ripPtr-info.rip_fn_addr;
                cprintf("  rbp %016llx rip %016llx\n",*temp,rip);
		  __asm __volatile("movl -4(%1),%0\n\t"
                          : "=r" (argument):"q"(temp2):"%rdi");
		 
		if(return_value==0)
		{		
			cprintf("   %s:%d: %s+%016x args:%d %016x\n",info.rip_file,info.rip_line,info.rip_fn_name,rip_offset,info.rip_fn_narg,argument);
		}
                 __asm __volatile("movq 8(%0),%1\n\t"
                          : "=r"(rip):"0"(temp2):"cc","memory");
		temp=(unsigned long long *)temp2;
        } 

	return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
