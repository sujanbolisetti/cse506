#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H
#include <kern/pci.h>

#define e1000_vendor_id 0x8086
#define e1000_device_id 0x100e

#define tx_buffer_size 	64
#define rx_buffer_size 64

/* transmit descriptor register addresses */

#define e1000_td_bal   	0x03800  
#define e1000_td_bah   	0x03804 
#define e1000_td_len   	0x03808
#define e1000_tdt      	0x03818
#define e1000_tdh      	0x03810

/* receive descriptor register address */

#define e1000_rd_bal    0x02800  /* RX Descriptor Base Address Low - RW */
#define e1000_rd_bah    0x02804  /* RX Descriptor Base Address High - RW */
#define e1000_rd_len    0x02808  /* RX Descriptor Length - RW */
#define e1000_rdh       0x02810  /* RX Descriptor Head - RW */
#define e1000_rdt       0x02818  /* RX Descriptor Tail - RW */
#define e1000_tctl     	0x00400
#define e1000_tcl_en   	0x00000002 
#define e1000_tcl_psp  	0x00000008
#define e1000_tcl_thl  	0x00000ff0
#define e1000_tcl_cold 	0x10000000
#define e1000_mta 	0x5200
#define e1000_rctl_stat_dd 0x01


#define e1000_tcl_stat_dd    0x00000001

#define e1000_tipg     0x00410 

#define e1000_tx_desc_rs     0x08000000 

#define e1000_tx_desc_eop 0x01000000

#define e1000_ectl     0x00100  /* RX Control - RW */

#define e1000_rctl 0x00100
#define e1000_rx_ral     0x05400
#define e1000_rx_rah     0x05404
#define e1000_rx_rah_av  0x80000000

/* RCTL registers  */

#define e1000_rctl_en 0x00000002
#define e1000_rctl_lpe 0x00000020
#define e1000_rctl_lbm 0x00000000
#define e1000_rctl_bam 0x00008000
#define e1000_rctl_secrc 0x04000000




struct tx_desc
{
	uint64_t addr; /* Address of the transmit descriptor array */
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

struct rx_desc {
    uint64_t addr; /* Address of the descriptor's data buffer */
    uint16_t length;     /* Length of data DMAed into data buffer */
    uint16_t csum;       /* Packet checksum */
    uint8_t status;      /* Descriptor status */
    uint8_t errors;      /* Descriptor Errors */
    uint16_t special;
};

int pci_e1000driver_attach_fun(struct pci_func *);
void initialization_e1000();
int transmit_packets_e1000(char *,uint32_t);
int receive_packets_e1000(char *);


#endif
