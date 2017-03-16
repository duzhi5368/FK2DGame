/**
*	created:		2012-6-29   14:55
*	filename: 		WndDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define TITLE_WND 456
//------------------------------------------------------------------------
#define WND_CHAT						70			// 聊天窗口
const static int WE_AW_UPDATE_IMEICON   =  4;		//IME窗口发来, 要求更新输入法图片
const static int WE_AW_CONTENT_ADDGOODS =  5;		//在聊天内容中,插入装备wParam为装备UID
//------------------------------------------------------------------------
#define WND_CHAT_CHANNEL                71          // 聊天子窗口 :频道选择窗口
const static int WE_WW_CHANNEL_SETCHANNEL  =  2;    // 频道子窗口收到本消息后,设置频道 wparam为目标频道
//------------------------------------------------------------------------
#define WND_CHAT_FACE                   72          // 聊天子窗口 :表情窗口
#define WND_CHAT_IME                    73          // 聊天子窗口 :输入法窗口
const static int WE_AW_DISCONNECT_IMEUPDATE            =  5;    //要求退订输入法的更新消息
//------------------------------------------------------------------------
#define WND_CHAT_COLOR                  74          // 聊天子窗口 :颜色窗口
#define WND_CHAT_PERSON                 75          // 聊天子窗口 :历史角色窗口
#define WND_CHAT_CONTENT                76          // 聊天子窗口 :历史聊天内容窗口
//------------------------------------------------------------------------
#define WND_INPUTWND					20			//输入窗口
//------------------------------------------------------------------------
// 弹出输入框
const int WE_CW_INPUT_CANCEL				= 0;	//弹出一个对话框
const int WE_CW_INPUT_OK					= 1;	//弹出一个对话框
const int WE_CW_INPUT_MSG					= 2;	//设置弹出消息
const int WE_WC_INPUT_VALUE					= 3;	//通知输入数值
const int WE_WC_INPUT_CANCEL				= 4;    //取消输入
const int WE_CW_INPUT_SET                   = 5;    //进行设置，比如最大长度和可输入汉字
const int WE_CW_INPUT_POP					= 6;	//让对方框停止或者开始弹出。WPARM:1 = 弹出，2 = 停止
const int WE_CW_INPUT_TIMER					= 7;	//设定窗口时间的死活。WPARM:1 = 运转，2 = 停止
const int WE_AW_INPUT_GETCOURSE				= 8;	//查询窗口弹出的原因 WPARAM 为int* , 用于接收原因
const int WE_CW_INPUT_CLEAR				    = 9;   	//消除消息框队列中所有的消息,及关闭当前消息窗口(如果正在显示的话)
const int WE_CW_INPUT_CLEARONCE				= 10;  	//清除一个对话框
//------------------------------------------------------------------------