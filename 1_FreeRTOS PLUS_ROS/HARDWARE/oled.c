/**
  ******************************************************************************
  * @file    oled.c
  * @author  The Daily Life of An Engineering Girl - RedBook - NewCoder - CSDN_ WeChat Official Account
  * @details link——RedBook: http://xhslink.com/a/ipmoHKmrFZWfb     NewCoder:https://www.nowcoder.com/share/jump/7241315122413047270
  * @version V1.0.0
  * @date    8-June-2025
  * @brief   Source Code File
  ******************************************************************************
  * @attention
  * Copyright (c) 2025 The Daily Life of An Engineering Girl.
  * All rights reserved.
  *
  * *----------------------------------------------------------------------------*/
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"

u8 OLED_GRAM[128][8]; // OLED显存数组，128列×8页（每页8行，共64像素高）

/**************************************************************************
函数功能：刷新OLED显存到物理屏幕
工作原理：
- 分8页（每页8行）逐页写入显存数据
- 每页写入128列数据
- 使用页地址模式提高刷新效率
**************************************************************************/
void OLED_Refresh_Gram(void)
{
    u8 i,n;		    
    for(i=0;i<8;i++)  // 遍历8个页（0~7页）
    {  
        OLED_WR_Byte(0xb0+i, OLED_CMD);   // 设置页地址（0xB0~0xB7对应页0~7）
        OLED_WR_Byte(0x00, OLED_CMD);     // 设置列地址低4位（0x00~0x0F）
        OLED_WR_Byte(0x10, OLED_CMD);     // 设置列地址高4位（0x10~0x1F）
        for(n=0;n<128;n++)
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA); // 写入整列（8像素垂直数据）
    }   
}

/**************************************************************************
函数功能：通过SPI接口向OLED写入单字节数据
参数说明：
- dat : 要写入的字节数据
- cmd : 命令/数据标志（CMD=0写命令，DATA=1写数据）
时序说明：
- 高位(MSB)先传输
- 每个时钟周期传输1bit，共8个时钟周期
**************************************************************************/  
void OLED_WR_Byte(u8 dat, u8 cmd)
{	
    u8 i;			  
    // 设置DC引脚（命令/数据选择）
    if(cmd)// DC引脚：0-命令，1-数据
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();
		  
    // 串行传输8位数据
    for(i=0;i<8;i++)
    {			  
        OLED_SCLK_Clr();                // 时钟线拉低
        if(dat&0x80)
		   OLED_SDIN_Set();             // 发送最高位（MSB First）
		else 
		   OLED_SDIN_Clr();
        OLED_SCLK_Set();                // 时钟上升沿锁存数据
        dat <<= 1;                      // 左移准备下一位
    }				 		  
    OLED_RS_Set(); // 恢复DC引脚默认状态（可选）  
} 

/**************************************************************************
函数功能：开启OLED显示及内部电荷泵
操作时序：
1. 激活内置DC/DC转换器（需要外部电容支持）
2. 发送显示开启命令
注意事项：
- 必须严格按照此顺序开启，否则可能导致屏幕无法点亮
**************************************************************************/
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);  // 使能电荷泵命令（SET_CHARGE_PUMP）
    OLED_WR_Byte(0X14, OLED_CMD);  // 开启内部DC/DC转换（0x14=启用）
    OLED_WR_Byte(0XAF, OLED_CMD);  // 显示开启命令（DISPLAY_ON）
}

/**************************************************************************
函数功能：关闭OLED显示及内部电荷泵
节能模式：
- 关闭显示后功耗可降至微安级
- 再次开启需要重新初始化显示参数
**************************************************************************/
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);  // 电荷泵控制命令
    OLED_WR_Byte(0X10, OLED_CMD);  // 关闭内部DC/DC（0x10=禁用）  
    OLED_WR_Byte(0XAE, OLED_CMD);  // 显示关闭命令（DISPLAY_OFF）
}	

/**************************************************************************
函数功能：清空显存并刷新屏幕
实现原理：
- 将128×8字节显存全部置零
- 执行全屏刷新
视觉效果：
- 屏幕整体变黑（所有像素熄灭）
**************************************************************************/  
void OLED_Clear(void)  
{  
    u8 i,n;  
    for(i=0;i<8;i++)
        for(n=0;n<128;n++)
            OLED_GRAM[n][i] = 0X00;  // 每列8像素置零
    OLED_Refresh_Gram();             // 提交显存到物理屏幕
}

