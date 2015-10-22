#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <openssl/md5.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>



FILE *list_dir (const char * dir_name,FILE *file_storage)
{
	//printf("%s\n",dir_name );
	DIR * d;
	d = opendir (dir_name);
	if (! d)
	{
		fprintf (stderr, "Cannot open directory '%s': %s\n",
				dir_name, strerror (errno));
		return file_storage;
	}
	while (1) 
	{
		struct dirent * entry;
		const char * d_name;
		entry = readdir (d);
		if (! entry) 
		{
			break;
		}
		d_name = entry->d_name;
		struct stat st;
		stat(entry->d_name, &st);
		if(!(entry->d_type & DT_DIR))
		{
			//printf("%s\n",d_name);
			fprintf (file_storage,"%s ", d_name);
			fprintf(file_storage,"%s", ctime(&st.st_mtime));
		}

		if (entry->d_type & DT_DIR) 
		{
			if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) 
			{
				int path_length;
				char path[PATH_MAX];
				path_length = snprintf (path, PATH_MAX,
						"%s/%s", dir_name, d_name);
				if (path_length >= PATH_MAX) {
					fprintf (stderr, "Path length has got too long.\n");
					break;
				}
				/* Recursively call "list_dir" with the new path. */
				fprintf(file_storage,"=================================================\n");
				fprintf(file_storage,"DIRECTORY_NAME: %s\n", path);
				list_dir (path,file_storage);
				fprintf(file_storage,"=================================================\n");
			}
		}
	}
	//fclose(file_storage);
	if (closedir (d)) 
	{
		fprintf (stderr, "Could not close '%s': %s\n",
				dir_name, strerror (errno));
		return file_storage;
	}

	return file_storage;
}


FILE *list_dir_hash (const char * dir_name,FILE *file_storage)
{
	char my_dir[1024];
	//printf("%s\n",dir_name );
	strcpy(my_dir,dir_name);
	DIR * d;
	d = opendir (dir_name);
	if (! d)
	{
		fprintf (stderr, "Cannot open directory '%s': %s\n",
				dir_name, strerror (errno));
		return file_storage;
	}
	while (1) 
	{
		struct dirent * entry;
		const char * d_name;
		entry = readdir (d);
		if (! entry) 
		{
			break;
		}
		d_name = entry->d_name;
		struct stat st;
		stat(entry->d_name, &st);
		if(!(entry->d_type & DT_DIR))
		{
			//printf("%s\n",d_name);
			fprintf (file_storage,"%s ", d_name);
			unsigned char c[MD5_DIGEST_LENGTH];
			MD5_CTX mdContext;
			int bytes;
			unsigned char data[1024];
			MD5_Init (&mdContext);
			bzero(my_dir,1024);
			strcpy(my_dir,dir_name);
			FILE *file_reading = fopen(strcat(my_dir,d_name),"r");
			while ((bytes = fread (data, 1, 1024, file_reading)) != 0)
			MD5_Update (&mdContext, data, bytes);
			MD5_Final (c,&mdContext);
			int i;
			for(i = 0; i < MD5_DIGEST_LENGTH; i++)
			{
				fprintf(file_storage,"%02x",c[i]);
			}

			fprintf(file_storage," %s", ctime(&st.st_mtime));
		}

		if (entry->d_type & DT_DIR) 
		{
			if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) 
			{
				int path_length;
				char path[PATH_MAX];
				path_length = snprintf (path, PATH_MAX,
						"%s/%s", dir_name, d_name);
				if (path_length >= PATH_MAX) {
					fprintf (stderr, "Path length has got too long.\n");
					break;
				}
				/* Recursively call "list_dir" with the new path. */
				fprintf(file_storage,"=================================================\n");
				fprintf(file_storage,"DIRECTORY_NAME: %s\n", path);
				list_dir_hash (path,file_storage);
				fprintf(file_storage,"=================================================\n");
			}
		}
	}
	//fclose(file_storage);
	if (closedir (d)) 
	{
		fprintf (stderr, "Could not close '%s': %s\n",
				dir_name, strerror (errno));
		return file_storage;
	}

	return file_storage;
}



