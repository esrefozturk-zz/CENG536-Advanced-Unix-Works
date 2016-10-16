#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#define UNION_MODE 0
#define INTERSECT_MODE 1
#define NAME_SIZE  256

void union_dirs( char *dir1, char *dir2, int depth )
{
	printf("union: %s %s\n",dir1,dir2);

	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	char *c1,*c2;

	dirp1 = opendir(dir1);
	dirp2 = opendir(dir2);

	while( (s1=readdir(dirp1)) )
	{
		if( !strcmp(s1->d_name,".") || !strcmp(s1->d_name,"..") )
			continue;
		if( s1->d_type == DT_DIR )
		{
			while( (s2=readdir(dirp2)) )
			{
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c1 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c1 = strcpy(c1,dir1);
					c1 = strcat(c1,"/");
					c1 = strcat(c1,s1->d_name);

					c2 = (char *)malloc( depth * (NAME_SIZE+1) * sizeof(char*) );
					c2 = strcpy(c2,dir1);
					c2 = strcat(c2,"/");
					c2 = strcat(c2,s2->d_name);

					union_dirs(c1,c2,depth+1);

					break;
				}
			}
		}
		else if( s1->d_type == DT_REG )
		{

		}
		else if( s1->d_type == DT_LNK )
		{

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
	}
	else
	{
		intersect_dirs(dir1,dir2,1);
	}

}
