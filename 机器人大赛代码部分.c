#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Motor.h"
#include "sys.h"
#include "BasePer.h"
#include "Encoder.h"
#include "mpuexti.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "MOVE.h"
#include "SENSOR.h"

uint8_t key_num=0;
uint8_t mode=0;
void Sys_Init(void)
{
	BasePer_Init();
	OLED_Init(); 
	Serial_Init();
	Encoer_Init();
	MPU_Init();
	while(mpu_dmp_init())
	{
		Delay_ms(20);
	};

	
	Motor_Init();
	MPU_exti_init();
	SENSOR_Init();
}


void My_delay(uint32_t ms)
{
	int i;
	while(ms--)
	{
		for(i=0;i<1000;i++)
		{
			
		}
	}
}



void menu(void)
{
	//进程0 即是出发到第一个拐弯处
	while(lucheng_falg==0)  //考虑能不能用While //验证后为可以
	{
		Control_Straight_Move(speed,Qian_Jin);
		if(ZuoSensor==0)
		{
			Control_Straight_Move(speed,Hou_Tui);
			My_delay(600);
			lucheng_falg=1;
			Control_Straight_Move(speed,Zuo_PinYi);
			Bell(1);
			My_delay(50);
			Bell(0);
		}
	}
	//进程1 即是遇到第一个挡板
	while(lucheng_falg==1)
	{
		if(ZuoShangSensor==0)
		{
			Control_Straight_Move(speed,You_PinYi);
			My_delay(200);
			Bell(1);
			My_delay(100);
			Bell(0);
			lucheng_falg=2;
			Control_Straight_Move(speed,Qian_Jin);
			while(ZuoShangSensor==0);
			Control_Straight_Move(speed,Hou_Tui);
			My_delay(100);
		}
			
	}
	//进程2 即是遇到第二个挡板
	while(lucheng_falg==2)
	{
		Control_Straight_Move(speed,Zuo_PinYi);
		if(ZuoShangSensor==0)
		{
			Control_Straight_Move(speed,You_PinYi);
			My_delay(100);
			Bell(1);
			My_delay(100);
			Bell(0);
			lucheng_falg=3;
			speed=3400;
			Control_Straight_Move(speed,Hou_Tui);
			while(ZuoShangSensor==0);
			Bell(1);
			My_delay(420);
			Bell(0);
			Control_Straight_Move(speed,Qian_Jin);
			My_delay(150);
			targe_yaw=0.5;
			speed=2400;
			Control_Straight_Move(speed,Zuo_PinYi);
		}
	}
	//进程3 即是到第二个拐弯处
	while(lucheng_falg==3)
	{
		if(ZuoSensor==1)
		{
			Control_Straight_Move(speed,You_PinYi);
			lucheng_falg=4;
			while(ZuoSensor==0);
			speed=3400;
			Control_Straight_Move(speed,Hou_Tui);
			Bell(1);
			My_delay(100);
			Bell(0);
		}
	}
	//进程四 即是进行柱形障碍物避障
	//考虑4种情况 1：障碍物是2，3，5 
	//考虑4种情况 2：障碍物是2，3，6 
	//考虑4种情况 3：障碍物是2，4，5 
	//考虑4种情况 4：障碍物是2，4，6 
	//进程四 即是进行柱形障碍物避障
	while(lucheng_falg==4)
	{
		if(HouShangSensor==0)  //如果前方有障碍物，进行避开操作
		{
			speed=5000;
			Control_Straight_Move(speed,Qian_Jin);
			My_delay(500);  //原100
			speed=1800; //改变速度
			Control_Straight_Move(speed,You_PinYi);
			targe_yaw=0.5; //改变目标角度
			Bell(1);
			My_delay(100);
			Bell(0);
			while(YouSensor==0);
			Control_Straight_Move(speed,Zuo_PinYi);
			while(YouSensor==1);
			speed=3400;
			targe_yaw=-1.0; //使其偏移
			Control_Straight_Move(speed,Hou_Tui);
			lucheng_falg=5;
			break;
		}
		if(HouSensor==1)  //前方没有障碍物
		{
			lucheng_falg=6;
			break;
		}
		if(ZuoShangWeiZhiSensor==1)
		{
			targe_yaw=-1.0;
			Control_Straight_Move(speed,You_PinYi);
			while(ZuoShangWeiZhiSensor==1);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);
		}
		else if(HouSensor==1)  //前方没有障碍物
		{
			lucheng_falg=6;
			break;
		}
		else if(ZuoSensor==0)
		{
			targe_yaw=1.0;
			Control_Straight_Move(speed,Zuo_PinYi);
			while(ZuoSensor==0);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);
		}
		else
		{
			targe_yaw=0.5;
			Control_Straight_Move(speed,Hou_Tui);
		}
	}
		//进程五  为了有障碍的设计
	while(lucheng_falg==5)
	{
		if(ZhuanwanSensor==0) //右巡边时，判断是否要转弯
		{
			//My_delay(100);
			//Control_Straight_Move(speed,You_PinYi);
			//My_delay(800);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);
			lucheng_falg=7;
			break;
		}
		if(YouShangWeiZhiSensor==1)
		{
			targe_yaw=-1.0;
			Control_Straight_Move(speed,Zuo_PinYi);
			while(YouShangWeiZhiSensor==1);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);		
		}
		else if(YouSensor==0)
		{
			targe_yaw=1.0;
			Control_Straight_Move(speed,You_PinYi);
			while(YouShangWeiZhiSensor==1);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);		
		}
		else
		{
			targe_yaw=0.5;
			Control_Straight_Move(speed,Hou_Tui);
		}
	}
	//
	//进程六  //为了没有障碍设计
	//stop
	while(lucheng_falg==6)
	{
		if(HouSensor==1)
		{
			speed=2500;
			Control_Straight_Move(speed,Qian_Jin);
			while(HouSensor==1);
//			Control_Straight_Move(speed,Hou_Tui);
//			My_delay(100); 
//			Control_Straight_Move(speed,Qian_Jin);
			targe_yaw=-1.5;
			Control_Straight_Move(speed,You_PinYi);
			Bell(1); 
			My_delay(10000);  
			Bell(0);
			Control_Straight_Move(speed,Qian_Jin);
			My_delay(200); 
			//targe_yaw=-5.0;  
			//speed=4000;  
			Control_Straight_Move(speed,You_PinYi);
			lucheng_falg=8;
			break;
	 }		
//		if(HouSensor==1||YouShangWeiZhiSensor==1)
//		{ speed=2500;
//			Control_Straight_Move(speed,Qian_Jin);
//			while(HouSensor==1);
//			targe_yaw=-2.5;
//			Control_Straight_Move(speed,You_PinYi);
//			Bell(1); 
//			My_delay(3000);  //原800
//			Bell(0);
//			Control_Straight_Move(speed,Qian_Jin);
//			My_delay(200);  //原100
//			targe_yaw=7.0;  //10.0
//			speed=4200;  //原4600
//			Control_Straight_Move(speed,You_PinYi);
//			lucheng_falg=8;
//			break;
//	}

	}//stop
	//为了有障碍,冲刺用第四段用
	while(lucheng_falg==7)
	{
//		if(HouSensor==1)
//		{
//			Control_Straight_Move(speed,Qian_Jin);
//			while(HouSensor==1);
//			Control_Straight_Move(speed,You_PinYi);
//			targe_yaw=-4.0;
//			speed=3600;
//			Bell(1);
//			My_delay(2000);  
//			Bell(0);
//			Control_Straight_Move(speed,Qian_Jin);
//			My_delay(100);  
//			targe_yaw=6.0;  
//			speed=4200; 
//			Control_Straight_Move(speed,You_PinYi);
//			lucheng_falg=8;
//			break;
//		}
		if(HouSensor==1)
		{
			speed=2500;
			Control_Straight_Move(speed,Qian_Jin);
			while(HouSensor==1);
			Control_Straight_Move(speed,You_PinYi);
			targe_yaw=-1.5;
			Bell(1);
			My_delay(7000);  
			Bell(0);
			Control_Straight_Move(speed,Qian_Jin);
			My_delay(200);  
//			targe_yaw=-5.0;  
//			speed=4200; 
			Control_Straight_Move(speed,You_PinYi);
		  lucheng_falg=8;
			break;
		}
	}
	//进程七
	while(lucheng_falg==8)
	{
//		if(ZhuanwanSensor==1)
//		{
//			speed=2000;
//			Control_Straight_Move(speed,Zuo_PinYi);
//			Bell(1);
//			My_delay(1000);
//			Bell(0);
//			speed=6000;
//			Control_Straight_Move(speed,Qian_Jin);
//			lucheng_falg=9;
//      targe_yaw=-15.0;
//			
//		}
		if(ZhuanwanSensor==1)
		{
			speed=2000;
			Control_Straight_Move(speed,Zuo_PinYi);
			Bell(1);
			My_delay(400);
			Bell(0);
			speed=5800;
			targe_yaw=0.5;
			Control_Straight_Move(speed,Qian_Jin);
			My_delay(5000);
			targe_yaw=-25.0;
			lucheng_falg=9;
			
		}
//		if(YouSensor==1)
//		{
//			targe_yaw=0;
//			speed=1800;
//			Control_Straight_Move(speed,Zuo_PinYi);
//	    while(YouSensor==0);
//			targe_yaw=-5.4;
//			speed=5600;
//			Control_Straight_Move(speed,Qian_Jin);
//			lucheng_falg=9;
//		}
	}

}
uint8_t flagg=1;
void xunbian()
{
	if(flagg==0)
	{
		if(ZuoShangWeiZhiSensor==1)
		{
			targe_yaw=-1.0;
			Control_Straight_Move(speed,You_PinYi);
			while(ZuoShangWeiZhiSensor==1);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);
		}
		else if(ZuoSensor==0)
		{
			targe_yaw=1.0;
			Control_Straight_Move(speed,Zuo_PinYi);
			while(ZuoSensor==0);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);
		}
		else
		{
			targe_yaw=0.5;
			Control_Straight_Move(speed,Hou_Tui);
		}
	}

		if(flagg==1)
	{
		if(YouShangWeiZhiSensor==1)
		{
			targe_yaw=-1.0;
			Control_Straight_Move(speed,Zuo_PinYi);
			while(YouShangWeiZhiSensor==1);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);		
		}
		else if(YouSensor==0)
		{
			targe_yaw=1.0;
			Control_Straight_Move(speed,You_PinYi);
			while(YouShangWeiZhiSensor==1);
			targe_yaw=0.0;
			Control_Straight_Move(speed,Hou_Tui);		
		}
		else
		{
			targe_yaw=0.5;
			Control_Straight_Move(speed,Hou_Tui);
		}
	}
}