/**************************************************************************
函数功能：在指定坐标绘制像素点
坐标系统：
- x轴：0~127（128列）
- y轴：0~63（64行）
显存结构：
- 每列数据对应8个垂直像素（1字节）
- 页地址计算：y轴位置/8（每页8行）
- 位掩码计算：1 << (7 - (y%8))
**************************************************************************/ 
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp;
    if(x>127 || y>63) return;  // 坐标范围检查
    
    pos = 7 - y/8;          // 计算页地址（从下往上排列）
    bx = y % 8;             // 计算页内行位置
    temp = 1 << (7 - bx);   // 生成位掩码（高位对应上方像素）
    
    t ? (OLED_GRAM[x][pos] |= temp)   // 置1点亮像素
      : (OLED_GRAM[x][pos] &= ~temp);// 置0熄灭像素
}

/**************************************************************************
函数功能：在指定位置显示ASCII字符
字库说明：
- 支持1206（12x6像素）和1608（16x8像素）两种字体
- 字库数据存储在oledfont.h文件中
实现原理：
- 根据字符编码获取点阵数据
- 逐行绘制像素点
参数说明：
- x,y : 起始坐标（左上角）
- chr : 要显示的字符（ASCII码）
- size: 字体大小（12或16）
- mode: 0-反色显示，1-正常显示
**************************************************************************/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{      			    
    u8 temp, t, t1;
    u8 y0 = y;  // 保存原始y坐标
    
    chr = chr - ' '; // 计算字库数组偏移量（跳过不可显示字符）
    
    for(t=0; t<size; t++)  // 遍历字符的每列数据
    {   
        // 获取字模数据（每列8位）
        temp = (size==12) ? oled_asc2_1206[chr][t]  // 12像素字体
                          : oled_asc2_1608[chr][t]; // 16像素字体
                          
        for(t1=0; t1<8; t1++) // 处理每个字节的8位
        {
            // 根据数据位绘制像素
            (temp & 0x80) ? OLED_DrawPoint(x,y,mode)
                          : OLED_DrawPoint(x,y,!mode);
            temp <<= 1;  // 左移处理下一位
            y++;         // 垂直方向下移
            
            // 换列处理（到达字体高度时）
            if((y-y0) == size) 
            {
                y = y0;  // 重置y坐标
                x++;     // 水平右移一列
                break;
            }
        }  	 
    }          
}

/**************************************************************************
函数功能：计算整数次方（辅助函数）
参数说明：
- m: 底数（取值范围0~9）
- n: 指数（建议不超过9，防止溢出）
实现原理：
- 通过循环累乘实现快速幂运算
注意事项：
- 当m=0且n=0时，根据数学定义应返回1
- 结果使用u32类型，最大支持4294967295
**************************************************************************/
u32 oled_pow(u8 m, u8 n)
{
    u32 result = 1;     // 初始化结果为1（处理n=0的情况）
    while(n--) result *= m;  // 循环n次乘法运算（时间复杂度O(n)）
    return result;       // 返回计算结果
}

/**************************************************************************
函数功能：格式化显示数字（支持前导零抑制）
参数说明：
- x,y    : 起始坐标（左上角）
- num    : 要显示的数值（0~4294967295）
- len    : 显示位数（必须≥实际数字位数）
- size   : 字体尺寸（需与字库匹配）
显示特性：
- 自动右对齐
- 超过指定位数时显示实际位数
- 前导零显示为空格（最后一位强制显示）
示例：
ShowNumber(0,0,314,5,16) → "  314"
**************************************************************************/
void OLED_ShowNumber(u8 x, u8 y, u32 num, u8 len, u8 size)
{         	
    u8 t, temp;            // 循环计数器，当前位数值
    u8 enshow = 0;        // 前导零标志（0-未遇到有效数字，1-已遇到有效数字）
						   
    for(t=0; t<len; t++)  // 遍历每一位数字（从高位到低位）
    {
        // 获取当前位数字（从最高位开始）
        temp = (num / oled_pow(10, len-t-1)) % 10;
        
        // 前导零处理逻辑
        if(enshow==0 && t<(len-1)) // 未遇到有效数字且不是最后一位
        {
            if(temp == 0)         // 当前位是零
            {
                // 显示空格替代前导零
                OLED_ShowChar(x+(size/2)*t, y, ' ', size, 1);
                continue;         // 跳过后续处理
            }
            else enshow = 1;      // 遇到第一个非零数字，标记有效位开始
        }
        
        // 显示当前位数字
        OLED_ShowChar(x+(size/2)*t, y, temp+'0', size, 1); 
    }
} 

