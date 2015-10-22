#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{
	int ClientSocket = 0;
	struct sockaddr_in serv_addr;
	char *token;
	
	char path[1024];

	// Creating a socket
	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(ClientSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return 0;
	}
	else
		printf("[CLIENT] Socket created \n");

	int portno = 5005;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Connection Establishment

	while(connect(ClientSocket,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0);
	int bytesreceived=0;
	while(1)
	{
		char buffer[1024];
		bzero(buffer,1024);
		printf("$>");
		fgets(buffer,1023,stdin);
		if(send(ClientSocket,buffer,strlen(buffer),0)<0)
			printf("ERROR while writing to the socket\n");

		/*---------------------------------------Splitting the input commands--------------------------------------*/

		char temp[10][100];
		int i,j;
		for(i = 0; i<10; i++)
		{
  			temp[i][0] = '\0';
		}
		int ne=0,t=0;
		int flag=0;
		FILE *request;
		
		for(i=0;i<10;i++)
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
		/*------------------Storing the history--------------------------*/

		request = fopen("./.requests.txt","ab");
		if(request == NULL)
		{
			printf("Error opening the requests file\n");
		}
		fprintf(request,"%s",buffer);
		fclose(request);

		/*---------------------------IndexGet -----------------------------*/
		if(strcmp(temp[0],"IndexGet")==0)
		{
			if(strcmp(temp[1],"--shortlist")==0)
			{
				//shortlist
				char file_buffer[1024],ch;
				FILE *index_file;
				index_file = fopen("file-storage.txt","ab");
				if(index_file==NULL)
				{
					printf("Error opening the file.\n");
				}
				while(bytesreceived =  read(ClientSocket, file_buffer, 1024))
				{
					printf("Bytes received %d.\n",bytesreceived);
					fwrite(file_buffer, 1, bytesreceived, index_file);
					if(bytesreceived <1024)
					{
						break;
					}
				}
				printf("transfer success\n");
				fclose(index_file);
				index_file = fopen("file-storage.txt","rb");
				while( ( ch = fgetc(index_file) ) != EOF )
				{
					if(ch=='\n')
					{
						printf("\n");
					}
					else
						printf("%c",ch);
				}
				fclose(index_file);
				int status = remove("file-storage.txt");
				
			}
			if(strcmp(temp[1],"--longlist\n")==0)
			{
				//longlist
				char file_buffer[1024],ch;
				FILE *index_file;
				index_file = fopen("file-storage_long.txt","ab");
				if(index_file==NULL)
				{
					printf("Error opening the file.\n");
				}
				while(bytesreceived =  read(ClientSocket, file_buffer, 1024))
				{
					printf("Bytes received %d.\n",bytesreceived);
					fwrite(file_buffer, 1, bytesreceived, index_file);
					if(bytesreceived <1024)
					{
						break;
					}
				}
				printf("transfer success\n");
				fclose(index_file);
				index_file = fopen("file-storage_long.txt","rb");
				while( ( ch = fgetc(index_file) ) != EOF )
				{
					if(ch=='\n')
					{
						printf("\n");
					}
					else
						printf("%c",ch);
				}
				fclose(index_file);
				int status = remove("file-storage_long.txt");
			}
			if(strcmp(temp[1],"--regex")==0)
			{
				//regex
				char file_buffer[1024],ch;
				FILE *index_file;
				index_file = fopen("file-storage_long.txt","ab");
				if(index_file==NULL)
				{
					printf("Error opening the file.\n");
				}
				while(bytesreceived =  read(ClientSocket, file_buffer, 1024))
				{
					printf("Bytes received %d.\n",bytesreceived);
					fwrite(file_buffer, 1, bytesreceived, index_file);
					if(bytesreceived <1024)
					{
						break;
					}
				}
				printf("transfer success\n");
				fclose(index_file);
				index_file = fopen("file-storage_long.txt","rb");
				while( ( ch = fgetc(index_file) ) != EOF )
				{
					if(ch=='\n')
					{
						printf("\n");
					}
					else
						printf("%c",ch);
				}
				fclose(index_file);
				int status = remove("file-storage_long.txt");
				printf("                                         ================END================                                                    \n");
			}

		}

		if(strcmp(temp[0],"FileHash")==0)
		{
			if(strcmp(temp[1],"--verify")==0)
			{
				printf("entered\n");
				int bytesreceived=0;
				char hash_buffer[1024],ch;
				FILE *hash_file;
				hash_file = fopen("file-hash.txt","ab");
				if(hash_file==NULL)
				{
					printf("Error opening the file.\n");
				}
				while(bytesreceived =  read(ClientSocket, hash_buffer, 1024))
				{
					printf("Bytes received %d.\n",bytesreceived);
					fwrite(hash_buffer, 1, bytesreceived, hash_file);
					if(bytesreceived <1024)
					{
						break;
					}
				}
				printf("transfer success\n");
				fclose(hash_file);
				hash_file = fopen("file-hash.txt","rb");
				while( ( ch = fgetc(hash_file) ) != EOF )
				{
					if(ch=='\n')
					{
						printf("\n");
					}
					else
						printf("%c",ch);
				}
				fclose(hash_file);
				int status = remove("file-hash.txt");
			}
			else if(strcmp(temp[1],"--checkall\n")==0)
			{
				char file_buffer[1024],ch;
				FILE *index_file;
				index_file = fopen("file-long-hash.txt","ab");
				if(index_file==NULL)
				{
					printf("Error opening the file.\n");
				}
							int lost, findHere, lether;
				int vors[5];
			for (lost = 0 ; lost < ( 5 - 1 ); lost++)
  {
    for (lether = 0 ; lether < 5 - lost - 1; lether++)
    {
      if (vors[lether] > vors[lether+1]) 
      {
        findHere       = vors[lether];
        vors[lether]   = vors[lether+1];
        vors[lether+1] = findHere;
      }
    }
  }
				while(bytesreceived =  read(ClientSocket, file_buffer, 1024))
				{
				//	printf("Bytes received %d.\n",bytesreceived);
					fwrite(file_buffer, 1, bytesreceived, index_file);
					if(bytesreceived <1024)
					{
						break;
					}
				}
				//printf("transfer success\n");
				fclose(index_file);
				index_file = fopen("file-long-hash.txt","rb");
				while( ( ch = fgetc(index_file) ) != EOF )
				{
					if(ch=='\n')
					{
						printf("\n");
					}
					else
						printf("%c",ch);
				}
				fclose(index_file);
				int status = remove("file-long-hash.txt");
			}
		}



		if(strcmp(temp[0],"FileDownload")==0)
		{
			if(strcmp(temp[1],"TCP")==0 || strcmp(temp[1],"UDP")==0)
                        {
                                int ClientSocket1 = 0;
                                struct sockaddr_in serv_addr1;
 
        // Creating a socket
                                if(strcmp(temp[1],"--TCP")==0)
                                        ClientSocket1 = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
                                else if(strcmp(temp[1],"--UDP")==0)
                                        ClientSocket1 = socket(AF_INET,SOCK_STREAM,IPPROTO_UDP);
 
                                if(ClientSocket1<0)
                                {
                                        printf("ERROR WHILE CREATING A SOCKET\n");
                                        return 0;
                                }
                                else
                                        printf("[CLIENT] Socket created \n");
 
                                int portno = 5004;
 
                                serv_addr1.sin_family = AF_INET;
                                serv_addr1.sin_port = htons(portno);
                                serv_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");
 
        //Connection Establishment
 
                                while(connect(ClientSocket1,(struct sockaddr *)&serv_addr1,sizeof(serv_addr1))<0);

                                char matter[1024];
			char safe[100];
			int i;
			int l=strlen(temp[2]);
			printf("len--> %d\n",l);
			for(i=0;i<l-1;i++)
			{
				safe[i]=temp[2][i];
			}
			token = strtok(safe, "/");
			int server_VAAl;
			int temp_VAAl = 5;

			while( token != NULL ) 
   			{
   				strcpy(safe, token);
   				int jjj;
   				for (jjj = 0; jjj < 3; jjj++){
				server_VAAl *= temp_VAAl; 
			}

      			printf( "%s\n", safe );
      			token = strtok(NULL, "/");

   			}   			
   			//strcpy(safe, token);
   			printf("safe = %s\n",safe );
			FILE *download_fp = fopen(safe,"ab");
			printf("file downloading --> %s\n",safe);
			while(bytesreceived =  read(ClientSocket1, matter, 1024))
			{
				//printf("Bytes received %d.\n",bytesreceived);
				fwrite(matter, 1, bytesreceived, download_fp);
				if(bytesreceived < 1024)
				{
					break;
				}
			}
			printf("---	------- Download Finished ----------\n");
			fclose(download_fp);
			int bytesreceived=0;
			char hash_buffer[1024],ch;
			FILE *hash_file;
			hash_file = fopen("file-hash.txt","ab");
			if(hash_file==NULL)
			{
				printf("Error opening the file.\n");
			}
			while(bytesreceived =  read(ClientSocket1, hash_buffer, 1024))
			{
				printf("Bytes received %d.\n",bytesreceived);
				fwrite(hash_buffer, 1, bytesreceived, hash_file);
				if(bytesreceived <1024)
				{
					int server_VAl;
					int temp_VAl = 5;
					int msa;
					for (msa = 0; msa < 3; msa++){
					server_VAl *= temp_VAl; 
					}

					break;
				}
			}
			printf("transfer success\n");
			fclose(hash_file);
			hash_file = fopen("file-hash.txt","rb");
			while( ( ch = fgetc(hash_file) ) != EOF )
			{
				if(ch=='\n')
				{
					printf("\n");
				}
				else
					printf("%c",ch);
			}
			fclose(hash_file);
			int status = remove("file-hash.txt");
		


 			}

			else
			{
			char matter[1024];
			char safe[100];
			if(strcmp(temp[1], "--TCP") == 0 || strcmp(temp[1], "--UDP" )== 0){
				strcpy(temp[1], temp[2]);
			}
			int i;
			int l=strlen(temp[1]);
			//printf("len--> %d\n",l);
			for(i=0;i<l-1;i++)
			{
				safe[i]=temp[1][i];
			}
			token = strtok(safe, "/");
			while( token != NULL ) 
   			{
   				strcpy(safe, token);
      			printf( "%s\n", safe );
      			token = strtok(NULL, "/");
   			}   			
   			//strcpy(safe, token);
   			//printf("safe = %s\n",safe );
   			bzero(path, 1024);
		strcpy(path, argv[1]);

			FILE *download_fp = fopen(strcat(path,safe),"ab");
			//printf("file downloading --> %s\n",safe);
			while(bytesreceived =  read(ClientSocket, matter, 1024))
			{
				//printf("Bytes received %d.\n",bytesreceived);
				fwrite(matter, 1, bytesreceived, download_fp);
				if(bytesreceived < 1024)
				{
					break;
				}
			}
			printf("------------- Download Finished ------------\n");
			fclose(download_fp);
			int bytesreceived=0;
			char hash_buffer[1024],ch;
			FILE *hash_file;
			hash_file = fopen("file-hash.txt","ab");
			if(hash_file==NULL)
			{
				printf("Error opening the file.\n");
			}
			while(bytesreceived =  read(ClientSocket, hash_buffer, 1024))
			{
				printf("Bytes received %d.\n",bytesreceived);
				fwrite(hash_buffer, 1, bytesreceived, hash_file);
				if(bytesreceived <1024)
				{
					break;
				}
			}

			int server_Val;
int temp_Val = 5;
int iter;
for (iter = 0; iter < 9; iter++){
	server_Val *= temp_Val; 
}

			//printf("transfer success\n");
			fclose(hash_file);
			hash_file = fopen("file-hash.txt","rb");
			while( ( ch = fgetc(hash_file) ) != EOF )
			{
				if(ch=='\n')
				{
					printf("\n");
				}
				else
					printf("%c",ch);
			}
			fclose(hash_file);
			int status = remove("file-hash.txt");
		}
		}
		if(strcmp(temp[0],"FileUpload")==0)
		{
			
if(strcmp(temp[1],"TCP")==0 || strcmp(temp[1],"UDP")==0)
                        {
                                int ClientSocket1 = 0;
                                struct sockaddr_in serv_addr1;
 
        // Creating a socket
                                if(strcmp(temp[1],"--TCP")==0)
                                        ClientSocket1 = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
                                else if(strcmp(temp[1],"--UDP")==0)
                                        ClientSocket1 = socket(AF_INET,SOCK_STREAM,IPPROTO_UDP);
 
                                if(ClientSocket1<0)
                                {
                                        printf("ERROR WHILE CREATING A SOCKET\n");
                                        return 0;
                                }
                                else
                                        printf("[CLIENT] Socket created \n");
 
                                int portno = 5004;
 
                                serv_addr1.sin_family = AF_INET;
                                serv_addr1.sin_port = htons(portno);
                                serv_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");
 
        //Connection Establishment
 
                                while(connect(ClientSocket1,(struct sockaddr *)&serv_addr1,sizeof(serv_addr1))<0);

                                char matter[1024];
			char safe[100];
			int i;
			int l=strlen(temp[2]);
			printf("len--> %d\n",l);
			for(i=0;i<l-1;i++)
			{
				safe[i]=temp[2][i];
			}
			token = strtok(safe, "/");
			int server_VAAl;
			int temp_VAAl = 5;

			while( token != NULL ) 
   			{
   				strcpy(safe, token);
   				int jjj;
   				for (jjj = 0; jjj < 3; jjj++){
				server_VAAl *= temp_VAAl; 
			}

      			printf( "%s\n", safe );
      			token = strtok(NULL, "/");

   			}   			
   			//strcpy(safe, token);
   			printf("safe = %s\n",safe );
			FILE *download_fp = fopen(safe,"ab");
			printf("file downloading --> %s\n",safe);
			while(bytesreceived =  read(ClientSocket1, matter, 1024))
			{
				//printf("Bytes received %d.\n",bytesreceived);
				fwrite(matter, 1, bytesreceived, download_fp);
				if(bytesreceived < 1024)
				{
					break;
				}
			}
			printf("---	------- Download Finished ----------\n");
			fclose(download_fp);
			int bytesreceived=0;
			char hash_buffer[1024],ch;
			FILE *hash_file;
			hash_file = fopen("file-hash.txt","ab");
			if(hash_file==NULL)
			{
				printf("Error opening the file.\n");
			}
			while(bytesreceived =  read(ClientSocket1, hash_buffer, 1024))
			{
				printf("Bytes received %d.\n",bytesreceived);
				fwrite(hash_buffer, 1, bytesreceived, hash_file);
				if(bytesreceived <1024)
				{
					int server_VAl;
					int temp_VAl = 5;
					int msa;
					for (msa = 0; msa < 3; msa++){
					server_VAl *= temp_VAl; 
					}

					break;
				}
			}
			printf("transfer success\n");
			fclose(hash_file);
			hash_file = fopen("file-hash.txt","rb");
			while( ( ch = fgetc(hash_file) ) != EOF )
			{
				if(ch=='\n')
				{
					printf("\n");
				}
				else
					printf("%c",ch);
			}
			fclose(hash_file);
			int status = remove("file-hash.txt");
		


 			}


else{
			char safe1[100];
			int i1;
			if(strcmp(temp[1], "--TCP") == 0 || strcmp(temp[1], "--UDP" )== 0){
				strcpy(temp[1], temp[2]);
			}
			int l1=strlen(temp[1]);
			//printf("len--> %d\n",l1);

			bzero(safe1, 100);
			for(i1=0;i1<l1-1;i1++)
			{
				safe1[i1]=temp[1][i1];
			}

			char buffer_upload[1024];
			bzero(path, 1024);
			strcpy(path, argv[1]);
			printf("path--> %s\n",path);
			FILE *fp2 = fopen(strcat(path,safe1),"rb");
			printf("pathafter--> %s\n",path);
			if(fp2 == 0)
			{
				        printf("now error\n");
				        int status = remove(path);
				        break;
			}
			while(1)
			{
				char buffer_upload[1024];
				bzero(buffer_upload,1024);
				int nread_upload = fread(buffer_upload,1,1024,fp2);
				write(ClientSocket, buffer_upload, nread_upload);
				if(nread_upload <= 0 )
				{
					//int nread_upload1 = fread(buffer_upload,1,1024,fp2);
					//write(ClientSocket, buffer_upload, nread_upload1);
					
					printf("----------Upload Finished------------\n");
					break;
				}
			}
			close(fp2);
		
}


		}
		else{
			printf("---------------------------------------------------------------------------\n");
			printf("NOTE:The format of commands should be as follows : \n");
			printf("\t 1. IndexGet ­­shortlist <start­time­stamp> <end­time­stamp>\n");
			printf("\t 2. IndexGet ­­longlist\n");
			printf("\t 3. IndexGet ­­regex <regex­argument>\n");
			printf("\t 4. FileHash ­­verify <filename>\n");
			printf("\t 5. FileHash ­­checkall\n");
			printf("\t 6. FileDownload <filename>\n");
			printf("\t 7. FileUpload <filename>\n");

			printf("\n* Commands are CASE sensitive\n");
			printf("---------------------------------------------------------------------------\n");

		}
	}

	printf("Closing Connection\n");
	close(ClientSocket);
	return 0;
}
