#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unistd.h>
using namespace std;
	vector<int> cli_vec;
	int listenfd, port;
	const char *ip = "127.0.0.1";
void SendMsg(char* buf, int connfd){
    //cout<<"now send..."<<endl;
	char *data = buf;
	int len;
	for(auto i=cli_vec.begin(); i!=cli_vec.end(); ++i){
        if(*i == connfd) continue;
		if((len=send(*i, data, strlen(data),0)) == -1){
            //cout<<"send error"<<endl;
		}else{
            //cout<<"Send: "<<data<<endl;
		}
	}
}

void RecvMsg(int connfd){
	int len;
	char buf[100];
	while(1){
		memset(buf, 0, sizeof(buf));
        //cout<<"now receive..."<<connfd<<endl;
		if((len = recv(connfd, buf,sizeof(buf), 0)) == -1){
            //cout<<"Falid to receive"<<endl;
		}
		if(0 == len){
            cout<<connfd<<" has exit the chatting room"<<endl;
			auto pos = find(cli_vec.begin(), cli_vec.end(), connfd);
			if(pos != cli_vec.end()){
				cli_vec.erase(pos);
			}
            break;
		}
        SendMsg(buf,connfd);
	}
}		

void WaitQuit(){
	char buf[100];
	while(1){
		cin>>buf;
		if(0 == strcmp(buf,"q")){
			for(auto i=cli_vec.begin(); i!=cli_vec.end(); i++){
				close(*i);
			}
			cli_vec.clear();
			close(listenfd);
			break;
		}
	}
	exit(0);
}

int main()
{
	if((listenfd=socket(AF_INET,SOCK_STREAM,0)) == -1){
		cout<<"socket error"<<endl;
		return -1;
	}
	cout<<"请输入端口号：";
	cin>>port;
	struct sockaddr_in serv_addr,cli_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port	 = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(ip); 
	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		cout<<"bind error"<<endl;
		return -1;
	}
	if(listen(listenfd, 10) == -1){
		cout<<"listen error"<<endl;
		return -1;
	}	
    std::thread(WaitQuit).detach();
    cout<<"server is running..."<<endl;
	while(1){
		size_t cli_len = sizeof(cli_addr);
		memset(&cli_addr, 0, cli_len);

		int connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_len);
		if(connfd == -1){
			cout<<"accept error"<<endl;
			return -1;
		}
        cout<<connfd<<" has enter the chatting room"<<endl;
		cli_vec.push_back(connfd);
        std::thread(RecvMsg, connfd).detach();
	}
	return 0;
}
		

	