//右为负  左为正
void test_yaw(void)
{
	OLED_ShowSignedNum(1,1,yaw,3);
}

void Sensor_test(void)
{
//		OLED_ShowSignedNum(1,1,HouSensor,1);
//		OLED_ShowSignedNum(1,3,YouSensor,1);
//		OLED_ShowSignedNum(1,5,HouSensor,1);
		OLED_ShowSignedNum(1,3,ZuoSensor,1);
}
uint8_t ffll=0;
void testDelay()
{
 if(ffll==0)
 {
	 Bell(1);
	 My_delay(8000); 
	 Bell(0);
	 ffll=1;
 }
}

void Key_test(void)
{
	key_num=Key_Scan();
	if(key_num==1)
	{
		mode++;
		if(mode>4)
		{
			mode=1;
		}
	}
	if(key_num==2)
	{
		mode=0;
	}
	switch(mode)
	{
		case 0:Control_Straight_Move(0,0);break;
		case 1:Control_Straight_Move(speed,Zuo_PinYi);//qianjin
		       break;
		case 2:Control_Straight_Move(speed,You_PinYi);//houtui
		       break;
		case 3:Control_Straight_Move(speed,Qian_Jin);//youpingyi
		       break;
		case 4:Control_Straight_Move(speed,Hou_Tui);//zuopingyi
		       break;
	}
}


void kong()
{
    speed=2000;
		Control_Straight_Move(speed,Hou_Tui);
    if(HouShangSensor==0)
		{
    Control_Straight_Move(speed,Qian_Jin);//zuopingyi
		My_delay(4000); 
    Control_Straight_Move(speed,Zuo_PinYi);//zuopingyi
		My_delay(4000); 
		}
}

int main()
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Sys_Init();
	
	while(1)
	{
		//Key_test();
		//Sensor_test();
		//test_yaw();
		//menu();
		//testDelay();
		//OLED_ShowSignedNum(1,1,HouSensor,1);
		//xunbian();
		kong();

	}
}
