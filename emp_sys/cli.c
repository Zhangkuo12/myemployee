#include "func.h"

int main(int argc, const char *argv[])
{
	int choose,ret,i = 1;
	struct msg buf;                                   //信息传递结构体
	struct init_info cli;                             //初始化相关变量结构体
	cli_init(&cli);                                    //客户端初始化函数

	while(1)
	{
		interface_choose(&buf.vn);                    //界面选项
		switch(buf.vn){
		case 'v':
			vip_interface_choose(&cli,&buf); 	  		  //管理员登陆与注册界面
			break;
		case 'n':
			narmal_interface(&cli,&buf);					  //普通员工
			break;
		case 'e':
			break;
		default:
			printf("输入错误，请重新输入\n");
			continue;
		}
		EXIT(buf.vn); 								  //判断是否退出当前页面

		if('y' == buf.yn)                              //判断服务器是否成功响应
		{
			switch(buf.vn){                           //判断是管理员还是普通员工
			case 'v':                                 //管理员增删改查功能
				vip_choose(&cli,&buf);
				break;
			case 'n':                                 //普通员工查找和修改功能
				narmal_choose(&cli,&buf);
				break;
			default:
				printf("返回信息出错\n");
			}
		}
	}
	return 0;
}


