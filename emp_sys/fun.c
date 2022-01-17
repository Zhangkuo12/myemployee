#include "func.h"

int cli_init(struct init_info* cli)
{
	cli->sfd = socket(AF_INET,SOCK_STREAM,0);                                 
	if(cli->sfd < 0)
	{
		ERR_MSG("socket");
		return -1;
	}

	if(setsockopt(cli->sfd,SOL_SOCKET,SO_REUSEADDR,&cli->reuse,sizeof(cli->reuse)) < 0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}

	cli->sin.sin_family      = AF_INET;
	cli->sin.sin_port        = htons(PORT);
	cli->sin.sin_addr.s_addr = inet_addr(IP);

	if(connect(cli->sfd,(struct sockaddr*)&cli->sin,sizeof(cli->sin)) < 0)
	{
		ERR_MSG("connect");                                                                      
		return -1;
	}
}

int ser_init(struct init_info* ser)//服务器初始化
{
	ser->sfd = socket(AF_INET,SOCK_STREAM,0);
	if(ser->sfd < 0) {
		ERR_MSG("socket");
		return -1;
	}

	int reuse = 1;
	if(setsockopt(ser->sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0)
	{   
		ERR_MSG("setsockopt");
		return -1;
	}
	ser->sin.sin_family      = AF_INET;
	ser->sin.sin_port        = htons(PORT);
	ser->sin.sin_addr.s_addr = inet_addr(IP);

	if(bind(ser->sfd,(struct sockaddr*)&(ser->sin),sizeof(ser->sin)) < 0){
		ERR_MSG("bind");
		return -1;
	}
	if(listen(ser->sfd,10) < 0){
		ERR_MSG("listen");
		return -1;
	}
	ser->addrlen = sizeof(ser->cin);
}

int reuse_cli(struct init_info* ser,void*recv_cli_msg)
{

	ser->newfd = accept(ser->sfd,(struct sockaddr*)&ser->cin,&ser->addrlen);
	if(ser->newfd < 0)
	{
		ERR_MSG("accept");
		return -1;
	}

	if(pthread_create(&ser->tid,NULL,recv_cli_msg,(void*)ser) < 0)
	{
		ERR_MSG("pthread_create");
		return -1;
	}
}

int interface_choose(char *ch)
{
	printf("***************员工管理系统*******************\n");	
	printf("***************v.管理员***********************\n");	
	printf("***************n.员工*************************\n");	
	printf("***************e.退出*************************\n");	
	printf("**********************************************\n");	
	printf("请输入您的选项>>>");
	scanf("%c",ch);
		while(getchar()!=10);
	return 0;
}

int vip_interface_choose(struct init_info *ser,struct msg*buf)/////////////////
{
	int choose = 0,i;
	buf->vn = 'v';
	while(1)
	{
		buf->yn = 'n';
		printf("***************管理员登陆注册界面**************\n");
		printf("***************r.管理员注册********************\n");
		printf("***************l.管理员登陆********************\n");
		printf("***************e.返回上一级********************\n");
		printf("***********************************************\n");
		printf("请输入您的选项>>>");
		scanf("%c",&buf->ch);
		while(getchar()!=10);
		switch(buf->ch)
		{
		case 'r':
			printf("请输入管理员申请许可号(520)>>>");
			scanf("%d",&i);
		while(getchar()!=10);
			if(520 != i){
				printf("没有管理员申请权限\n");
				i = -1;}
			i=member_add(buf);
			break;
		case 'l':
			get_passwd(buf);
			break;
		case 'e':
			break;
		default:
			printf("输入错误\n");
		}
		if(-1 == i)
			buf->ch = 'e';
		EXIT(buf->ch);//返回上一级
		MYSEND(ser->sfd,buf);
		memset(buf,0,sizeof(buf));
		MYRECV(ser->sfd,buf);
		if(buf->vn == 'n')
		{
			printf("当前页面无法登陆普通员工\n");
			EXIT('e');
		}
		if((buf->yn=='y')&&(buf->ch == 'l'))
		{
			printf("登陆成功，进入管理员权限界面\n");
			break;
		}
		printf("%s\n",buf->text);
	}
}
void get_passwd(struct msg*buf)
{
	printf("请输入您的姓名>>>");
	scanf("%s",buf->name);
	while(getchar()!=10);
	printf("请输入您的密码>>>");
	scanf("%s",buf->passwd);
	while(getchar()!=10);
}

int narmal_interface(struct init_info *ser,struct msg*buf)/////////////
{
	buf->vn = 'n';
	while(1)
	{
		printf("***************员工登陆界面********************\n");
		printf("***************l.员工登陆**********************\n");
		printf("***************e.返回上一级********************\n");
		printf("***********************************************\n");
		printf("请输入您的选项>>>");
		scanf("%c",&buf->ch);
		while(getchar() != 10);
		switch(buf->ch)
		{
		case 'l':
			get_passwd(buf);
			break;
		case 'e':
			break;
		default:
			printf("输入错误\n");
		}
		EXIT(buf->ch);                                //返回上一级
		MYSEND(ser->sfd,buf);
		MYRECV(ser->sfd,buf);
		if(buf->vn == 'v')
		{
			printf("当前界面无法登陆管理员账号\n");
			EXIT('e');
		}
		if((buf->yn=='y')&&(buf->ch == 'l'))
		{
			printf("登陆成功了，进入员工权限界面\n");
			break;
		}
		printf("%s",buf->text);
	}
}
int member_add(struct msg*buf)
{
	printf("请输入名称>>>");
	scanf("%s",buf->modifi);
	while(getchar()!=10);

	printf("请输入密码>>>");
	scanf("%s",buf->passwd);
	while(getchar()!=10);
	
	printf("请输入电话>>>");
	scanf("%s",buf->telphone);
	while(getchar()!=10);
	
	printf("请输入地址>>>");
	scanf("%s",buf->address);
	while(getchar()!=10);
	
	printf("请输入年龄>>>");
	scanf("%s",buf->age);
	while(getchar()!=10);
	printf("请输入薪资>>>");
	scanf("%s",buf->money);
	while(getchar()!=10);
	return 0;
}


int member_modfi(struct msg*buf)
{
	printf("注意!\n");
	printf("管理员能修改name,passwd,telphone,address,age,money\n");
	printf("员工只能修改name,passwd,telphone,address\n");
	printf("请输入要修改的项目>>>");
	scanf("%s",buf->modifi);
	while(getchar()!=10);
	if(((strcmp(buf->modifi,"money")==0)||(strcmp(buf->modifi,"age")==0))&&('v'!=buf->vn))
	{
		strcpy(buf->text,"您没有修改该项目的权限,您的权限为");
		return -1;
	}
	if((strcmp(buf->modifi,"passwd")!=0)&&(strcmp(buf->modifi,\
		"telphone")!=0)&&(strcmp(buf->modifi,"address")!=0)&&(strcmp(buf->modifi,"money")!=0)\
		&&(strcmp(buf->modifi,"age")!=0))
	{
		strcpy(buf->text,"不存在您想要修改的项目");
		return -1;
	}
	printf("您想要修改为>>>");
	scanf("%s",buf->text);
	while(getchar()!=10);

}
int vip_choose(struct init_info *ser,struct msg*buf)
{
	int i=0;
	char pname[20]="";
	while(1){
		printf("******************管理员界面*******************\n");
		printf("******************a.增加员工*******************\n");
		printf("******************d.删除员工*******************\n");
		printf("******************m.修改员工*******************\n");
		printf("******************i.查寻员工*******************\n");
		printf("******************e.返回上一级*****************\n");
		printf("***********************************************\n");
		printf("请输入您的选项>>>");
		scanf("%c",&buf->ch);
	while(getchar()!=10);
		switch(buf->ch){
		case 'a':
			i = member_add(buf);                        //增
			break;
		case 'd':
			printf("请输入要删除的员工名称>>>");                  //删
			scanf("%s",buf->modifi);
	while(getchar()!=10);
			break;
		case 'm':
			strcpy(pname,buf->name);
			printf("请输入要修改员工名称>>>");
			scanf("%s",buf->name);
			while(getchar()!=10);
			member_modfi(buf);                      //改
			break;
		case 'i':
			printf("请输入要查询的员工名称>>>");                     //删
			scanf("%s",buf->modifi);
			while(getchar()!=10);
			break;
		case 'e':
			break;                               //退出
		default:
			printf("输入错误，请重新输入\n");   
			continue;
		}
		if (i == -1)
		{	buf->ch = 'e';
			EXIT(buf->ch);}
		MYSEND(ser->sfd,buf);
		MYRECV(ser->sfd,buf);
		if(buf->ch == 'm')
			strcpy(buf->name,pname);
		EXIT(buf->ch);
		if((buf->ch == 'i')&&(buf->yn == 'y'))
		{
			printf("查询到的结果为:\n");
			printf("姓名：%s   年龄：%s   电话：%s   地址：%s  薪资：%s\n",\
					buf->text,buf->age,buf->telphone,buf->address,buf->money);
		}
	}
}

int narmal_choose(struct init_info *ser,struct msg*buf)
{
	while(1)
	{
		printf("***************员工界面************************\n");
		printf("***************i.查询员工信息******************\n");
		printf("***************m.修改员工信息******************\n");
		printf("***************e.返回上一级********************\n");
		printf("***********************************************\n");
		printf("请输入您的选项>>>");
		scanf("%c",&buf->ch);
		while(getchar()!=10);
		switch(buf->ch)
		{
		case 'i':
			strcpy(buf->modifi,buf->name);
			break;
		case 'm':
			member_modfi(buf);
			break;
		case 'e':
			break;
		default:
			printf("输入错误，请重新输入\n");
		}
		MYSEND(ser->sfd,buf);
		MYRECV(ser->sfd,buf);
		EXIT(buf->ch);
		if((buf->ch == 'i')&&(buf->yn == 'y'))
		{
			printf("查询到的结果为:\n");
			printf("姓名：%s   年龄：%s   电话：%s   地址：%s  薪资：%s\n",\
					buf->text,buf->age,buf->telphone,buf->address,buf->money);
		}
	}
}


int sqlite_init(struct init_info* info)//数据库初始化
{
	if(sqlite3_open("./employ.db",&info->db) != 0)
	{
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(info->db));
		return -1;
	}
	char sql[256] = "create table if not exists employee(vn char,\
					 name char, passwd char, telphone char, \
					 address char, age char, money char,flag int);";
	char *errmsg = NULL;
	if(sqlite3_exec(info->db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		printf("sqlite3_exec:%s __%d__",errmsg,__LINE__);
		return -1;
	}
}

int ser_dovip(struct init_info*ser,struct msg*buf)
{
	switch(buf->ch)
	{
	case 'r':
		do_register(ser,buf);
		break;
	case 'l':
		do_login(ser,buf);
		break;
	case 'e':
		do_quit(ser,buf);
		break;
	case 'a':
		buf->vn = 'n';
		printf("员工注册\n");
		do_register(ser,buf);
		buf->vn = 'v';
		break;
	case 'd':
		do_delete(ser,buf);
		break;
	case 'm':
		do_modifi(ser,buf);
		break;
	case 'i':
		do_search(ser,buf);
		break;
	default:
		printf("消息传输错误\n");
	}
	MYSEND(ser->newfd,buf);
}


int ser_donarmal(struct init_info*ser,struct msg*buf)
{
	switch(buf->ch)
	{
	case 'l':
		do_login(ser,buf);
		break;
	case 'i':
		do_search(ser,buf);
		printf("%s\n",buf->text);
		break;
	case 'm':
		do_modifi(ser,buf);
	default:
		printf("消息传输错误");
	}
	MYSEND(ser->newfd,buf);
}

int ser_doerr(struct init_info*ser,struct msg*buf)
{
	printf("客户端传输员工权限信息错误\n");
	strcpy(buf->text,"服务器未识别信号\n");
	MYSEND(ser->newfd,buf);
}

int do_search(struct init_info *ser,struct msg* buf)
{	

	if(do_search_sqlite(ser,buf) == 0)
	{
		strcpy(buf->text,ser->presult[1]);
		strcpy(buf->passwd,ser->presult[2]);
		strcpy(buf->telphone,ser->presult[3]);
		strcpy(buf->address,ser->presult[4]);
		strcpy(buf->age,ser->presult[5]);
		strcpy(buf->money,ser->presult[6]);
		buf->yn = 'y';
		return 0;
	}
	else
	{
		buf->yn = 'n';
		strcpy(buf->text,"该用户不存在");
		return -1;
	}
}

int do_search_sqlite(struct init_info *ser,struct msg* buf)
{
	char sql[128] = "";
	strcpy(sql,"select * from employee;");
	char *errmsg;
	int line,list,j = 0,row,column;
	ser->presult =NULL;
	if(sqlite3_get_table(ser->db,sql,&ser->presult,&row,&column,&errmsg) != 0)
	{
		printf("sqlite3_get_table:%s __%d__\n",errmsg,__LINE__);
		return -1;
	}
	for(line = 0;line < (row+1);line++)
	{
		for(list = 0;list < column;list++)
		{
			if(strcmp(ser->presult[j],buf->modifi) == 0)
			{
				ser->presult = &ser->presult[j-1];
				return 0;
			}
			j++;
		}
	}
	return -1;
}

int do_register(struct init_info *ser,struct msg* buf)
{
	if(do_search(ser,buf) == 0)
	{
		strcpy(buf->text,"该用户已经存在，无法注册");
		printf("重复注册，操作失败\n");
		return -1;
	}
	else
	{
		do_add(ser,buf);
		buf->yn = 'y';
		strcpy(buf->text,"注册成功");
	}
	printf("用户注册成功\n");
}

int do_login(struct init_info *ser,struct msg* buf)
{
	char sql[128] = "";
	char** errmsg = NULL;
	printf("%s请求登陆\n",buf->name);
	strcpy(buf->modifi,buf->name);
	if(do_search(ser,buf) == 0)
	{
		if(strcmp(ser->presult[7],"1") == 0)
		{
			buf->yn = 'n';
			strcpy(buf->text,"登陆失败，其他用户登陆了该账号\n");
			printf("账号重复登陆，操作失败\n");
			return -1;
		}

		if(strcmp(ser->presult[2],buf->passwd) == 0)
		{

			buf->yn = 'y';
			sprintf(sql,"update employee set flag=\"1\" where name=\"%s\"",buf->name);
			if(sqlite3_exec(ser->db,sql,NULL,NULL,errmsg) != 0)
			{
				ERR_MSG("sqlite3_exec");
				return -1;
			}
			strcpy(buf->text,"登陆成功");
			printf("用户%s登陆成功\n",buf->name);
		}
		else
		{
			buf->yn = 'n';
			strcpy(buf->text,"密码错误");
		}
	}
	else
	{
		buf->yn = 'n';
		strcpy(buf->text,"该用户不存在");
	}
	return 0;
}

int do_quit(struct init_info *ser,struct msg* buf)
{

	char sql[128] = "";
	char** errmsg = NULL;
	sprintf(sql,"update employee set flag=\"0\" where name=\"%s\"",buf->name);
	printf("%s下线\n",buf->name);
	if(sqlite3_exec(ser->db,sql,NULL,NULL,errmsg) != 0)
	{
		ERR_MSG("sqlite3_exec");
		return -1;
	}
	return 0;
}

int do_add(struct init_info *ser,struct msg* buf)

{
	char *errmsg = NULL;
	//调用sqlite3_exec函数	
	char sql[200] = "";
	sprintf(sql,"insert into employee values(\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",'0')",\
			buf->vn,buf->modifi,buf->passwd,buf->telphone,buf->address,buf->age,buf->money);

	if(sqlite3_exec(ser->db,sql,NULL,NULL,&errmsg) != 0)
	{
		printf("sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
		return -1;
	}

	buf->yn = 'y';
	strcpy(buf->text,"添加员工成功");
	printf("添加员工成功\n");

	return 0;
}

int do_delete(struct init_info *ser,struct msg* buf)
{	
	if(do_search(ser,buf) == 0)
	{
		char *errmsg = NULL;
		char sql[128] = "";
		sprintf(sql,"delete from employee where name = \"%s\"",buf->modifi);

		if(sqlite3_exec(ser->db,sql,NULL,NULL,&errmsg) != 0)
		{
			buf->yn = 'n';
			printf("sqlite3_exec:%s  ***__%d__\n",errmsg,__LINE__);
			return -1;
		}
		buf->yn = 'y';
		printf("删除数据成功\n");
	}
	else{
		printf("您要删除的对象不存在！\n");
		buf->yn = 'n';
	}
	return 0;
}



int do_modifi(struct init_info *ser,struct msg* buf)
{
	char *errmsg = NULL;
	char sql[256] = "";
	sprintf(sql,"update employee set \"%s\"=\"%s\" where name = \"%s\"",\
			buf->modifi,buf->text,buf->name);
	if(sqlite3_exec(ser->db,sql,NULL,NULL,&errmsg) != 0)
	{
		buf->yn = 'n';
		return -1;
	}
	buf->yn = 'y';
	printf("修改数据成功\n");
}

