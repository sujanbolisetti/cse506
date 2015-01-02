
#include "fs.h"

// Return the virtual address of this disk block.
void*
diskaddr(uint64_t blockno)
{
	if (blockno == 0 || (super && blockno >= super->s_nblocks))
		panic("bad block number %08x in diskaddr", blockno);
	return (char*) (DISKMAP + blockno * BLKSIZE);
}

// Fault any disk block that is read in to memory by
// loading it from disk.
static void
bc_pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint64_t blockno = ((uint64_t)addr - DISKMAP) / BLKSIZE;
	int r;

	// Check that the fault was within the block cache region
	if (addr < (void*)DISKMAP || addr >= (void*)(DISKMAP + DISKSIZE))
		panic("page fault in FS: eip %08x, va %08x, err %04x",
		      utf->utf_rip, addr, utf->utf_err);

	// Sanity check the block number.
	if (super && blockno >= super->s_nblocks)
		panic("reading non-existent block %08x\n", blockno);

	// Allocate a page in the disk map region, read the contents
	// of the block from the disk into that page.
	// Hint: first round addr to page boundary.
	//
	// LAB 5: you code here:
	void *va = ROUNDDOWN(addr,PGSIZE);
	
	//struct PageInfo *page;
	//page_insert(thisenv->pml4e,page,va,PTE_P|PTE_U);
	//pte_t *pt = pml4e_walk(thisenv->pml4e,va,0);
	//int r;
	r=sys_page_alloc(0,ROUNDDOWN(addr,PGSIZE),PTE_P|PTE_W|PTE_U);
	if(r<0)
		panic("page alloc failed\n");
	
	ide_read(blockno*BLKSECTS,ROUNDDOWN(addr,PGSIZE),BLKSECTS);
	//memmove(page,diskaddr(blockno),sizeof(struct PageInfo));

}


void
bc_init(void)
{
	struct Super super;
	set_pgfault_handler(bc_pgfault);

	// cache the super block by reading it once
	memmove(&super, diskaddr(1), sizeof super);
}

