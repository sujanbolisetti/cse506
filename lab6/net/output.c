#include "ns.h"
#include <inc/lib.h>

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	
	/*struct sockaddr *addr = nsipcbuf.acceptRet.ret_addr;
	socklen_t soc_len = nsipcbuf.acceptRet.ret_addrlen;
	nsipc_accept(NSREQ_ACCEPT,addr,&soc_len);*/
	
	int r,t;
	while(1){
		
		r = ipc_recv(NULL,&nsipcbuf,NULL);
		if(r < 0)
		{
			cprintf("Unfortunately receive has failed");
			return;
		}
		//cprintf(" in output.c %s\n",nsipcbuf.pkt.jp_data);
		while((t=sys_transmit_packets(nsipcbuf.pkt.jp_data,nsipcbuf.pkt.jp_len)) < 0)
			sys_yield();
	
		}

}
