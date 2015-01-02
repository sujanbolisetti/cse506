// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	if(!(err & FEC_WR))
	{
		panic("Sorry no write access\n");
	}	

	if(!(uvpt[VPN(addr)] & PTE_COW))
	{
		panic("COW Bit was not set\n");
	}
	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	//   No need to explicitly delete the old page's mapping.

	// LAB 4: Your code here.
	r=sys_page_alloc(0, (void *)PFTEMP, PTE_P|PTE_U|PTE_W);
        if (r < 0)
                panic("error",r);
        
	memcpy(PFTEMP, (void *) ROUNDDOWN(addr, PGSIZE), PGSIZE);
	void* tempPtr = (void *) ROUNDDOWN(addr, PGSIZE);
        r = sys_page_map(0, (void *)PFTEMP,0,tempPtr, PTE_P|PTE_U|PTE_W);
        if (r < 0)
                panic("error",r);
        r=sys_page_unmap(0, (void *)PFTEMP);
        if (r < 0)
                panic("error",r);

}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	
	int pageOffset;
        envid_t parentEnvid = thisenv->env_id;
	
        uintptr_t va = pn*PGSIZE;
	int res;	
        pte_t* pgtentry = (pte_t *)&(uvpt[VPN(va)]);
        //pageOffset = ((*ptentry) & 0xFFF);
	// doing And operation will give us the offse of the physical page from va
	pageOffset = ((*pgtentry)) & 0xFFF;
        if(pageOffset & (PTE_COW|PTE_W)){
                pageOffset = pageOffset | PTE_COW;
                pageOffset = pageOffset & (~(PTE_W));
        }

        pageOffset = pageOffset & (~(PTE_A|PTE_D));

        res = sys_page_map(parentEnvid, (void *)va, envid, (void *)va, pageOffset);
        if(res < 0)
               panic("Error in page map in dup page\n");
        res = sys_page_map(parentEnvid, (void *)va, parentEnvid, (void *)va, pageOffset);
        if(res < 0)
                panic(" error in duppage for parent env \n ");

        return 0;
	// LAB 4: Your code here.
//	panic("duppage not implemented");
	//return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
		
	uint64_t va=UTEXT;
	int res;
	set_pgfault_handler(pgfault);
	envid_t childEnvid = sys_exofork();
	if (childEnvid < 0)
		panic("child Fork creation failed\n");

	if (childEnvid == 0){
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
        while(va < (USTACKTOP-PGSIZE))
	{
		// checking whether the va's are mapped in this env
		// if so then we will create duppages
		if((uvpml4e[VPML4E(va)] & PTE_P))
		{
			if(uvpde[VPDPE(va)] & PTE_P)
			{
                		if((uvpd[VPD(va)] & PTE_P))
				{
 					if((uvpt[VPN(va)] & PTE_P)) 
						duppage(childEnvid, VPN(va));
					va+=PGSIZE;
				}
				else
				{
					va+=512*PGSIZE;
				}
			}
			else	
			{
				va+=512*512*PGSIZE;
			}
		}
		else
		{
			va+=512*512*PGSIZE;
		}
		//va+=PGSIZE;
	}
	// exception stack for child will created using sys_page_alloc sys call
	res = sys_page_alloc(childEnvid,(void*)(UXSTACKTOP-PGSIZE), PTE_P|PTE_U|PTE_W);
	if(res<0)
	 	panic("error",res);
	// Now copy the contents of the user stack to the child by copying the contents to PFTEMP
	// there and then to child stack
	uint64_t vaChildStack = USTACKTOP-PGSIZE;
	res=sys_page_alloc(0, (void *)PFTEMP, PTE_P|PTE_U|PTE_W);
	if (res < 0)
                panic("error",res);
        memcpy(PFTEMP, (void *) ROUNDDOWN(vaChildStack, PGSIZE), PGSIZE);
        
	void* vaPtrChildStack = (void *) ROUNDDOWN(va, PGSIZE);
        res = sys_page_map(0, (void *)PFTEMP,childEnvid, vaPtrChildStack, PTE_P|PTE_U|PTE_W);
	if (res < 0)
                panic("error",res);
        res=sys_page_unmap(0, (void *)PFTEMP);
	if (res < 0)
            	panic("error",res);
	// We are done with copying the address space into the chil env now we 
	// will set up pg fault handler for child
	
    	 res = sys_env_set_pgfault_upcall(childEnvid,thisenv->env_pgfault_upcall);    
        if (res < 0)
                panic("error",res);
    	// we have given all the power for the child to run 
	// so lets send it to the runnable state
	res = sys_env_set_status(childEnvid, ENV_RUNNABLE);
        if (res< 0)
                panic("error",res);
	
	
	return childEnvid;
	// LAB 4: Your code here.
	//panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