/**************************************************************************
函数功能：显示ASCII字符串（自动换行）
参数说明：
- x,y : 起始坐标（左上角）
- *p  : 字符串指针（UTF-8编码）
显示特性：
- 使用固定12像素字体
- 每字符宽度8像素，高度16像素
- 横向超出122像素自动换行
- 纵向超出58像素清屏重置
**************************************************************************/
void OLED_ShowString(u8 x, u8 y, const u8 *p)
{
#define MAX_CHAR_POSX 122  // 最大横向坐标（128屏宽 - 6像素余量）
#define MAX_CHAR_POSY 58   // 最大纵向坐标（64屏高 - 6像素余量）          
    while(*p != '\0')      // 遍历字符串直到结束符
    {       
        // 横向换行处理
        if(x > MAX_CHAR_POSX) {
            x = 0;         // 复位到最左侧
            y += 16;       // 下移一行（12像素字体+4像素行间距）
        }
        
        // 纵向换行处理
        if(y > MAX_CHAR_POSY) {
            y = x = 0;     // 复位到左上角
            OLED_Clear();  // 清屏避免显示混乱
        }
        
        // 显示当前字符
        OLED_ShowChar(x, y, *p, 12, 1); // 使用12像素字体
        
        // 光标移动到下一字符位置
        x += 8;  // 每个字符占8像素宽度
        p++;     // 指向下一个字符
    }  
}

/**************************************************************************
Function: Initialize the OLED
Input   : none
Output  : none
函数功能：初始化OLED	
入口参数: 无 
返回  值：无
**************************************************************************/	    
void OLED_Init(void)
{ 	
  GPIO_InitTypeDef GPIO_InitStructure;
	
	//Enable pB port clock
	//使能PB端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD  , ENABLE);  //使能GPIOC

	//Port configuration
	//端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   //输出
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_2MHz;  //2M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//根据设定参数初始化GPIO
	GPIO_Init(GPIOD, &GPIO_InitStructure);				      

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR  , ENABLE);    //PWR使能
	//Allows you to modify RTC and backup registers
  //允许修改RTC和后备寄存器
	PWR_BackupAccessCmd(ENABLE);
	//Turn off the external low speed external clock signal function, PC13, PC14, PC15 can be used as normal IO
	//关闭外部低速外部时钟信号功能 后，PC13 PC14 PC15 才可以当普通IO用
	RCC_LSEConfig(RCC_LSE_OFF);
	//Do not modify the backup register
	//禁止修改后备寄存器
	PWR_BackupAccessCmd(DISABLE);

	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
				  
	OLED_WR_Byte(0xAE,OLED_CMD); //Close display //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //The frequency frequency factor, the frequency of the shock //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0], the frequency dividing factor;[7:4], oscillation frequency //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //Set the number of driver paths //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //Default 0x3f(1/64) //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //Setting display deviation //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //Default is 0//默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //Sets the number of rows to display starting line [5:0] //设置显示开始行 [5:0],行数
													
	OLED_WR_Byte(0x8D,OLED_CMD); //Charge pump setup //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //Bit2, on/off //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //Set up the memory address mode //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00, column address mode;01, line address mode;10. Page address mode;The default 10; //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //Segment redefine setting,bit0:0,0- >;0;1, 0 - & gt;127; //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //Set the COM scan direction;Bit3:0, normal mode;1, Re-define schema COM[n-1]- >;COM0;N: Number of driving paths//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //Set the COM hardware pin configuration //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]configuration //[5:4]配置
	 
	OLED_WR_Byte(0x81,OLED_CMD); //Contrast Settings //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~ 255; Default 0x7f (brightness Settings, the bigger the brighter) //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //Set the pre-charging cycle //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //Setting vcomh voltage multiplier//设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //Global display; Bit0:1, open; 0, close; (white screen/black screen)//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //Settings display mode; Bit0:1, anti-phase display; 0, normal display//设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //Open display //开启显示	 
	OLED_Clear();//清空显示
}  

