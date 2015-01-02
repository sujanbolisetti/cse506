#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
// LAB 6: Your driver code here


volatile uint32_t *e1000Addr;
struct tx_desc tx_desc_array[tx_buffer_size];
char tx_packet_buff[tx_buffer_size][1518];
char rx_packet_buff[rx_buffer_size][2048];
struct rx_desc rx_desc_array[rx_buffer_size];



int
pci_e1000driver_attach_fun(struct pci_func *pcif)
{
        //mmio_map_region()
        pci_func_enable(pcif);
        e1000Addr = mmio_map_region((physaddr_t)pcif->reg_base[0],(size_t)pcif->reg_size[0]);
        cprintf("status %x\n",*(e1000Addr+2));
	initialization_e1000();
        return 1;
}


void initialization_e1000()
{
	
	int i;
	*(e1000Addr + e1000_td_bal/4) = PADDR(tx_desc_array);
	*(e1000Addr + e1000_td_len/4) = sizeof(tx_desc_array);
	*(e1000Addr + e1000_tdh/4) = 0x0;
	*(e1000Addr + e1000_tdt/4) = 0x0;
	
	*(e1000Addr + e1000_tctl/4) = (e1000_tcl_en | e1000_tcl_psp | e1000_tcl_thl | e1000_tcl_cold);
	
	// calculated based on the default values given in the manual
	//*(e1000Addr + e1000_tipg/4) = 
	
	for(i=0;i<tx_buffer_size;i++)
	{
		tx_desc_array[i].addr = PADDR(tx_packet_buff[i]);
		tx_desc_array[i].status = e1000_tcl_stat_dd;
	
	}
	
	//transmit_packets_e1000("hi transmitting",14);

	*(e1000Addr + e1000_rx_ral/4) = 0x12005452;
	*(e1000Addr + e1000_rx_rah/4)  = 0x5634 | e1000_rx_rah_av;
	*(e1000Addr + e1000_rd_bal/4) = PADDR(rx_desc_array);
        *(e1000Addr + e1000_rd_len/4) = sizeof(rx_desc_array);
        *(e1000Addr + e1000_rdh/4) = 0x0;
        *(e1000Addr + e1000_rdt/4) = 0x0;

	*(e1000Addr + e1000_mta/4) =0x0;
	
	*(e1000Addr + e1000_rctl/4) = (e1000_rctl_en | e1000_rctl_bam | e1000_rctl_secrc);
	 for(i=0;i<rx_buffer_size;i++)
         {
                rx_desc_array[i].addr = PADDR(rx_packet_buff[i]);
		//rx_desc_array[i].status = e1000_tcl_stat_dd;
	 }	
	 return; 

	
}

/*
	transmit_packets_e1000 :
				Returns 0 on success
				Return -1 on failure due to the transmit queue is full 

*/

int transmit_packets_e1000(char *packet_buffer,uint32_t pkt_length){
	uint32_t current_tdt,next_tdt;
	current_tdt = *(e1000Addr + e1000_tdt/4);
	next_tdt = (current_tdt + 1)%tx_buffer_size;
	
	int isFree = tx_desc_array[current_tdt].status & e1000_tcl_stat_dd;
	
	if(!isFree)
		return -1;	
	memmove(tx_packet_buff[current_tdt],packet_buffer,pkt_length);

	tx_desc_array[current_tdt].length = pkt_length;
	tx_desc_array[current_tdt].cmd |= e1000_tx_desc_rs;	
	tx_desc_array[current_tdt].cmd = tx_desc_array[current_tdt].cmd || e1000_tx_desc_eop;	
	
	*(e1000Addr + e1000_tdt/4) = next_tdt;
	return 0;

}


int receive_packets_e1000(char *packet_buffer)
{

	uint32_t current_rdt,next_rdt;
	current_rdt = *(e1000Addr + e1000_rdt/4);
	next_rdt = (current_rdt+1)%rx_buffer_size;
	
	int status = rx_desc_array[current_rdt].status & e1000_rctl_stat_dd;

//	cprintf("status %d\n",rx_desc_array[current_rdt].status);
	if(!status)
		return -1;
	cprintf("status %d\n",rx_desc_array[current_rdt].status);
	//if(!(rx_desc_array[current_rdt].status & e1000_tcl_stat_dd))
		//return -1;
	
	int length = rx_desc_array[current_rdt].length;
	memmove(packet_buffer,rx_packet_buff[current_rdt],length);
	
	cprintf(" packet data %s\n",packet_buffer);	

	/* resetting the status so that hardware can use this on next receive */
        rx_desc_array[current_rdt].status = 0x0;
	*(e1000Addr + e1000_rdt/4) = next_rdt;
	cprintf("length %d\n",length);
	return length;	
}	