int main(int argc, char *argv[])
{
	int i,j;
	char *token;
	char path[1024];
	char temp[20][100];
	int listenSocket = 0;	// This is my server's socket which is created to 
	//	listen to incoming connections
	int connectionSocket = 0;

	struct sockaddr_in serv_addr;		// This is for addrport for listening

	int listenSocket1 = 0;  // This is my server's socket which is created to
		//      listen to incoming connections
	int connectionSocket1 = 0;
	struct sockaddr_in serv_addr1;          // This is for addrport for listening

	// Creating a socket

	listenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(listenSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return 0;
	}
	else
		printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

	// Its a general practice to make the entries 0 to clear them of malicious entry

	bzero((char *) &serv_addr,sizeof(serv_addr));

	// Binding the socket

	int portno = 5005;
	serv_addr.sin_family = AF_INET;	//For a remote machine
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portno);

	if(bind(listenSocket,(struct sockaddr * )&serv_addr,sizeof(serv_addr))<0)
		printf("ERROR WHILE BINDING THE SOCKET\n");
	else
		printf("[SERVER] SOCKET BINDED SUCCESSFULLY\n");

	// Listening to connections

	if(listen(listenSocket,100) == -1)	//maximum connections listening to 10
	{
		printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
	}
	printf("[SERVER] Waiting fo client to connect....\n" );

	// Accepting connections
	while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);

	// NULL will get filled in by the client's sockaddr once a connection is establised

	int mine=0;
	printf("[CONNECTED]\n");
	while(1)
	{

		char buffer[1024];
		bzero(buffer,1024);
		for(i=0;i<1024;i++)
			buffer[i]='\0';
		if(mine!=1)
			if(recv(connectionSocket,buffer,1023,0)>0)
			{
				//printf("ERROR whiile reading from Client\n");
				//printf("Command from Client:");
				//int l = strlen(buffer);
				//printf("len--> %d",l);
				printf("%s\n",buffer);
			}
		//printf("buf---> %d\n",strlen(buffer));
			int buf_len=strlen(buffer);
		/*--------------------------------------- Splitting the input commands------------------------------------------*/
		if(buf_len==0)
		{
			break;
		}
		

		/*for(i = 0; i<20; i++)
		{
			temp[i][0] = '\0';
		}*/
		int ne=0,t=0;
		int flag=0;
		for(i=0;i<20;i++)
		{
			for(j=0;j<100;j++)
			{
				temp[i][j]='\0';
			}
		}
		while(buffer[ne]!='\0')
		{
			if(buffer[ne]==' ')
			{
				flag++;
				ne++;
				t=0;
			}
			//if(buffer[ne]!='\n')
			//{
			temp[flag][t]=buffer[ne];
			ne++;
			t++;
			//}
		}

		/*------------------------------------------IndexGet -------------------------------------------------------*/
		if(strcmp(temp[0], "IndexGet") == 0)
		{
			printf("%s\n",temp[1]);
			if(strcmp(temp[1],"--shortlist")==0)
			{
				printf("getting shortlist\n");
				FILE *file_storage;
				DIR *directory;
				struct dirent *dir;
				directory = opendir(argv[1]);
				if(directory)
				{
					file_storage = fopen("file-storage.txt","w");
					while((dir = readdir(directory))!= NULL)
					{
						if(dir->d_name[0]!='.')
						{

							// get the modified time of the file
							struct stat st;
							stat(dir->d_name, &st);
							char timer[10][100];
							char timer_buffer[200];
							bzero(timer_buffer,200);
							//printf("time of the file ---> %s %s\n",dir->d_name,ctime(&st.st_mtime));
							strcpy(timer_buffer,ctime(&st.st_mtime));
							int integer=0,position=0;
							int pos=0;
							while(timer_buffer[integer]!='\0')
							{
								if(timer_buffer[integer]==' ')
								{
									pos++;
									integer++;
									position=0;
								}
								timer[pos][position]=timer_buffer[integer];
								integer++;
								position++;
							}
							int file_year = atoi(timer[4]);
							int start_year = atoi(temp[6]);
							int end_year = atoi(temp[11]);
							if(file_year < end_year && file_year > start_year)
							{
								fprintf(file_storage,"%s ",dir->d_name);
								//struct stat st;
								//stat(dir->d_name, &st);
								int size = st.st_size;
								fprintf(file_storage," %d ",size);
								fprintf(file_storage,"%s", ctime(&st.st_mtime));
							}
							if(file_year == end_year || file_year == start_year)
							{
								printf("years passed\n");

								int file_month;
								int start_month;
								int end_month;
								if(strcmp(temp[3],"Jan")==0)
								{
									start_month = 1;
								}
								if(strcmp(temp[3],"Feb")==0)
								{
									start_month = 2;
								}
								if(strcmp(temp[3],"Mar")==0)
								{
									start_month = 3;
								}
								if(strcmp(temp[3],"Apr")==0)
								{
									start_month = 4;
								}
								if(strcmp(temp[3],"May")==0)
								{
									start_month = 5;
								}
								if(strcmp(temp[3],"Jun")==0)
								{
									start_month = 6;
								}
								if(strcmp(temp[3],"Jul")==0)
								{
									start_month = 7;
								}
								if(strcmp(temp[3],"Aug")==0)
								{
									start_month = 8;
								}
								if(strcmp(temp[3],"Sep")==0)
								{
									start_month = 9;
								}
								if(strcmp(temp[3],"Oct")==0)
								{
									start_month = 10;
								}
								if(strcmp(temp[3],"Nov")==0)
								{
									start_month = 11;
								}
								if(strcmp(temp[3],"Dec")==0)
								{
									start_month = 12;
								}
								/*-----------------------------------------*/
								if(strcmp(temp[8],"Jan")==0)
								{
									end_month = 1;
								}
								if(strcmp(temp[8],"Feb")==0)
								{
									end_month = 2;
								}
								if(strcmp(temp[8],"Mar")==0)
								{
									end_month = 3;
								}
								if(strcmp(temp[8],"Apr")==0)
								{
									end_month = 4;
								}
								if(strcmp(temp[8],"May")==0)
								{
									end_month = 5;
								}
								if(strcmp(temp[8],"Jun")==0)
								{
									end_month = 6;
								}
								if(strcmp(temp[8],"Jul")==0)
								{
									end_month = 7;
								}
								if(strcmp(temp[8],"Aug")==0)
								{
									end_month = 8;
								}
								if(strcmp(temp[8],"Sep")==0)
								{
									end_month = 9;
								}
								if(strcmp(temp[8],"Oct")==0)
								{
									end_month = 10;
								}
								if(strcmp(temp[8],"Nov")==0)
								{
									end_month = 11;
								}
								if(strcmp(temp[8],"Dec")==0)
								{
									end_month = 12;
								}

								/*---------------------------------------------------------------------------*/
								if(strcmp(timer[1],"Jan")==0)
								{
									file_month = 1;
								}
								if(strcmp(timer[1],"Feb")==0)
								{
									file_month = 2;
								}
								if(strcmp(timer[1],"Mar")==0)
								{
									file_month = 3;
								}
								if(strcmp(timer[1],"Apr")==0)
								{
									file_month = 4;
								}
								if(strcmp(timer[1],"May")==0)
								{
									file_month = 5;
								}
								if(strcmp(timer[1],"Jun")==0)
								{
									file_month = 6;
								}
								if(strcmp(timer[1],"Jul")==0)
								{
									file_month = 7;
								}
								if(strcmp(timer[1],"Aug")==0)
								{
									file_month = 8;
								}
								if(strcmp(timer[1],"Sep")==0)
								{
									file_month = 9;
								}
								if(strcmp(timer[1],"Oct")==0)
								{
									file_month = 10;
								}
								if(strcmp(timer[1],"Nov")==0)
								{
									file_month = 11;
								}
								if(strcmp(timer[1],"Dec")==0)
								{
									file_month = 12;
								}

								printf("start end file --> %d %d %d\n",start_month,end_month,file_month);

								if(file_month < end_month && file_month > start_month)
								{
									printf("month passed\n");

									fprintf(file_storage,"%s ",dir->d_name);
									//struct stat st;
									//stat(dir->d_name, &st);
									int size = st.st_size;
									fprintf(file_storage," %d ",size);
									fprintf(file_storage,"%s", ctime(&st.st_mtime));

								}
								if(file_month == end_month || file_month == start_month)
								{
									printf("month passed\n");
									int file_date = atoi(timer[2]);
									int start_date = atoi(temp[4]);
									int end_date = atoi(temp[9]);
									if(file_date < end_date && file_date > start_date)
									{
										fprintf(file_storage,"%s ",dir->d_name);
										//struct stat st;
										//stat(dir->d_name, &st);
										int size = st.st_size;
										fprintf(file_storage," %d ",size);
										fprintf(file_storage,"%s", ctime(&st.st_mtime));
									}
									if(file_date == end_date || file_date == start_date)
									{
										int ind=0;
										int ind1=0;int ind2=0;
										char start_hour[3][10];
										while(temp[5][ind]!='\0')
										{
											if(temp[5][ind]==':')
											{
												ind1++;
												ind2=0;
											}
											else
											{
												start_hour[ind1][ind2]=temp[5][ind];
												//ind++;
												ind2++;
											}
											ind++;
										}

										ind=0;ind1=0;ind2=0;
										char end_hour[3][10];
										while(temp[10][ind]!='\0')
										{
											if(temp[10][ind]==':')
											{
												ind1++;
												ind2=0;
											}
											else
											{
												end_hour[ind1][ind2]=temp[10][ind];
												//ind++;
												ind2++;
											}
											ind++;
										}

										ind=0;ind1=0;ind2=0;
										char file_hour[3][10];
										while(timer[3][ind]!='\0')
										{
											if(timer[3][ind]==':')
											{
												ind1++;
												ind2=0;
											}
											else
											{
												file_hour[ind1][ind2]=timer[3][ind];
												//ind++;
												ind2++;
											}
											ind++;
										}
										int file_hours0 = atoi(file_hour[0]);
										int file_hours1 = atoi(file_hour[1]);
										int file_hours2 = atoi(file_hour[2]);
										int start_hours0 = atoi(start_hour[0]);
										int start_hours1 = atoi(start_hour[1]);
										int start_hours2 = atoi(start_hour[2]);
										int end_hours0 = atoi(end_hour[0]);
										int end_hours1 = atoi(end_hour[1]);
										int end_hours2 = atoi(end_hour[2]);
										printf("--> start %d %d %d\n",start_hours0,start_hours1,start_hours2);
										printf("--> end %d %d %d\n",end_hours0,end_hours1,end_hours2);
										printf("--> file %d %d %d\n",file_hours0,file_hours1,file_hours2);
										if(file_hours0 < end_hours0 && file_hours0 > start_hours0)
										{
											fprintf(file_storage,"%s ",dir->d_name);
											//struct stat st;
											//stat(dir->d_name, &st);
											int size = st.st_size;
											fprintf(file_storage," %d ",size);
											fprintf(file_storage,"%s", ctime(&st.st_mtime));
										}
										if(file_hours0 == end_hours0 || file_hours0 == start_hours0)
										{

											if(file_hours1 < end_hours1 && file_hours1 > start_hours1)
											{
												fprintf(file_storage,"%s ",dir->d_name);
												//			struct stat st;
												//stat(dir->d_name, &st);
												int size = st.st_size;
												fprintf(file_storage," %d ",size);
												fprintf(file_storage,"%s", ctime(&st.st_mtime));
											}
											if(file_hours1 == end_hours1 || file_hours1 == start_hours1)
											{
												if(file_hours2 < end_hours2 && file_hours2 > start_hours2)
												{
													printf("-------> all passed\n");

													fprintf(file_storage,"%s ",dir->d_name);
													//					struct stat st;
													//stat(dir->d_name, &st);
													int size = st.st_size;
													fprintf(file_storage," %d ",size);
													fprintf(file_storage,"%s", ctime(&st.st_mtime));
												}
												if(file_hours2 == end_hours2 || file_hours2 == start_hours2)
												{
													printf("-------> all passed\n");
													fprintf(file_storage,"%s ",dir->d_name);
													//			struct stat st;
													//stat(dir->d_name, &st);
													int size = st.st_size;
													fprintf(file_storage," %d ",size);
													fprintf(file_storage,"%s", ctime(&st.st_mtime));
												}

											}
										}
									}
								}
							}
						}
					}
					//printf("%s\n",dir->d_name);
				}
				fclose(file_storage);
				closedir(directory);

				FILE *fp = fopen("file-storage.txt","rb");
				if(fp==NULL)
				{
					printf("file open error\n");
				}
				while(1)
				{
					unsigned char my_buffer[1024]={0};
					int nread = fread(my_buffer, 1, 1024, fp);
					if(nread < 0)
					{
						printf("read Failure.\n");
						break;
					}
					write(connectionSocket, my_buffer, nread);
					if(nread<1024)
					{
						break;
					}       
				}
				close(fp);
				int status=remove("file-storage.txt");
			}
			//shortlist

			if(strcmp(temp[1],"--longlist\n")==0)
			{
				//longlist
				FILE *file_storage1 = fopen("file-storage_long.txt","w");
				printf("getting longlist\n");
				//FILE *file_storage;
				DIR *directory;
				struct dirent *dir;
				file_storage1=list_dir (argv[1],file_storage1);
				fclose(file_storage1);
				FILE *fp = fopen("file-storage_long.txt","rb");
				if(fp==NULL)
				{
					printf("file open error\n");
				}
				while(1)
				{
					unsigned char my_buffer[1024]={0};
					int nread = fread(my_buffer, 1, 1024, fp);
					printf("%s\n",my_buffer);
					if(nread < 0)
					{
						printf("read Failure.\n");
						break;
					}
					write(connectionSocket, my_buffer, nread);

					if(nread<1024)
					{
						break;
					}       
				}
				close(fp);
				//int status=remove("file-storage_long.txt");
			}

			if(strcmp(temp[1],"--regex")==0)
			{
				//regex

			}

		}

		if(strcmp(temp[0],"FileHash")==0)
		{
			//printf("enntered\n");
			if(strcmp(temp[1],"--verify")==0)
			{
				//printf("enntered1\n");
				int le = strlen(temp[2]);
				//char c;
				printf("temp2--> %s ",temp[2]);
				printf("%d\n",le);
				char file_name[le];
				//file_name[le-1]='\0';
				bzero(file_name, le);
				le = strlen(temp[2]);
				printf("%d\n",le);
				int i=0;
				for (i=0;i<(le-1);i++)
				{
					file_name[i] = temp[2][i];
					//file_name[i] = c;
				}
				le = strlen(file_name);
				//printf("file_len--> %d\n",le);
				//printf("fucking file name-- %c", file_name[0] );
				FILE *file_hash;
				file_hash = fopen("file-hash.txt","w");
				DIR *given_dir;
				bzero(path, 1024);
				strcpy(path, argv[1]);
				FILE *inFile = fopen (strcat(path ,file_name), "rb");
				if (inFile == NULL) {
					perror("fopen");
					printf("opening1 file failed: %s\n", strerror(errno));
				}

				unsigned char c[MD5_DIGEST_LENGTH];

				MD5_CTX mdContext;
				int bytes;
				unsigned char data[1024];
				MD5_Init (&mdContext);


				while ((bytes = fread (data, 1, 1024, inFile)) != 0)
					MD5_Update (&mdContext, data, bytes);
				MD5_Final (c,&mdContext);
				for(i = 0; i < MD5_DIGEST_LENGTH; i++)
				{
					fprintf(file_hash,"%02x",c[i]);
				}

				fclose (inFile);
				struct stat attr;
				stat(file_name, &attr);
				fprintf(file_hash," %s",ctime(&attr.st_mtime));
				fclose(file_hash);
				FILE *fp = fopen("file-hash.txt","rb");
				if(fp==NULL)
				{
					printf("file open error\n");
				}
				while(1)
				{
					unsigned char my_buffer[1024]={0};
					int nread = fread(my_buffer, 1, 1024, fp);
					if(nread < 0)
					{
						printf("read Failure.\n");
						break;
					}
					write(connectionSocket, my_buffer, nread);
					if(nread<1024)
					{
						break;
					}       
				}
				close(fp);
				int j;
				for(i = 0; i<20; i++)
				{
					for(j=0;j<100;j++)
					{
						temp[i][j]='\0';
					}
				}
			}

			else if(strcmp(temp[1],"--checkall\n")==0)
			{
				FILE *file_long_hash = fopen("file-long-hash.txt","w");
				//printf("getting hashing of all\n");
				DIR *der;
				struct dirent *derect;
				file_long_hash = list_dir_hash(argv[1],file_long_hash);
				fclose(file_long_hash);
				FILE *fp_hash = fopen("file-long-hash.txt","rb");
				if(fp_hash==NULL)
				{
					printf("file open error\n");
				}
				while(1)
				{
					unsigned char my_buffer[1024]={0};
					int nread = fread(my_buffer, 1, 1024, fp_hash);
					printf("%s\n",my_buffer);
					if(nread < 0)
					{
						printf("read Failure.\n");
						break;
					}
					write(connectionSocket, my_buffer, nread);

					if(nread<1024)
					{
						break;
					}       
				}
				close(fp_hash);


			}

			//int status = remove("file_hash.txt");
		}

		if(strcmp(temp[0],"FileDownload")==0)
		{
			if( strcmp(temp[1],"--TCP")==0 ||(strcmp(temp[1],"--UDP")==0))
			{
		
				// Creating a socket
				if(strcmp(temp[1],"--TCP")==0)
					listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
				else if(strcmp(temp[1],"UDP")==0)
					listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_UDP);
				if(listenSocket<0)
				{
					printf("ERROR WHILE CREATING A SOCKET\n");
					return 0;
				}

				else
					printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

				// Its a general practice to make the entries 0 to clear them of malicious entry

				bzero((char *) &serv_addr,sizeof(serv_addr));

				// Binding the socket

				int portno = 5004;
				serv_addr1.sin_family = AF_INET;        //For a remote machine
				serv_addr1.sin_addr.s_addr = htonl(INADDR_ANY);
				serv_addr1.sin_port = htons(portno);

				if(bind(listenSocket1,(struct sockaddr * )&serv_addr1,sizeof(serv_addr))<0)
					printf("ERROR WHILE BINDING THE SOCKET\n");
				else
					printf("[SERVER] SOCKET BINDED SUCCESSFULLY\n");

				// Listening to connections

				if(listen(listenSocket1,10) == -1)      //maximum connections listening to 10
				{
					printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
				}
				printf("[SERVER] Waiting fo client to connect....\n" );

				// Accepting connections
				while((connectionSocket1=accept(listenSocket1 , (struct sockaddr*)NULL,NULL))<0);

				// NULL will get filled in by the client's sockaddr once a connection is establised

				//int mine=0;
				printf("[CONNECTED]\n");
			

			char safe[100];
			//char c[1024];
			bzero(safe,100);
			//printf("filename--> %s\n",temp[2]);
			int l = strlen(temp[2]);


			for(i=0;i<(l-1);i++)
			{
				safe[i] = temp[2][i];
			}
			//printf("filename--> %s\n",safe);
			//printf("len--> %d\n",strlen(safe));

			bzero(path, 1024);
			strcpy(path, argv[1]);
			printf("path = %s\n", path);
			FILE *fp2 = fopen(strcat(path ,safe), "rb");
			printf("%s\n", path);
			if(fp2 == 0)
			{
				printf("now error\n");
				break;	
			}
			while(1)
			{
				char buffer_download[1024];
				bzero(buffer_download,1024);
				int nread_download = fread(buffer_download,1,1024,fp2);
				//printf("nread--> %d\n",nread_download);
				write(connectionSocket1, buffer_download, nread_download);
				if(nread_download < 1024)
					break;
			}
			printf("over\n");
			close(fp2);
			FILE *file_hash = fopen("file-hash.txt","w");
			unsigned char c[MD5_DIGEST_LENGTH];
			int i;
			FILE *inFile = fopen(path, "rb");
			printf("%s || path = %s\n", argv[1], path);
			MD5_CTX mdContext;
			int bytes;
			unsigned char data[1024];

			if (inFile == NULL) {
				printf ("%s can't be opened.\n", safe);
				break;
			}

			MD5_Init (&mdContext);
			while ((bytes = fread (data, 1, 1024, inFile)) != 0)
				MD5_Update (&mdContext, data, bytes);
			MD5_Final (c,&mdContext);
			for(i = 0; i < MD5_DIGEST_LENGTH; i++)
			{
				fprintf(file_hash,"%02x",c[i]);
				//printf(file_hash,"%02x",c[i]);
			}

			fclose (inFile);
			struct stat attr;
			stat(safe, &attr);
			//strcpy(timer_buffer,ctime(&attr.st_mtime));
			fprintf(file_hash," %s",ctime(&attr.st_mtime));
			fclose(file_hash);

			FILE *fp = fopen("file-hash.txt","rb");
			if(fp==NULL)
			{
				printf("file open error\n");
			}
			while(1)
			{
				unsigned char my_buffer[1024]={0};
				int nread = fread(my_buffer, 1, 1024, fp);
				if(nread < 0)
				{
					printf("read Failure.\n");
					break;
				}
				write(connectionSocket1, my_buffer, nread);
				if(nread<1024)
				{
					//	if(feof(fp))	
					//		printf("end of file.\n");
					//	if(ferror(fp))
					//		printf("error reading.\n");
					break;
				}       
			}
			close(fp);
		}

		else
		{
			char safe[100];
			//char c[1024];
			bzero(safe,100);
			//printf("filename--> %s\n",temp[1]);
			int l = strlen(temp[1]);


			for(i=0;i<(l-1);i++)
			{
				safe[i] = temp[1][i];
			}
			//printf("filename--> %s\n",safe);
			//printf("len--> %d\n",strlen(safe));

			bzero(path, 1024);
			strcpy(path, argv[1]);
			printf("path = %s\n", path);
			FILE *fp2 = fopen(strcat(path ,safe), "rb");
			printf("%s\n", path);
			if(fp2 == 0)
			{
				printf("now error\n");
				break;	
			}
			while(1)
			{
				char buffer_download[1024];
				bzero(buffer_download,1024);
				int nread_download = fread(buffer_download,1,1024,fp2);
				//printf("nread--> %d\n",nread_download);
				write(connectionSocket, buffer_download, nread_download);
				if(nread_download < 1024)
					break;
			}
			printf("over\n");
			close(fp2);
			FILE *file_hash = fopen("file-hash.txt","w");
			unsigned char c[MD5_DIGEST_LENGTH];
			int i;
			FILE *inFile = fopen(path, "rb");
			printf("%s || path = %s\n", argv[1], path);
			MD5_CTX mdContext;
			int bytes;
			unsigned char data[1024];

			if (inFile == NULL) {
				printf ("%s can't be opened.\n", safe);
				break;
			}

			MD5_Init (&mdContext);
			while ((bytes = fread (data, 1, 1024, inFile)) != 0)
				MD5_Update (&mdContext, data, bytes);
			MD5_Final (c,&mdContext);
			for(i = 0; i < MD5_DIGEST_LENGTH; i++)
			{
				fprintf(file_hash,"%02x",c[i]);
				//printf(file_hash,"%02x",c[i]);
			}

			fclose (inFile);
			struct stat attr;
			stat(safe, &attr);
			//strcpy(timer_buffer,ctime(&attr.st_mtime));
			fprintf(file_hash," %s",ctime(&attr.st_mtime));
			fclose(file_hash);

			FILE *fp = fopen("file-hash.txt","rb");
			if(fp==NULL)
			{
				printf("file open error\n");
			}
			while(1)
			{
				unsigned char my_buffer[1024]={0};
				int nread = fread(my_buffer, 1, 1024, fp);
				if(nread < 0)
				{
					printf("read Failure.\n");
					break;
				}
				write(connectionSocket, my_buffer, nread);
				if(nread<1024)
				{
					break;
				}       
			}
			close(fp);
		}

	}
	if(strcmp(temp[0],"FileUpload")==0 || mine==1)
	{
		//mine = 1;
		char safe1[100];
		bzero(safe1,100);
		printf("filename--> %s\n",temp[1]);
		int l = strlen(temp[1]);
		//printf("len--> %d\n",l);
		int i1;
		for(i1=0;i1<(l-1);i1++)
		{
			safe1[i1] = temp[1][i1];
		}
		token = strtok(safe1, "/");
			while( token != NULL ) 
   			{
   				strcpy(safe1, token);
      			printf( "%s\n", safe1 );
      			token = strtok(NULL, "/");
   			}   			
		//printf("filename--> %s\n",safe1);
		bzero(path, 1024);
		strcpy(path, argv[1]);
        printf("path--> %s\n",path);
		FILE *upload_fp = fopen(strcat(path,safe1),"w");
		printf("pathafter--> %s\n",path);
		if(upload_fp==0)
		{
			printf("error while uploading\n");
		}
		char buffer_upload[1024];
		/*strcpy(buffer_upload ,buffer);
		int fuck = recv(connectionSocket,buffer_upload,1023,0);
		printf("fuckvalue - %d\n", &fuck);
		fwrite(buffer_upload, sizeof(char), strlen(buffer_upload), upload_fp);
				printf("buffer length- %d\n", strlen(buffer_upload));
				if(strlen(buffer_upload)  < 1023)
				{

					printf("------------- Upload Finished ------------\n");
					//break;
					mine=0;
				}
				printf("WTF\n");*/
		while(1)
		{
			/*int bytesreceived=0;
			char buffer_upload[1024];
			bzero(buffer_upload,1024);
			while(bytesreceived =  read(connectionSocket, buffer_upload, 1024))
			{
				printf("Bytes received %d.\n",bytesreceived);*/
				int fuck = recv(connectionSocket,buffer_upload,1023,0);
				printf("fuckvalue - %d\n", fuck);
			/*{
				printf("ERROR whiile reading from Client\n");
				printf("Command from Client:");
				int l = strlen(buffer);
				printf("len--> %d",l);
				printf("Here mkil\n");
			}*/

				fwrite(buffer_upload, sizeof(char), fuck, upload_fp);
				printf("buffer length- %d\n", strlen(buffer_upload));
				if(fuck < 1023)
				{


					printf("------------- Upload Finished ------------\n");
					break;
					mine=0;
				}
			//}
				//fwrite(buffer, sizeof(char), strlen(buffer), upload_fp);
				//printf("buffer length2- %d\n", strlen(buffer));
				
			
			//break;

			
		}


		fclose(upload_fp);
		mine=0;
		}

		//else{
		//	break;
		//}
	}
	//break;

	printf("\nClosing connection\n");
	close(connectionSocket);
	close(listenSocket);

	return 0;
}
