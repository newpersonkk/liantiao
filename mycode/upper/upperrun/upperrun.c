
#include "upperrun.h"
#include "param.h"
#include "math.h"
#include "upperservo.h"
#include "cmsis_os.h"
#include "tim.h"
#include "mi_motor.h"
#include "string.h"
#include <ctype.h>

#define pi 3.1415926535

uint16_t flag1 = 0;
uint16_t flag2 = 0;

uint16_t stateflag = 0 ;
uint16_t runflag = 0 ;//0
uint16_t step00 = 0 ; 
uint16_t step01 = 0 ; 
uint16_t step02 = 0 ; 

uint16_t step10 = 0 ; 
uint16_t step11 = 0 ; 
uint16_t step12 = 0 ; 

uint16_t step20 = 0 ; 
uint16_t step21 = 0 ; 
uint16_t step22 = 0 ;

int16_t xiangzi[6]; 
int16_t zhiduo[6];  
int16_t mapping[6]={0,0,0,0,0,0};
uint16_t jiaquzhuangtai = 0;//0

float MIkp = 5.0;
float MIkd = 1.5;

float S0 = pi * 0.5;//黑白向货箱 彩色向2345
float S90 = pi * 1.0  ;//彩色向1纸垛 黑白向6纸垛
float S180 = pi * 1.5 ;//彩色向货箱 黑白向2345
float S270 = pi * 2.0 ;//黑白向1纸垛 彩色向6纸垛

float errorth = 3.1415926/180.0 * 0.0;

void generate_mapping_array(int16_t arr1[], int16_t arr2[], int16_t output[]) {
    // 存储每个数字在两个数组中的索引
    int index_list1[6][6] = {{0}}; 
    int index_list2[6][6] = {{0}};
    
    // 存储每个数字在两个数组中出现的次数
    int count1[6] = {0};
    int count2[6] = {0};
    
    // 初始化输出数组为0（表示未匹配）
    for (int i = 0; i < 6; i++) {
        output[i] = 0;
    }
    
    // 遍历第一个数组（只处理1-6的数字）
    for (int i = 0; i < 6; i++) {
        int num = arr1[i];
        if (num >= 1 && num <= 6) {
            int idx = num - 1;
            index_list1[idx][count1[idx]++] = i;
        }
    }
    
    // 遍历第二个数组（忽略0值，只处理1-6的数字）
    for (int i = 0; i < 6; i++) {
        int num = arr2[i];
        if (num >= 1 && num <= 6) {
            int idx = num - 1;
            index_list2[idx][count2[idx]++] = i;
        }
    }
    
    // 为每个数字生成配对并填充输出数组
    for (int num = 0; num < 6; num++) {
        // 取两个数组中该数字出现次数的最小值
        int min_count = count1[num] < count2[num] ? count1[num] : count2[num];
        
        // 为每个配对更新输出数组
        for (int j = 0; j < min_count; j++) {
            int arr1_index = index_list1[num][j];
            int arr2_index = index_list2[num][j];
            output[arr1_index] = arr2_index + 1; // 索引值加1
        }
    }
}

void matchingnum(int16_t arr1[], const char arr2[])
{
  for (uint16_t i = 0; i < 6; ++i) {
      if (isdigit(arr2[i])) {
          arr1[i] = arr2[i] - '0';  // 转换数字字符为整数
      } else {
          arr1[i] = 0;             // 非数字字符赋值为0
      }
  }
}

