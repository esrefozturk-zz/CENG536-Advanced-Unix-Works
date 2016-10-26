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

char* concat3(char *ch1, char *ch2, char *ch3)
{
	char *result = (char*)malloc( sizeof(char) * ( strlen(ch1) + strlen(ch2) + strlen(ch3) + 2 ) );
	result = strcpy(result,ch1);
	result = strcat(result,ch2);
	result = strcat(result,ch3);
	return result;
}



void union_dirs( char *dir1, char *dir2, int look )
{
	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	char *c1,*c2;
	int flag=0;
	struct stat *buf1,*buf2;
	char *sym1,*sym2;
	int size1,size2;

	dirp1 = opendir(dir1);

	while( (s1=readdir(dirp1)) )
	{
		if( !strcmp(s1->d_name,".") || !strcmp(s1->d_name,"..") )
			continue;
		dirp2 = opendir(dir2);
		c1 = concat3( dir1, "/" , s1->d_name );

		buf1 = (struct stat*)malloc( sizeof(struct stat) );

		if( s1->d_type == DT_REG )
			stat(c1,buf1);
		else if( s1->d_type == DT_LNK )
		{
			lstat(c1,buf1);

			sym1 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
			size1 = readlink(c1,sym1, NAME_SIZE);
			sym1[size1] = 0;
		}

		flag = 0;
		if( s1->d_type == DT_DIR )
		{
			while( (s2=readdir(dirp2)) )
			{
				if( look )
				{
					flag = 1;
					break;
				}
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c2 = concat3( dir2, "/" , s2->d_name );

					union_dirs(c1,c2,look);

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
			while( (s2=readdir(dirp2)) )
			{
				if( look )
				{
					flag = 1;
					break;
				}
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c2 = concat3( dir2, "/" , s2->d_name );

					buf2 = (struct stat*)malloc( sizeof(struct stat) );

					if( s2->d_type == DT_REG )
						stat(c2,buf2);
					else
					{
						lstat(c2,buf2);

						sym2 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
						size2 = readlink(c2,sym2, NAME_SIZE);
						sym2[size2] = 0;

					}
					if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						if( s2->d_type == DT_LNK )
							printf("rm %s\n",c2);
						printf("cp -p %s %s\n",c1,dir2);
					}
					else if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						if( s2->d_type == DT_REG )
						{
							printf("cp -p %s %s\n",c2,dir1);
						}
						else
						{
							printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
						}
					}
					else if( s2->d_type == DT_LNK )
					{
						printf("rm %s\n",c2);
						printf("cp -p %s %s\n",c1,dir2);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("cp -p %s %s\n",c1,dir2);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						printf("cp -p %s %s\n",c2,dir1);
					}


					free(c2);
					if( s2->d_type == DT_LNK )
						free(sym2);
					free(buf2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				printf("cp -p %s %s\n",c1,dir2);
			}
		}
		else if( s1->d_type == DT_LNK )
		{
			while( (s2=readdir(dirp2)) )
			{
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					if( look )
					{
						flag = 1;
						break;
					}
					c2 = concat3( dir2, "/" , s2->d_name );

					sym2 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
					size2 = readlink(c2,sym2, NAME_SIZE);
					sym2[size2] = 0;


					buf2 = (struct stat*)malloc( sizeof(struct stat) );



					if( s2->d_type == DT_REG )
						stat(c2,buf2);
					else
						lstat(c2,buf2);

					if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						if( s2->d_type == DT_LNK )
							printf("rm %s\n",c2);
						printf("ln -sf %s %s/%s\n",sym1,dir2,s1->d_name);
					}
					else if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						printf("rm %s\n",c1);
						if( s2->d_type == DT_REG )
						{
							printf("cp -p %s %s\n",c2,dir1);
						}
						else
						{
							printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
						}
					}
					else if( s2->d_type == DT_REG )
					{
						printf("rm %s\n",c1);
						printf("cp -p %s %s\n",c2,dir1);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("rm %s\n",c2);
						printf("ln -sf %s %s/%s\n",sym1,dir2,s1->d_name);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						printf("rm %s\n",c1);
						printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
					}


					free(c2);
					free(buf2);
					free(sym2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				printf("cp -p %s/%s %s\n",dir1,s1->d_name,dir2);
			}

		}

		if( s1->d_type == DT_LNK )
		{
			free(sym1);
		}
		free(c1);
		free(buf1);

		closedir(dirp2);
	}

	closedir(dirp1);




}

