#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define UNION_MODE 0
#define INTERSECT_MODE 1

void union_dirs( char *dir1, char *dir2 )
{
	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	
	dirp1 = opendir(dir1);
	dirp2 = opendir(dir2);

	s1 = readdir(dirp1);
	s2 = readdir(dirp2);

	closedir(dirp1);
	closedir(dirp2);

		
}

void intersect_dirs( char* dir1, char* dir2 )
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
		union_dirs(dir1,dir2);
	}
	else
	{
		intersect_dirs(dir1,dir2);
	}
	
}
