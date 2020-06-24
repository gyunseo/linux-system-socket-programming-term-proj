//서버코드
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFER 1460


typedef struct thread_arg{

	int clsock;
	int n;

}Thread_Arg;

void error_handling(char *message);
void* thread_send_video(void* arg);

int main(int argc, char *argv[])
{	
	int i;
	int range;
	pthread_t id_t;//thread id

	int clnt_sock;
	int serv_sock;
	
	

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;


	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	if(serv_sock == -1)
		error_handling("socket() error");
	system("split -d -a 1 -b 100m test_file.mp4 split_file");//file split per 100mb, N=8
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error"); 
		
	if(listen(serv_sock, 10)==-1)
		error_handling("listen() error");

	

	i=0;
	while(1){
		clnt_addr_size=sizeof(clnt_addr);  
		//file send

		
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);

		Thread_Arg* tmp=(Thread_Arg*)malloc(sizeof(Thread_Arg));
		tmp->clsock=clnt_sock;
		tmp->n=i;
		//multiple arg
	
		if(clnt_sock==-1)
			error_handling("accept() error");

		printf("%d th socket connected between server and client\nfd: %d\n",i,clnt_sock);
		printf("client ip: %s\nclient port: %d\n\n",inet_ntoa(clnt_addr.sin_addr),(clnt_addr.sin_port));

		pthread_create(&id_t,NULL,thread_send_video,(void*)tmp);
		pthread_detach(id_t);//separate the thread
		i++;
	}
	
	close(serv_sock);

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void* thread_send_video(void* arg){

	Thread_Arg* tmp=(Thread_Arg*)arg;
	int n=tmp->n;
	int fd,client_sock;
	int read_size;
	char send_buffer[BUFFER];


	char file_name[12]="split_file0";
	file_name[10]=n+48;
	client_sock=tmp->clsock;




	
	
	fd=open(file_name,O_RDONLY);


	if(fd==-1){
		printf("%d\n",n);
		error_handling("file open() error");
	}

	while((read_size=read(fd,send_buffer,BUFFER))!=0){
		
		
		write(client_sock,send_buffer,read_size);
		
	}

		
	if(shutdown(client_sock,SHUT_WR)==-1){
		error_handling("shutdown error!");
	}
	
		
	
	
	close(fd);
	close(client_sock);	
	free(tmp);

	printf("%d th subfile transfer is finished\n",n);
	return NULL;

}
