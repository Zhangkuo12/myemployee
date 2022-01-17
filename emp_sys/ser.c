#include "func.h"
void* recv_cli_msg(void* arg);
int main(int argc, const char *argv[])
{	
	struct init_info ser;                           //初始化相关变量结构体
	sqlite_init(&ser);                              //数据库初始化
	ser_init(&ser);                                 //服务器初始化
	while(1)
	{
		reuse_cli(&ser,recv_cli_msg);               //服务器多路复用
	}
	close(ser.sfd);

	return 0;
}

void* recv_cli_msg(void* arg)
{
	pthread_detach(pthread_self());                 //线程回收          
	ssize_t ret;
	struct init_info ser = *(struct init_info*)arg; 
	struct msg buf;
	while(1)
	{
		MYRECV(ser.newfd,&buf);                     //接收客户端信息
		switch(buf.vn)                              //判断登陆用户权限
		{
		case 'v':                                   //管理员权限操作
			ser_dovip(&ser,&buf);
			break;
		case 'n':
			ser_donarmal(&ser,&buf);                //员工权限操作
			break;
		default:
			ser_doerr(&ser,&buf);
		}
	}
	close(ser.newfd);
	pthread_exit(NULL);
}

