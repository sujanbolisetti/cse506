#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

extern long delete_module(const char*, unsigned int);

int main(int argc, char* argv[]);
{
	if (argc==0)
	{
		fprintf(stderr, "No module has been specified!Cannot remove!\n");
		exit(1);
	}
	
	for(int i=1; i<=argc; i++)
	{
		int r=remove(argv[i-1],O_NONBLOCK|O_RDONLY|O_WRONLY);
		if(r==-1)
			exit(1);
	}
}

static int remove(const char* mod_name, int flags)
{
	if(flags & O_NONBLOCK)
	{
		//check if module is in use
	}	
	int r=delete_module(mod_name,flags);
	if(r==-1)
	{
		fprintf(stderr, "Unable to remove the module!\n");
	}
	return r;
}
