#include <stdio.h>
#include <stdlib.h>
#include <inc/string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inc/elf.h>

//extern long init_module(void*, unsigned long, const char*);

int main(int argc, char* argv[])
{
	unsigned int len;

	char* file_name;
	file_name=argv[0];
	void* file;
	//file = read_file(file_name, &len);
	file=load_mod(file,&len);
	if(!file)
	{
		fprintf(stderr, "unable to read file!\n");
		exit(1);
	}
	long int r;
	//r = init_module(file, len, options);
	//file = load_mod(file,&len);
	r=init_module(file, len, options)
	if(r==-1)
	{
		fprintf(stderr, "unable to insert the module!\n");
		exit(1);
	}
	free(file);
}


static void* load_mod(const char *name, unsigned int *len)
{
	int* l=0;
	if((struct Elf*)file_name->e_magic != ELF_MAGIC)
	{
		fprintf(stderr, "Not a valid ELF image!\n");
	}
	unsigned int max_size = 16384;
        unsigned int max_new=2*max_size;

	struct Proghdr* p=(struct Proghdr*)(file_name + ((struct Elf*)file_name)->e_phoff);
        int p_num=((struct Elf*)file_name)->e_phnum;
        e_phnum--;
        while(e_phnum>=0)
        {
                if(p->p_type==ELF_PROG_LOAD)
                {
			void* buf = malloc(max_size);
        		size_t r = read(fd, buf, *len);
        		if(r<0)
        		{
                		fprintf(stderr);
                		return NULL;
        		}
        		l=l+r;
        		while(r>0)
        		{
                		if(l==max_size)
                		{
                        		void* r_new = realloc(buf, max_new);
                        		if(!r_new)
                        		{
                                		fprintf(stderr);
                                		return NULL;
                        		}
                        		buf=r_new;
				 }
                		l+=r;
        		}
        		close(fd);
        		return buf;
		}

        }
        e_phnum--;
}

	