/**************************************************************************
Function: Display character
Input   : x: indicates the horizontal coordinates displayed; Y: the vertical coordinates that show the display;
          no: the line number in the array of the Chinese character (module) in the hzk-and "array", which is determined by the line number to determine the characters shown in the array,
          The value of the width of the font here must be consistent with the size of the dot matrix value of the use of the word mold.
          font_height: the font is high for the use of the word mold, because my screen pixels are 32hours, 128----0~ 7, and four bits per page
Output  : none
Note: this method is used to show that the Chinese character must satisfy the size of the word that the word model generates the software to generate the same size as the dot matrix
函数功能：显示汉字	
入口参数: x：表示显示的水平坐标; y: 表示显示的垂直坐标;
          no: 表示要显示的汉字（模组）在hzk[][]数组中的行号,通过行号来确定在数组中要显示的汉字,
              这里字体的宽font_width的值必须与用字模制作软件生成字模时的点阵值大小一致;
          font_height:为用字模制作软件生成字模时字体的高,由于我的屏像素为32*128-----0~7共8页，每页4个位
返回  值：无
注意：用这种方法来显示汉字一定要满足用字模生成软件生成的字宽与点阵大小相同才行，否者容易乱码
**************************************************************************/	    
/**************************************************************************
函数功能：显示汉字字符（基于预生成字库）
参数说明：
- x,y         : 起始坐标（左上角，x:0-127, y:0-63）
- no          : 字库数组中的汉字索引（0~字库总数-1）
- font_width  : 汉字宽度（像素，需与字库数据匹配）
- font_height : 汉字高度（像素，必须为8的倍数）
字库要求：
- Hzk16需为二维数组，存储格式为[汉字数量][font_height/8行][font_width列]
- 每个字节表示垂直8个像素点（高位在上，低位在下）
显示原理：
1. 分多页写入（每页8行）
2. 每页按列顺序写入字模数据
**************************************************************************/
void OLED_ShowCHinese(u8 x, u8 y, u8 no, u8 font_width, u8 font_height)
{     			    
    u8 t, i;
    // 按8行分页处理（font_height必须为8的倍数）
    for(i=0; i<(font_height/8); i++)  // 例：32像素高需循环4次（32/8=4页）
    {
        OLED_Set_Pos(x, y+i);  // 设置起始坐标为(x,当前页)
        
        // 逐列写入字模数据（每列8个垂直像素）
        for(t=0; t<font_width; t++)  // 遍历汉字宽度（每列）
        {	
            // 从字库读取数据（三维数组访问）
            // Hzk16[汉字索引][当前页][当前列]
            OLED_WR_Byte(Hzk16[(font_height/8)*no + i][t], OLED_DATA);
        }		
    }
}

/**************************************************************************
函数功能：设置OLED显示起始坐标（页地址模式）
参数说明：
- x : 列地址（0-127）
- y : 页地址（0-7，每页8行）
命令序列：
1. 0xB0+y : 设置页地址（0xB0~0xB7对应页0~7）
2. ((x&0xF0)>>4)|0x10 : 设置列地址高4位（0x10~0x1F）
3. x&0x0F             : 设置列地址低4位（0x00~0x0F）
硬件特性：
- OLED屏幕内存按页组织，每页包含128列×8行
- 列地址分两次写入（高4位+低4位）
**************************************************************************/	  
void OLED_Set_Pos(u8 x, u8 y)
{ 	
    // 设置页地址（0xB0基础值 + y偏移）
    OLED_WR_Byte(0xB0 + y, OLED_CMD);
    
    // 设置列地址高4位（x的4~7位）并添加命令前缀0x10
    OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD);
    
    // 设置列地址低4位（x的0~3位）
    OLED_WR_Byte(x & 0x0F, OLED_CMD); 
}
