//클라이언트 코드
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
#include<time.h>

#define BUFFER 1460
#define N 8
typedef struct thread_arg{

	int clsock;
	int n;

}Thread_Arg;

void error_handling(char *message);
void* thread_receive_video(void* arg);

int main(int argc, char* argv[])
{
	
	pthread_t id_t[N];//thread id
	int i,sock;
	clock_t start,end;
	struct sockaddr_in serv_addr;
	
	if(argc!=3){

		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	start=clock();//time start
	for(i=0;i<N;i++){
		
		

		sock=socket(PF_INET, SOCK_STREAM, 0);

		if(sock == -1)
			error_handling("socket() error");
		
		
		if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
			error_handling("connect() error!");


		printf("%d th client socket connect to server, file descriptor: %d\n",i,sock);
		Thread_Arg* tmp=(Thread_Arg*)malloc(sizeof(Thread_Arg));
		tmp->clsock=sock;
		tmp->n=i;
		//multiple arg

		pthread_create(&id_t[i],NULL,thread_receive_video,(void*)tmp);
		
	}
	
	for(i=0;i<N;i++){
		pthread_join(id_t[i],NULL);
	}
	
	system("cat receive_file* > receive.mp4");
	system("chmod u+r receive.mp4");
	system("cmp test_file.mp4 receive.mp4");//파일이 같은지 확인하는 명령어, 아무런 출력값이 없으면, 파일이 원본과 같은 것
	end=clock();//time end
	printf("transfer duration: %.3lf sec\t%.3lf milli sec\n",(double)((end-start)/CLOCKS_PER_SEC),(double)(end-start));
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
void* thread_receive_video(void* arg){

	Thread_Arg* tmp=(Thread_Arg*)arg;
	int n=tmp->n;
	int sock=tmp->clsock;//client itself
	int fd;
	
	int read_size;
	char input_buffer[BUFFER];
	char src_file_name[12]="split_file0";
	char file_name[14]="receive_file0";
	char test_cmd[40]="cmp ";
	
	

	file_name[12]=n+48;
	src_file_name[10]=n+48;
	fd=open(file_name,O_WRONLY|O_TRUNC|O_CREAT);//receive mp4 file


	


	//file send




	while((read_size=read(sock,input_buffer,BUFFER))!=0){
		
			write(fd,input_buffer,read_size);
	}
	
	close(fd);
	close(sock);


	strcat(test_cmd,file_name);
	strcat(test_cmd," ");
	strcat(test_cmd,src_file_name);
	

	system("chmod 777 receive_file*");
	system(test_cmd);

}