void uppergoingtask(void const * argument)
{
  /* USER CODE BEGIN uppergoingtask */
  /* Infinite loop */





  /*
                     (2985.0,2985.0,1390.0) (2985.0,3000.0,955.0) (2985.0,3000.0,490.0) (2985,3000,55.0)
    (2807,2807,1151)                                                                                     (2807,2807,275.0)



    x:250  y:210 702 1200
     货架
     y----->
     1 2 3
  x
  |
  |
  |
  激光---->
  |
  |
  6         1
    5 4 3 2 
     纸垛
  z轴是0为起始位置

  右 左
  3000 2970 
  2822 2795
  270 220  

  */
  //963.3=10.6cm 是夹取上层的位置 1040=8cm  74=夹取下层箱子的位置  150=进去的位置
  //220等于刚好放纸垛的位置
  //500=26.8 550=24.9 600=23  700=19.4 800=16  1100=12.3 930=11.5 

  //
  float zhuazibhfang = 75 ;
  float zhuazibhopen = 78 ;
  float zhuazibhclose = 96 ;

  float zhuazicaifang = 80 ;
  float zhuazicaiopen = 82 ;
  float zhuazicaiclose = 101 ; 
  
  //230/240举起来  15取物   867取物  66放置1    397放置2

  float z_upeer_up = 1025;
  float z_upeer_down = 870;
  float z_down_up = 240;
  float z_down_down = 10;
  float z_zhiduo_fang = 188.0;
  float z_zhiduo_fang2 = 450;
  float z_middle = 850;

  float x_qu = 264.0;
  float x_qu_ps = 600;
  float x_fang_4 = 3011.0;//3051  3040
  float x_fix = 40.0;
  float x_errorth = 8.0;
  float x_1errorth = 0;
  float x_2errorth = 0;
  float x_fang_4_2 = 3011.0;//
  float x_fang_2 = 2827.0;
  float x_middle = 1980.0;
  float x_zhan = 2660.0;  

  float y_qu_1 = 204.0;
  float y_qu_2 = 710.0;
  float y_qu_3 = 1204.0;
  float y_middle = 750.0;
  float y_errorth = -3.0;  

  float y_fang_1 = 1149.0;
  float y_fang_2 = 1388.0;
  float y_fang_3 = 945.0;
  float y_fang_4 = 480.5;
  float y_fang_5 = 60.5;
  float y_fang_6  = 270.0;

  // xiangzi[0] = 2;
  // xiangzi[1] = 6;//
  // xiangzi[2] = 1;
  // xiangzi[3] = 4;
  // xiangzi[4] = 3;//
  // xiangzi[5] = 5;

  // zhiduo[0] = 5;
  // zhiduo[1] = 4;
  // zhiduo[2] = 1;
  // zhiduo[3] = 3;
  // zhiduo[4] = 6;
  // zhiduo[5] = 0;
  osDelay(600);
  osDelay(1350);
  matchingnum(xiangzi, tx_buffer4);
  matchingnum(zhiduo, tx_buffer5);
  generate_mapping_array(xiangzi,zhiduo,mapping);
  for(;;)
  {
    if(jiaquzhuangtai == 0)//夹取1和4的箱子
    {
      if(runflag == 0)//让夹爪先升起来
      {
        mygantry.gantrypos.z = z_down_up;
        osDelay(100);
        mygantry.gantrypos.degree = S0 ;
        osDelay(500);
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          osDelay(200);
          runflag = 1;
        }
      }
      if(runflag == 1)//让夹爪张开然后摆正位置
      {
          __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
          __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
          osDelay(10);
          runflag = 2;          
      }
      if(runflag == 2)
      {
        mygantry.gantrypos.z = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          runflag = 3;
          osDelay(10);
        }
      }
      if(runflag == 3)
      {
        mygantry.gantrypos.y = y_qu_1;
        osDelay(500);
        mygantry.gantrypos.x = x_qu;
        float diff1 = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff2 = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff1 < 15 && diff2 < 4 && diff3 < 10)
        {
          runflag = 4;
          osDelay(10);
        }
      }
      if(runflag == 4)//取了第一个箱子
      {
        mygantry.gantrypos.z = z_down_down;//取下面
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);//
          MIkp = 1.8;
          MIkd = 1.0;
          pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);////一个箱子的Y
          osDelay(30);
          runflag = 5;
        }
      }
      if(runflag == 5)
      {
        mygantry.gantrypos.z = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          runflag = 6;
          osDelay(100);
        }
      }
      if(runflag == 6)
      {
        mygantry.gantrypos.x = x_qu_ps;
        float diff = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff < 60 && diff3 < 60)
        {
          runflag = 7;
          osDelay(100);
        }
      }
      if(runflag == 7)
      {
        mygantry.gantrypos.y = y_middle;
        osDelay(200);
        mygantry.gantrypos.z = z_upeer_down;
        mygantry.gantrypos.degree = S180;
        osDelay(500);
        float diff1 = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        float diff2 = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff1 < 50 && diff2 < 30 )
        {
          osDelay(50);
          runflag = 8;
        }
      }
      if(runflag == 8)
      {
        osDelay(10);
        runflag = 9;
      }
      if(runflag == 9)
      {
        mygantry.gantrypos.y = y_qu_1;
        mygantry.gantrypos.z = z_upeer_up;
        float diff = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        if(diff < 100)
        {
          runflag = 10;
          osDelay(500);
        }
      }
      if(runflag == 10)
      {
        mygantry.gantrypos.x = x_qu + 10.0;
        float diff = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff < 15 && diff3 < 10)
        {
          runflag = 11;
          osDelay(10);
        }
      }
      if(runflag == 11)
      {
        mygantry.gantrypos.z = z_upeer_down;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 6)
        {
          runflag = 12;
          __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
          MIkp = 2.2;
          MIkd = 1.5;
          pid_reset(&(mygantry.Motor_Y->posPID), 15.000002, 0.0, 0);
          osDelay(500);
        }
      }
      if(runflag == 12)
      {
        mygantry.gantrypos.z = z_upeer_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          runflag = 13;
          osDelay(100);
        }
      }
      if(runflag == 13)
      {
        mygantry.gantrypos.x  = x_middle;
        osDelay(200);
        mygantry.gantrypos.y  = y_middle;
        osDelay(200);
        mygantry.gantrypos.z = z_middle;
        osDelay(50);
        runflag = 15;
        jiaquzhuangtai = 1 ;
      }
    }
    //开始放置
    if(jiaquzhuangtai == 1 )//012是黑白夹爪 345是彩色夹爪
    {
      if(mapping[0] != 0 && mapping[3] != 0)//因为是先取[3]所以先放[3]htim3  后取[0]所以后放[0]htim4
      {
        if(step00 == 0)
        {
          if(mapping[3] < 5 && mapping[3] > 2)  mygantry.gantrypos.degree = S180;
          else if(mapping[3] == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(mapping[3] == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(mapping[3] == 1) mygantry.gantrypos.degree = S270;
          else if(mapping[3] == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step00 = 100;
        }
        if(step00 == 100)
        {
          if(mapping[3] < 5 && mapping[3] > 2)//不需要转动直接进行放置再回到中点
          {
            if(mapping[3] == 3) mygantry.gantrypos.y = y_fang_3;
            if(mapping[3] == 4) 
            {
              mygantry.gantrypos.y = y_fang_4;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(mapping[3] == 2)
          {
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2; 
          }else if(mapping[3] == 5)
          {
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02;
          }
          else if(mapping[3] == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(mapping[3] == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step00 = 1;
          }
        }
        if(step00 == 1)
        {
          mygantry.gantrypos.z = z_zhiduo_fang;//3号箱子面向了纸垛 是黑白的夹爪 
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen); 
            MIkp = 1.8;
            MIkd = 1.0;
            osDelay(100);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            osDelay(100);
            step00 = 2;
          }
        }
        if(step00 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 100 && diff2 < 80 && diff3 < 80)
          {
            step00 = 200;
            osDelay(50);
          }
        }
        if(step00 == 200)//一个箱子
        {
          if(mapping[0] < 5 && mapping[0] > 2)  mygantry.gantrypos.degree = S0;
          else if(mapping[0] == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(mapping[0] == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(mapping[0] == 1) mygantry.gantrypos.degree = S90;
          else if(mapping[0] == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step00 = 3;
        }
        if(step00 == 3)
        {
          if(mapping[0] < 5 && mapping[0] > 2)//不需要转动直接进行放置再回到中点
          {
            if(mapping[0] == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(mapping[0] == 4)
            { 
              mygantry.gantrypos.y = y_fang_4;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(mapping[0] == 2)
          {
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }else if(mapping[0] == 5)
          {
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02;
          }
          else if(mapping[0] == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(mapping[0] == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 10 && diff2 < 4 && diff3 < 10)
          {
            step00 = 4;//[3]运动到位
          }
        }
        if(step00 == 4)
        {
          Lcompensation = 1.0;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
            MIkp = 1.0;
            MIkd = 0.5;
            osDelay(200);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            step00 = 5;
          }
        }
        if(step00 == 5)
        {
          mygantry.gantrypos.x = x_middle;
          osDelay(200);
          mygantry.gantrypos.y = y_middle;
          osDelay(500);
          mygantry.gantrypos.degree = S0;
          jiaquzhuangtai = 2;
        }
      }
      else if(mapping[0]==0 && mapping[3]!=0) //先放[3]又因为[3]不是0 可以先放
      {
        uint16_t num = mapping[0] + mapping[3];
        if(step01 == 0)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S180;
          else if(num == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S270;
          else if(num == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step01 = 100;
        }
        if(step01 == 100)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3;
            if(num == 4) 
            {
              mygantry.gantrypos.y = y_fang_4 ;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02;
          }
          else if(num == 1)
          { 
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 10 && diff2 < 4 && diff3 < 10)
          {
            step01 = 1;//[3]运动到位
          }
        }
        if(step01 == 1)
        {
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            MIkp = 1.8;
            MIkd = 1.0;
            osDelay(200);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            step01 = 2;
          }
        }
        if(step01 == 2)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step01 = 3;
            osDelay(10);
          }
        }
        if(step01 == 3) //现在是S180S270
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S0;
          else if(num == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S90;
          else if(num == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step01 = 400 ;
        }
        if(step01 == 400)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(num == 4)
            {
               mygantry.gantrypos.y = y_fang_4 + y_errorth;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          { 
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10) 
          {
            osDelay(50);
            step01 = 4;//[3]运动到位
          }
        }
        if(step01 == 4)
        {
          Lcompensation = 1.0 ; 
          mygantry.gantrypos.z = z_zhiduo_fang2;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
            osDelay(200);
            MIkp = 1.0;
            MIkd = 0.5;
            mygantry.gantrypos.z = z_middle+100;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            step01 = 5;
          }
        }
        if(step01 == 5)
        {
          mygantry.gantrypos.y = y_middle;
          osDelay(200);
          mygantry.gantrypos.x = x_middle;
          osDelay(500);
          step01 = 6;//[3]运动到位
          osDelay(20);
          mygantry.gantrypos.degree = S0;
          jiaquzhuangtai = 2;
        } 
      }
      else if(mapping[0] != 0 && mapping[3]==0)//mapping[0] = 1; mapping[3]=0
      {
        //转180度
        uint16_t num = mapping[0] + mapping[3];// num = 1
        if(step02 == 0)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S0;
          else if(num == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S0 - 1.1* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S90;
          else if(num == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step02 = 100 ;
        }
        if(step02 == 100)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3;
            if(num == 4)
            {
               mygantry.gantrypos.y = y_fang_4;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2 + x_2errorth;
          }else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          { 
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step02 = 1;//[3]运动到位
          }
        }
        if(step02 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaifang);//
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);//
            MIkp = 1.8;
            MIkd = 1.0;
            osDelay(200);
            step02 = 2;
          }
        }
        if(step02 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step02 = 300;
            osDelay(50);
          }
        }
        if(step02 == 300)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S180;
          else if(num == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S270;
          else if(num == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step02 = 3;
        }
        if(step02 == 3)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(num == 4)
            { 
              mygantry.gantrypos.y = y_fang_4 + y_errorth;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step02 = 4;//[3]运动到位
          }
        }
        if(step02 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang2;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhfang);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(300);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            MIkp = 1.0;
            MIkd = 0.5;
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            osDelay(200);
            step02 = 5;
          }
        }
        if(step02 == 5)
        {
          mygantry.gantrypos.x = x_middle;
          osDelay(200);
          mygantry.gantrypos.y = y_middle;
          step02 = 6;//[3]运动到位
          osDelay(500);
          mygantry.gantrypos.degree = S0;
        } 
        if(step02 == 6)
        {
          osDelay(50);
          jiaquzhuangtai = 2;
        } 
      }
    }
    if(jiaquzhuangtai == 2)//夹取1和4的箱子
    {
      if(runflag == 15)//让夹爪先升起来
      {
        mygantry.gantrypos.z = z_down_up;
        mygantry.gantrypos.degree = S0;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 10)
        {
          osDelay(50);
          runflag = 1;
        }
      }
      if(runflag == 1)//让夹爪张开然后摆正位置
      {
        __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
        __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
        mygantry.gantrypos.y  = y_middle;
        mygantry.gantrypos.x  = x_middle;
        osDelay(100);
        runflag = 2;          
      }
      if(runflag == 2)
      {
        mygantry.gantrypos.z = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 10)
        {
          runflag = 3;
          osDelay(50);
        }
      }
      if(runflag == 3)
      {
        mygantry.gantrypos.y = y_qu_2;
        osDelay(500);
        mygantry.gantrypos.x = x_qu;
        float diff1 = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff2 = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff1 < 15 && diff2 < 4 && diff3 < 10)
        {
          runflag = 4;
          osDelay(10);
        }
      }
      if(runflag == 4)
      {
        mygantry.gantrypos.z = z_down_down;//取下面
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);//
          MIkp = 1.8;
          MIkd = 1.0;
          pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
          osDelay(10);
          runflag = 5;
        }
      }
      if(runflag == 5)
      {
        mygantry.gantrypos.z = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          runflag = 6;
          osDelay(30);
        }
      }
      if(runflag == 6)
      {
        mygantry.gantrypos.x = x_qu_ps;
        float diff = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff < 100 && diff3 < 100)
        {
          runflag = 7;
          osDelay(10);
        }
      }
      if(runflag == 7)
      {
        mygantry.gantrypos.y = y_qu_2;

        mygantry.gantrypos.z = z_upeer_down;
        osDelay(500);
        mygantry.gantrypos.degree = S180 ;
        osDelay(500);
        float diff1 = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        float diff2 = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff1 < 20 && diff2 < 6)
        {
          osDelay(10);
          runflag = 9;
        }
      }
      if(runflag == 9)
      {
        mygantry.gantrypos.y = y_qu_2;
        mygantry.gantrypos.z = z_upeer_up;
        mygantry.gantrypos.degree = S180;
        osDelay(500);
        float diff = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        if(diff < 40)
        {
          runflag = 10;
          osDelay(200);
        } 
      }
      if(runflag == 10)
      {
        mygantry.gantrypos.x = x_qu + 10.0;
        float diff = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff < 15 && diff3 < 15)
        {
          runflag = 11;
          osDelay(10);
        }
      }
      if(runflag == 11)
      {
        mygantry.gantrypos.z = z_upeer_down - 20;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 10)
        {
          __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
          MIkp = 2.2;
          MIkd = 1.5;
          pid_reset(&(mygantry.Motor_Y->posPID), 15.000002, 0.0, 0);
          osDelay(100);
          runflag = 12;
        }
      }
      if(runflag == 12)
      {
        mygantry.gantrypos.z = z_upeer_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 6)
        {
          runflag = 13;
          osDelay(50);
        }
      }
      if(runflag == 13)
      {
        mygantry.gantrypos.x  = x_middle;
        osDelay(200);
        mygantry.gantrypos.y  = y_middle;
        osDelay(200);
        mygantry.gantrypos.z  = z_middle;
        runflag = 15;
        jiaquzhuangtai = 3 ;
        osDelay(50);
      }
    }
    if(jiaquzhuangtai == 3 )
    {
      if(mapping[1]!=0 && mapping[4]!=0)
      {
        if(step10 == 0){
        if(mapping[4] < 5 && mapping[4] > 2)  mygantry.gantrypos.degree = S180;
          else if(mapping[4] == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(mapping[4] == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(mapping[4] == 1) mygantry.gantrypos.degree = S270;
          else if(mapping[4] == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step10 = 100;
        }
        if(step10 == 100)
        {
          if(mapping[4] < 5 && mapping[4] > 2)//不需要转动直接进行放置再回到中点
          {
            if(mapping[4] == 3) mygantry.gantrypos.y = y_fang_3;
            if(mapping[4] == 4)
            {
              mygantry.gantrypos.y = y_fang_4;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(mapping[4] == 2)
          {
            mygantry.gantrypos.y = y_fang_2;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }else if(mapping[4] == 5)
          {
            mygantry.gantrypos.y = y_fang_5;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(mapping[4] == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(mapping[4] == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step10 = 1;
          }
        }
        if(step10 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            osDelay(100);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);//
            MIkp = 1.8;
            MIkd = 1.0;
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            osDelay(300);
            step10 = 2;
          }
        }
        if(step10 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step10 = 200;
            osDelay(50);
          }
        }
        if(step10 == 200)
        {
          if(mapping[1] < 5 && mapping[1] > 2)  mygantry.gantrypos.degree = S0;
          else if(mapping[1] == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(mapping[1] == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(mapping[1] == 1) mygantry.gantrypos.degree = S90;
          else if(mapping[1] == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step10 = 3;
        }
        if(step10 == 3)
        {
          if(mapping[1] < 5 && mapping[1] > 2)//不需要转动直接进行放置再回到中点
          {
            if(mapping[1] == 3) mygantry.gantrypos.y = y_fang_3;
            if(mapping[1] == 4)
            { 
              mygantry.gantrypos.y = y_fang_4;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(mapping[1] == 2)
          {
            mygantry.gantrypos.y = y_fang_2;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2  + x_errorth;
          }else if(mapping[1] == 5)
          {
            mygantry.gantrypos.y = y_fang_5;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2  + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(mapping[1] == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(mapping[1] == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step10 = 4;//[3]运动到位
          }
        }
        if(step10 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            MIkp = 1.0;
            MIkd = 0.5;
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            osDelay(200);
            step10 = 5;
          }
        }
        if(step10 == 5)
        {
          mygantry.gantrypos.x = x_middle;
          osDelay(200);
          mygantry.gantrypos.y = y_middle;
          osDelay(200);
          mygantry.gantrypos.z = z_middle;
          osDelay(300);
          mygantry.gantrypos.degree = S0;
          osDelay(100);
          jiaquzhuangtai = 4;
        }
        //转到0度   jiaquzhuangtai = 4;
      }
      else if(mapping[1]==0 && mapping[4]!=0)
      {
        uint16_t num = mapping[1] + mapping[4];
        if(step11 == 0){
        if(num < 5 && num > 2)  mygantry.gantrypos.degree = S180;
          else if(num == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S270;
          else if(num == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step11 = 100;
        }
        if(step11 == 100)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3;
            if(num == 4)
            {
               mygantry.gantrypos.y = y_fang_4;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step11 = 1;
          }
        }
        if(step11 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);//
            MIkp = 1.8;
            MIkd = 1.0;
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            osDelay(200);
            step11 = 2;
          }
        }
        if(step11 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step11 = 3;
            osDelay(50);
          }
        }
        if(step11 == 3) //现在是S180
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S0;
          else if(num == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S90;
          else if(num == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step11 = 400 ;
        }
        if(step11 == 400)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(num == 4)
            { 
              mygantry.gantrypos.y = y_fang_4 + y_errorth;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + 10.0;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step11 = 4;
            osDelay(10);
          }
        }
        if(step11 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang2;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
            osDelay(200);
            MIkp = 1.0;
            MIkd = 0.5;
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            osDelay(100);
            step11 = 5;
          }
        }
        if(step11 == 5)
        {
          mygantry.gantrypos.y = y_middle;
          osDelay(200);
          mygantry.gantrypos.x = x_middle;
          osDelay(300);
          step11 = 6;//[3]运动到位
          mygantry.gantrypos.degree = S0;
          osDelay(20);
          jiaquzhuangtai = 4;
          
        }    
      }
      else if(mapping[1]!=0 && mapping[4]==0)
      {
        //转180度
        uint16_t num = mapping[1] + mapping[4];
        if(step12 == 0)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S0;
          else if(num == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S90;
          else if(num == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step12 = 100 ;
        }
        if(step12 == 100)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3;
            if(num == 4)
            {
              mygantry.gantrypos.y = y_fang_4;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step12 = 1;//[3]运动到位
          }
        }
        if(step12 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);//
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            MIkp = 1.8;
            MIkd = 1.0;
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);//
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            osDelay(200);
            step12 = 2;
          }
        }
        if(step12 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step12 = 300;
            osDelay(50);
          }
        }
        if(step12 == 300)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S180;
          else if(num == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S270;
          else if(num == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step12 = 3;//[3]运动到位
        }
        if(step12 == 3)
        {
         if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(num == 4)
            {
               mygantry.gantrypos.y = y_fang_4 + y_errorth;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2  + x_errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step12 = 4;//[3]运动到位
          }
        }
        if(step12 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang2;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(300);
            MIkp = 1.0;
            MIkd = 0.5;
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            osDelay(100);
            step12 = 5;
          }
        }
        if(step12 == 5)
        {
          mygantry.gantrypos.y = y_middle; 
          osDelay(200);
          mygantry.gantrypos.x = x_middle;
          osDelay(500);
          step12 = 6;//[3]运动到位
          mygantry.gantrypos.degree = S0;
          osDelay(200);
        } 
        if(step12 == 6)
        {
          osDelay(50);
          jiaquzhuangtai = 4;
        } 
        //jiaquzhuangtai = 4;
      }
    }
    if(jiaquzhuangtai == 4)//夹取1和4的箱子
    {
      if(runflag == 15)//让夹爪先升起来
      {
        mygantry.gantrypos.z = z_down_up;
        mygantry.gantrypos.degree = S0;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 10)
        {
          osDelay(10);
          runflag = 1;
        }
      }
      if(runflag == 1)//让夹爪张开然后摆正位置
      {
        __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
        __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
        mygantry.gantrypos.y  = y_middle;
        mygantry.gantrypos.x  = x_middle;
        osDelay(200);
        runflag = 2;          
        
      }
      if(runflag == 2)
      {
        mygantry.gantrypos.z = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 10)
        {
          runflag = 3;
          osDelay(10);
        }
      }
      if(runflag == 3)
      {
        mygantry.gantrypos.y = y_qu_3;
        osDelay(500);
        mygantry.gantrypos.x = x_qu;
        float diff1 = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff2 = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff1 < 15 && diff2 < 4 && diff3 < 10)
        {
          runflag = 4;
          osDelay(10);
        }
      }
      if(runflag == 4)
      {
        mygantry.gantrypos.z = z_down_down - 10.0;//取下面
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);//
          MIkp = 1.8;
          MIkd = 1.0;
          pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
          osDelay(100);
          runflag = 5;
        }
      }
      if(runflag == 5)
      {
        mygantry.gantrypos.z = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          runflag = 6;
          osDelay(50);
        }
      }
      if(runflag == 6)
      {
        mygantry.gantrypos.x = x_qu_ps;
        float diff = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff < 100 && diff3 < 100)
        {
          runflag = 7;
          osDelay(50);
        }
      }
      if(runflag == 7)
      {
        mygantry.gantrypos.y = y_middle;
        mygantry.gantrypos.z = z_upeer_up;
        osDelay(500);
        mygantry.gantrypos.degree = S180;
        float diff1 = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        float diff2 = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff1 < 50 && diff2 < 30)
        {
          osDelay(50);
          runflag = 9;
        }
      }
      if(runflag == 9)
      {
        mygantry.gantrypos.y = y_qu_3 + 2.0;
        float diff = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        if(diff < 20)
        {
          mygantry.gantrypos.degree = S180;
          runflag = 10;
          osDelay(50);
        } 
      }
      if(runflag == 10)
      {
        mygantry.gantrypos.x = x_qu + 10.0;
        float diff = fabs(mygantry.gantrypos.x - Lidar1.distance_aver);
        float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
        if(diff < 15 && diff3 < 15)
        {
          runflag = 11;
          osDelay(50);
        }
      }
      if(runflag == 11)
      {
        mygantry.gantrypos.z = z_upeer_down - 20;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          osDelay(50);
          __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
          MIkp = 2.2;
          MIkd = 1.5;
          pid_reset(&(mygantry.Motor_Y->posPID), 15.000002, 0.0, 0);
          osDelay(100);
          runflag = 12;
        }
      }
      if(runflag == 12)
      {
        mygantry.gantrypos.z = z_upeer_up;
        float diff = fabs(mygantry.gantrypos.z - hDJI[4].AxisData.AxisAngle_inDegree);
        if(diff < 4)
        {
          runflag = 13;
          osDelay(50);
        }
      }
      if(runflag == 13)
      {
        mygantry.gantrypos.x  = x_middle;
        osDelay(200);
        mygantry.gantrypos.y  = y_middle;
        osDelay(200);
        mygantry.gantrypos.z  = z_middle;
        runflag = 15;
        jiaquzhuangtai = 5 ;
        osDelay(200);
      }
    
    }
    if(jiaquzhuangtai == 5 )
    {
      if(mapping[2]!=0 && mapping[5]!=0)
      {
        if(step20 == 0)
        {
          if(mapping[5] < 5 && mapping[5] > 2)  mygantry.gantrypos.degree = S180;
          else if(mapping[5] == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(mapping[5] == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(mapping[5] == 1) mygantry.gantrypos.degree = S270;
          else if(mapping[5] == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step20 = 100;
        }
        if(step20 == 100)
        {
          if(mapping[5] < 5 && mapping[5] > 2)//不需要转动直接进行放置再回到中点
          {
            if(mapping[5] == 3) mygantry.gantrypos.y = y_fang_3;
            if(mapping[5] == 4)
            {
               mygantry.gantrypos.y = y_fang_4;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(mapping[5] == 2)
          {
            mygantry.gantrypos.y = y_fang_2;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }else if(mapping[5] == 5)
          {
            mygantry.gantrypos.y = y_fang_5;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(mapping[5] == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(mapping[5] == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step20 = 1;//[0]运动到位
          }
        }
        if(step20 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            MIkp = 1.8;
            MIkd = 1.0;
            osDelay(200);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            osDelay(200);
            step20 = 2;
          }
        }
        if(step20 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step20 = 200;
            osDelay(50);
          }
        }
        if(step20 == 200)
        {
          if(mapping[2] < 5 && mapping[2] > 2)  mygantry.gantrypos.degree = S0;
          else if(mapping[2] == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(mapping[2] == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(mapping[2] == 1) mygantry.gantrypos.degree = S90;
          else if(mapping[2] == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step20 = 3;
        }
        if(step20 == 3)
        {
          if(mapping[2] < 5 && mapping[2] > 2)//不需要转动直接进行放置再回到中点
          {
            if(mapping[2] == 3) mygantry.gantrypos.y = y_fang_3;
            if(mapping[2] == 4)
            {
               mygantry.gantrypos.y = y_fang_4;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(mapping[2] == 2)
          {
            mygantry.gantrypos.y = y_fang_2;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }else if(mapping[2] == 5)
          {
            mygantry.gantrypos.y = y_fang_5;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(mapping[2] == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(mapping[2] == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {

            step20 = 4;//[3]运动到位
          }
        }
        if(step20 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            MIkp = 1.0;
            MIkd = 0.5;
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            step20 = 5;
          }
        }
        if(step20 == 5)
        {
          mygantry.gantrypos.x = x_middle-1100;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            osDelay(200);
            step20 = 6;//[3]运动到位
          }
        }
        //转到0度   jiaquzhuangtai = 2;
      }
      else if(mapping[2]==0 && mapping[5]!=0)
      {
        uint16_t num = mapping[2] + mapping[5];
        if(step21 == 0)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S180;
          else if(num == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S270;
          else if(num == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step21 = 100;
        }
        if(step21 == 100)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3;
            if(num == 4)
            {
               mygantry.gantrypos.y = y_fang_4;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step21 = 1;//[3]运动到位
          }
        }
        if(step21 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            MIkp = 1.8;
            MIkd = 1.0;
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            step21 = 2;
          }
        }
        if(step21 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step21 = 3;
            osDelay(50);
          }
        }
        if(step21 == 3) //现在是S180
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S0;
          else if(num == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S90;
          else if(num == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step21 = 400 ;
        }
        if(step21 == 400)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(num == 4)
            {
               mygantry.gantrypos.y = y_fang_4 + y_errorth;
               Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step21 = 4;//[3]运动到位
          }
        }
        if(step21 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang2;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle+100;
            osDelay(300);
            MIkp = 1.0;
            MIkd = 0.5;
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            osDelay(200);
            step21 = 5;
          }
        }
        if(step21 == 5)
        {
          mygantry.gantrypos.x = x_middle-1100;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step21 = 6;//[3]运动到位
          }
        } 
        //jiaquzhuangtai = 6;       
      }
      else if(mapping[2]!=0 && mapping[5]==0)
      {
        //转180度
        uint16_t num = mapping[2] + mapping[5];
        if(step22 == 0)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S0;
          else if(num == 2) mygantry.gantrypos.degree = S0 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S0 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S90;
          else if(num == 6) mygantry.gantrypos.degree = S270;
          osDelay(2000);
          step22 = 100 ;
        }
        if(step22 == 100)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3;
            if(num == 4) 
            {
              mygantry.gantrypos.y = y_fang_4;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2;
            mygantry.gantrypos.x = x_fang_4_2  + x_2errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5;
            mygantry.gantrypos.x = x_fang_4_2;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6 ;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step22 = 1;//[3]运动到位
          }
        }
        if(step22 == 1)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiopen);//
            osDelay(200);
            mygantry.gantrypos.z = z_middle;
            osDelay(200);
            MIkp = 1.8;
            MIkd = 1.0;
            __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,zhuazicaiclose);//
            pid_reset(&(mygantry.Motor_Y->posPID), 15.000001, 0.0, 0);
            osDelay(200);
            step22 = 2;
          }
        }
        if(step22 == 2)
        {
          mygantry.gantrypos.x = x_zhan;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 120 && diff2 < 80 && diff3 < 80)
          {
            step22 = 300;
            osDelay(50);
          }
        }
        if(step22 == 300)
        {
          if(num < 5 && num > 2)  mygantry.gantrypos.degree = S180;
          else if(num == 2) mygantry.gantrypos.degree = S180 +1.1 *errorth;
          else if(num == 5) mygantry.gantrypos.degree = S180 - 1.2* errorth;
          else if(num == 1) mygantry.gantrypos.degree = S270;
          else if(num == 6) mygantry.gantrypos.degree = S90;
          osDelay(2000);
          step22 = 3;//[3]运动到位
        }
        if(step22 == 3)
        {
          if(num > 2 && num < 5 )//
          {
            if(num == 3) mygantry.gantrypos.y = y_fang_3 + y_errorth;
            if(num == 4) 
            {
              mygantry.gantrypos.y = y_fang_4 + y_errorth;
              Lcompensation = 1.01;
            }
            mygantry.gantrypos.x = x_fang_4 + x_errorth;
          }
          else if(num == 2)
          { 
            mygantry.gantrypos.y = y_fang_2 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
          }
          else if(num == 5)
          { 
            mygantry.gantrypos.y = y_fang_5 + y_errorth;
            mygantry.gantrypos.x = x_fang_4_2 + x_errorth;
            Lcompensation = 1.02 ;
          }
          else if(num == 1)
          {
            mygantry.gantrypos.y = y_fang_1;
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2 + x_1errorth;
          }
          else if(num == 6)
          {
            mygantry.gantrypos.y = y_fang_6;  
            osDelay(800);
            mygantry.gantrypos.x = x_fang_2;
          }
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step22 = 4;//[3]运动到位
          }
        }
        if(step22 == 4)
        {
          Lcompensation = 1.0 ;
          mygantry.gantrypos.z = z_zhiduo_fang2;
          float diff = fabs(mygantry.gantrypos.z-hDJI[4].AxisData.AxisAngle_inDegree);
          if(diff < 4)
          {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhopen);
            osDelay(200);
            mygantry.gantrypos.z = z_middle+100;
            osDelay(300);
            MIkp = 1.0;
            MIkd = 0.5;
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,zhuazibhclose);
            pid_reset(&(mygantry.Motor_Y->posPID), 15.0, 0.0, 0);
            osDelay(200);
            step22 = 5;
          }
        }
        if(step22 == 5)
        {
          mygantry.gantrypos.x = x_middle-1100;
          mygantry.gantrypos.y = y_middle;
          float diff1 = fabs(mygantry.gantrypos.x-Lidar1.distance_aver);
          float diff2 = fabs(mygantry.gantrypos.y-Lidar2.distance_aver);
          float diff3 = fabs(mygantry.gantrypos.x - Lidar6.distance_aver - x_fix);
          if(diff1 < 4 && diff2 < 4 && diff3 < 10)
          {
            step22 = 6;//[3]运动到位
          }
        } 
        //jiaquzhuangtai = 6;
      }
    }

  }
    /*

    if(stateflag == 0)//区最左上的箱子到右下方的箱子
    {
      if(runflag == 0)
      {
        mygantry.gantrypos.y  = y_middle;
        float diff = fabs(mygantry.gantrypos.y - Lidar2.distance_aver);
        if(diff < 4)
        {
          runflag = 1;
        }
      }
      if(runflag == 1)
      {
        mygantry.gantrypos.z  = z_down_up;
        float diff = fabs(mygantry.gantrypos.z - z_down_up);
        if(diff < 4)
        {
          runflag = 1;
        }
      }
      
    }
  */
  
    osDelay(1);
  /* USER CODE END uppergoingtask */
}

