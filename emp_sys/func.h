#ifndef SQLITE_FUNC_H
#define SQLITE_FUNC_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sqlite3.h>
#include <time.h>

#define ERR_MSG(msg) do{\
	printf("line = %d\n",__LINE__);\
	perror(msg);\
}while(0);

#define NAME_LEN 20 
#define PASSWD_LEN 20 
#define TEXT_LEN 512 
 
#define PORT 6667
#define IP "192.168.1.250"

#define EXIT(choose) ({\
	if (choose == 'e')\
		break;})

#define MYSEND(sfd,buf) do{\
	if (send(sfd,buf,sizeof(*buf),0) == -1){\
		ERR_MSG("send");\
		memset(buf,0,sizeof(*buf));\
		return 0;}}\
	while(0)

#define MYRECV(sfd,buf) do{\
		memset(buf,0,sizeof(*buf));\
	if (recv(sfd,buf,sizeof(*buf),0) == -1){\
		ERR_MSG("recv");\
		return 0;}}\
	while(0)


struct init_info{
	sqlite3 *db;
	int reuse;
	int sfd;
	int newfd;
	pthread_t tid;
	struct sockaddr_in sin;
	struct sockaddr_in cin;
	socklen_t addrlen;                         
	char **presult;                                            //sqlite查找后存储
};

struct msg{
	char vn;                                                   //登陆身份
	char ch;                                                   //操作选项
	char yn;                                                   //操作成败
	char name[NAME_LEN];                                       //员工名称
	char passwd[PASSWD_LEN];                                   //登陆密码
	char telphone[12];                                         //电话号码
	char address[20];                                          //家庭住址
	char age[3];                                               //员工年龄
	char money[10];                                            //员工薪资
	char text[50];                                             //信息传递
	char modifi[20];                                           //要修改的元素
}__attribute__((packed));


int cli_init(struct init_info* cli);                           //客户端网络初始化

/*界面处理函数*/
int interface_choose(char *choose);                            //管理员与员工登陆选择界面
int vip_interface_choose(struct init_info *ser,struct msg*buf);//管理员注册与登陆选择界面
int narmal_interface(struct init_info *ser,struct msg*buf);    //员工登陆界面
int vip_choose(struct init_info *ser,struct msg*buf);          //管理员权限使用界面
int narmal_choose(struct init_info *ser,struct msg*buf);       //员工权限使用界面
int ser_dovip(struct init_info*ser,struct msg*buf);            //管理员操作
int ser_donarmal(struct init_info*ser,struct msg*buf);         //普通员工操作
int ser_doerr(struct init_info*ser,struct msg*buf);            //消息传输错误操作

/*信息传输函数*/
int sqlite_init(struct init_info* info);                       //数据库初始化
int ser_init(struct init_info* ser);                           //服务器初始化
int reuse_cli(struct init_info* ser,void* p);                  //服务器多路复用
void* recv_cli_msg(void* arg);                                 //线程回调函数

/*功能实现函数*/
int do_search_sqlite(struct init_info *ser,struct msg* buf);   //数据库查找
int do_search(struct init_info *ser,struct msg* buf);          //查找员工
int do_register(struct init_info *ser,struct msg* buf);        //注册
int do_login(struct init_info *ser,struct msg* buf);           //登陆
int do_quit(struct init_info *ser,struct msg* buf);            //下线
int do_add(struct init_info *ser,struct msg* buf);             //添加
int do_delete(struct init_info *ser,struct msg* buf);          //删除
int do_modifi(struct init_info *ser,struct msg* buf);          //修改
void get_passwd(struct msg*buf);                               //获取用户名和密码
int member_add(struct msg*buf); 
int member_modifi(struct msg*buf);




#endif
