#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
//test_server 
//sudo gcc -o server server.c -lpthread
	int serv_sock;
	int clnt_sock;

void error_handling(char* message);

typedef struct {
	int* clnt_sock;
	char* message;
}recieve_Data;

void* send_function(void* clnt_sock){
	int* cs=(int*)clnt_sock;	
	while(1){
		char message[30];
        int result=0;
		printf("\nserver -> : ");
		scanf("%s",message);
        result = strcmp("exit", message);
        write(*cs,message,sizeof(message));
        if(result == 0){
            printf("exit_ input!\n");
            exit(1);
        }else{
            printf("number : %d\n",result);
        }
	}
}

void* recieve_function(void* rcvdt){
	recieve_Data* data =(recieve_Data*)rcvdt;
	while(1){
		int str_len=read(*(data->clnt_sock),data->message,sizeof(char)*30);	
        int result=0;
		if(str_len!=-1){	
			printf("\n <- client : %s\n",data->message);
            result = strcmp("exit", data->message);
            	
            if(result == 0){
                printf("exit_ input!\n");
                close(clnt_sock);
	            close(serv_sock);
                exit(1);//이건 테스트 해봐야 함
                }
                else{
                    printf("number : %d\n",result);	
                }
		}
	}
}

int main(int argc, char *argv[]){
	char message[30];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;	

	printf("read port....\n");
	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	
	printf("set server socket\n");
    //PF_INET = IPV4 , PFINET6 = IPV6, 
    //SOCK_STREAM 1 //스트림 , TCP 프롤토콜의 전송 방식   
    //SOCK_DGRAM  2 //데이터 그램, UDP 프로토콜의 전송 방식
	serv_sock=socket(PF_INET, SOCK_STREAM,0);//ipv4 소켓 서버 2: IPV4 , 23: IPV6
    int option = 1; // SO_REUSEADDR 의 옵션 값을 TRUE 로
    setsockopt( serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option) );

	if(serv_sock==-1){
		error_handling("socket() error");	
	}

	printf("set server addr...\n");	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;//#define AF_INET       2         //IPv4
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//현재 사용하는 주소를 가져온다. 
	serv_addr.sin_port=htons(atoi(argv[1]));//16bit short로 변환 해야 하니까 atoi 사용 포트 불러오기

	printf("binding...\n");
	if(bind(serv_sock,(struct sockaddr*) &serv_addr,sizeof(serv_addr))==-1){//주소와 포트를 이용해서 bind 시작
		error_handling("bind() error");//에러시 사용
	}
	
	if(listen(serv_sock,5)==-1){
		error_handling("listen() error");
	}	
	printf("waiting...\n");
	
	clnt_addr_size =sizeof(clnt_addr);
	clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
	if(clnt_sock==-1) error_handling("accept() error");
	printf("accept!\n");
	
	recieve_Data rcvdt;
	rcvdt.clnt_sock=&clnt_sock;
	rcvdt.message=message;

	pthread_t p_thread[2];	
	int t;
	int status;
	for(t=0;t<2;t++){
		if(t==0) pthread_create(&p_thread[t],NULL,send_function,(void*)&clnt_sock);//쓰레드 생성 송신용
		else if(t==1)pthread_create(&p_thread[t],NULL,recieve_function,(void*)&rcvdt);//쓰레드 생성 수신용
	}
	
	pthread_join(p_thread[0],(void **)&status);//활성화
	pthread_join(p_thread[1],(void **)&status);//활성화

	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}