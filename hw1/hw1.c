#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define UNION_MODE 0
#define INTERSECT_MODE 1
#define NAME_SIZE  256

void union_dirs( char *dir1, char *dir2, int depth )
{
	printf("union: %s %s\n",dir1,dir2);

	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	char *c1,*c2;
	int flag=0;
	struct stat *buf1,*buf2;

	dirp1 = opendir(dir1);
	dirp2 = opendir(dir2);

	while( (s1=readdir(dirp1)) )
	{
		if( !strcmp(s1->d_name,".") || !strcmp(s1->d_name,"..") )
			continue;
		if( s1->d_type == DT_DIR )
		{
			flag=0;
			while( (s2=readdir(dirp2)) )
			{
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c1 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c1 = strcpy(c1,dir1);
					c1 = strcat(c1,"/");
					c1 = strcat(c1,s1->d_name);

					c2 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c2 = strcpy(c2,dir2);
					c2 = strcat(c2,"/");
					c2 = strcat(c2,s2->d_name);

					union_dirs(c1,c2,depth+1);
					free(c1);
					free(c2);
					flag=1;
					break;
				}
			}
			if(!flag)
			{
				printf("cp -rp %s/%s %s\n",dir1,s1->d_name,dir2);
			}
		}
		else if( s1->d_type == DT_REG )
		{
			flag=0;
			while( (s2=readdir(dirp2)) )
			{
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c1 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c1 = strcpy(c1,dir1);
					c1 = strcat(c1,"/");
					c1 = strcat(c1,s1->d_name);

					c2 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c2 = strcpy(c2,dir2);
					c2 = strcat(c2,"/");
					c2 = strcat(c2,s2->d_name);

					buf1 = (struct stat*)malloc( sizeof(struct stat) );
					buf2 = (struct stat*)malloc( sizeof(struct stat) );


					stat(c1,buf1);
					stat(c2,buf2);

					if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						printf("cp -p %s %s",c1,dir2);
					}
					else if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						printf("cp -p %s %s",c2,dir1);
					}
					else if( s2->d_type == DT_LNK )
					{
						printf("cp -p %s %s",c1,dir2);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("cp -p %s %s",c1,dir2);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						printf("cp -p %s %s",c2,dir1);
					}
					flag=1;
					break;

					free(c1);
					free(c2);
					free(buf1);
					free(buf2);
				}
			}
			printf("cp -p %s/%s %s",dir1,s1->d_name,dir2);
		}
		else if( s1->d_type == DT_LNK )
		{
			flag=0;
			while( (s2=readdir(dirp2)) )
			{
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c1 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c1 = strcpy(c1,dir1);
					c1 = strcat(c1,"/");
					c1 = strcat(c1,s1->d_name);

					c2 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c2 = strcpy(c2,dir2);
					c2 = strcat(c2,"/");
					c2 = strcat(c2,s2->d_name);

					buf1 = (struct stat*)malloc( sizeof(struct stat) );
					buf2 = (struct stat*)malloc( sizeof(struct stat) );


					stat(c1,buf1);
					stat(c2,buf2);

					if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						printf("cp -p %s %s",c1,dir2);
					}
					else if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						printf("cp -p %s %s",c2,dir1);
					}
					else if( s2->d_type == DT_REG )
					{
						printf("cp -p %s %s",c2,dir1);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("cp -p %s %s",c1,dir2);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						printf("cp -p %s %s",c2,dir1);
					}
					flag=1;
					break;

					free(c1);
					free(c2);
					free(buf1);
					free(buf2);
				}
			}
			printf("cp -p %s/%s %s",dir1,s1->d_name,dir2);

		}
	}

	closedir(dirp1);
	closedir(dirp2);



}

void intersect_dirs( char* dir1, char* dir2, int depth )
{

}

int main(int argc, char **argv)
{
	char *dir1,*dir2;
	int mode = UNION_MODE;
	if( !strcmp(argv[1],"-i") )
	{
		mode = INTERSECT_MODE;
		dir1 = argv[2];
		dir2 = argv[3];
	}
	else
	{
		dir1 = argv[1];
		dir2 = argv[2];
	}

	if( mode == UNION_MODE )
	{
		union_dirs(dir1,dir2,1);
		union_dirs(dir2,dir1,1);
	}
	else
	{
		intersect_dirs(dir1,dir2,1);
	}

}
