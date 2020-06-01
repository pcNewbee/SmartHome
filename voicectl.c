//////////////////////////////////////////////////////////////////
//
//  Copyright(C), 2013-2017, GEC Tech. Co., Ltd.
//
//  File name: GPLE/voicectl.c
//
//  Author: GEC
//
//  Date: 2017-01
//  
//  Description: 获取语音指令，根据指令完成相应动作
//
//
//////////////////////////////////////////////////////////////////

#include "common.h"
#include <sys/ioctl.h>
  
#define TEST_MAGIC 'x'                           //定义幻数
#define TEST_MAX_NR 2                            //定义命令的最大序数

//定义LED的魔幻数
#define LED1 _IO(TEST_MAGIC, 0)              
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3)



//#define REC_CMD  "./arecord -d4 -c1 -r16000 -traw -fS16_LE cmd.pcm"
#define REC_CMD  "arecord -d3 -c1 -r16000 -traw -fS16_LE cmd.pcm"
#define PCM_FILE "./cmd.pcm"
/* -d：录音时长（duration）
-c：音轨（channels）
-r：采样频率（rate）
-t：封装格式（type）
-f：量化位数（format） */
int said=0;

void catch(int sig)
{
	if(sig == SIGPIPE)
	{
		printf("killed by SIGPIPE\n");
		exit(0);
	}
}


int main(int argc, char const *argv[]) // ./wav2pcm ubuntu-IP
{

	signal(SIGPIPE, catch);
	int id_num=0;
	if(argc != 2)
	{
		printf("Usage: %s <ubuntu-IP>\n", argv[0]);
		exit(0);
	}

	int sockfd = init_sock(argv[1]); //tcp
	
	while(1)
	{
		bool music=false;
		// 1，调用arecord来录一段音频
		printf("please to start REC in 3s...\n");

		// 在程序中执行一条命令  “录音的命令”
		system(REC_CMD);

		// 2，将录制好的PCM音频发送给语音识别引擎
		send_pcm(sockfd, PCM_FILE);

		// 3，等待对方回送识别结果（字符串ID）
		xmlChar *id = wait4id(sockfd);
		
		if(id == NULL)
		{
			continue;
		}
		id_num=atoi((char *)id);
		if(id_num == 999)
		{
			printf("bye-bye!\n");
			system("madplay  mp3/bye.mp3"); // 放一个语音文件
			goto exit;
		}
		if(id_num == 100 )
		{
			printf("你好，主人！有什么吩咐\n");
			said = 1;  //唤醒它
			system("madplay  mp3/online.mp3"); // 放一个语音文件
			
		}
		if(said == 1 && id_num != 100 )
		{
			send_data_to_8266(id_num);

			printf("id: %d\n",id_num);
			 if(id_num == 1)
		    {
				 system("madplay  mp3/openWindow.mp3"); 
			}
			 if(id_num == 2)
		    {
				 system("madplay  mp3/openDoor.mp3"); 
			}
		    if(id_num == 3)
		    {
			    system("madplay  mp3/openLight.mp3"); 
			}
			 if(id_num == 4)
		    {
				 system("madplay  mp3/closeWindow.mp3"); 
			}
			 if(id_num == 5)
		    {
				 system("madplay  mp3/closeDoor.mp3"); 
			}
			   if(id_num == 6)
		    {
			    system("madplay  mp3/clodeLight.mp3"); 
			}
			 if(id_num == 8&&music==true)
		    {
			 	system("madplay  mp3/pauseMusic.mp3"); 
				music=false;
			}
			if(id_num == 7&&music==false)
		    {
			 	system("madplay  mp3/music.mp3"); 
				music=true;
				system("madplay  mp3/nevada.mp3"); 
			}
			said = 0; 
		}
	}

exit:
	
	close(sockfd);
	return 0;
}