void intersect_dirs( char* dir1, char* dir2, int look)
{
	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	char *c1,*c2;
	int flag=0;
	struct stat *buf1,*buf2;
	char *sym1,*sym2;
	int size1,size2;

	dirp1 = opendir(dir1);

	while( (s1=readdir(dirp1)) )
	{

		if( !strcmp(s1->d_name,".") || !strcmp(s1->d_name,"..") )
			continue;
		dirp2 = opendir(dir2);

		dirp2 = opendir(dir2);
		c1 = concat3( dir1, "/" , s1->d_name );

		buf1 = (struct stat*)malloc( sizeof(struct stat) );

		if( s1->d_type == DT_REG )
			stat(c1,buf1);
		else if( s1->d_type == DT_LNK )
		{
			lstat(c1,buf1);

			sym1 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
			size1 = readlink(c1,sym1, NAME_SIZE);
			sym1[size1] = 0;
		}

		flag = 0;
		if( s1->d_type == DT_DIR )
		{
			while( (s2=readdir(dirp2)) )
			{
				if( look )
				{
					flag = 1;
					break;
				}
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c2 = concat3( dir2, "/" , s2->d_name );

					intersect_dirs(c1,c2,look);

					free(c2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				printf("rm -rf %s/%s\n",dir1,s1->d_name);
			}
		}
		else if( s1->d_type == DT_REG )
		{
			while( (s2=readdir(dirp2)) )
			{
				if( look )
				{
					flag = 1;
					break;
				}
				if( !strcmp(s1->d_name, s2->d_name) )
				{
					c2 = concat3( dir2, "/" , s2->d_name );

					buf2 = (struct stat*)malloc( sizeof(struct stat) );


					if( s2->d_type == DT_REG )
						stat(c2,buf2);
					else
					{
						lstat(c2,buf2);

						sym2 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
						size2 = readlink(c2,sym2, NAME_SIZE);
						sym2[size2] = 0;

					}

					if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						if( s2->d_type == DT_LNK )
							printf("rm %s\n",c2);
						printf("cp -p %s %s\n",c1,dir2);
					}
					else if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						if( s2->d_type == DT_REG )
						{
							printf("cp -p %s %s\n",c2,dir1);
						}
						else
						{
							printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
						}
					}
					else if( s2->d_type == DT_LNK )
					{
						printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						printf("cp -p %s %s\n",c1,dir2);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("cp -p %s %s\n",c2,dir1);
					}


					free(c2);
					if( s2->d_type == DT_LNK )
						free(sym2);
					free(buf2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				printf("rm %s\n",c1);
			}
		}
		else if( s1->d_type == DT_LNK )
		{

			while( (s2=readdir(dirp2)) )
			{
				if( look )
				{
					flag = 1;
					break;
				}
				if( !strcmp(s1->d_name, s2->d_name) )
				{

					c2 = concat3( dir2, "/" , s2->d_name );

					sym1 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
					size1 = readlink(c1,sym1, NAME_SIZE);
					sym1[size1] = 0;

					buf2 = (struct stat*)malloc( sizeof(struct stat) );


					if( s2->d_type == DT_REG )
						stat(c2,buf2);
					else
					{
						lstat(c2,buf2);

						sym2 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
						size2 = readlink(c2,sym2, NAME_SIZE);
						sym2[size2] = 0;

					}

					if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						if( s2->d_type == DT_LNK )
							printf("rm %s\n",c2);
						printf("ln -sf %s %s/%s\n",sym1,dir2,s1->d_name);
					}
					else if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						printf("rm %s\n",c1);
						if( s2->d_type == DT_REG )
						{
							printf("cp -p %s %s\n",c2,dir1);
						}
						else
						{
							printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
						}
					}
					else if( s2->d_type == DT_REG )
					{
						printf("rm %s\n",c2);
						printf("ln -sf %s %s/%s\n",sym1,dir2,s1->d_name);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("rm %s\n",c2);
						printf("ln -sf %s %s/%s\n",sym1,dir2,s1->d_name);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						printf("rm %s\n",c1);
						printf("ln -sf %s %s/%s\n",sym2,dir1,s2->d_name);
					}

					free(c2);
					if( s2->d_type == DT_LNK )
						free(sym2);
					free(buf2);


					flag=1;
					break;
				}
			}
			if(!flag)
			{
				printf("cp -p %s/%s %s\n",dir1,s1->d_name,dir2);
			}

		}

		if( s1->d_type == DT_LNK )
		{
			free(sym1);
		}
		free(c1);
		free(buf1);

		closedir(dirp2);
	}

	closedir(dirp1);




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
		union_dirs(dir1,dir2,0);
		union_dirs(dir2,dir1,1);
	}
	else
	{
		intersect_dirs(dir1,dir2,0);
		intersect_dirs(dir1,dir2,1);
	}

}
