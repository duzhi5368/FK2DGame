/**
*	created:		2012-6-29   14:04
*	filename: 		ChatWndMsg
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		定义了各聊天子窗口中的交互的消息
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define MSG_COLOR_SETEDIT      90     //通知颜色选择窗口,设置输出编辑框信息,wParam为编辑框地址, 
//lParam 为 TRUE  ,当用户选定颜色后,将编辑框设为选定颜色
//lParam 为 FALSE  ,当用户选定颜色后,将颜色字符串添加到编辑框中
//------------------------------------------------------------------------
#define MSG_FACE_SETEDIT       90     //通知表情选择窗口,设置编辑框信息,wParam为编辑框地址
//------------------------------------------------------------------------
#define MSG_IME_CHANGED        99     //由IME窗口向WND_CHAT发送,通知其输入法已改变,wParam为新图标指针, lParam为输入法索引(切换时用)
#define MSG_CHANNEL_CHANGED    100    //由频道窗口向WND_CHAT发送,通知其频道已经改变,wParam为新频道图片指针,lParam为频道ID
//------------------------------------------------------------------------
#define MSG_POPTXT_SETEDIT    90
#define MSG_POPTXT_ADDTEXT    91   //通知弹出文本窗口,添加最近的人物
#define MSG_POPTXT_GETTEXT    92   //通知弹出文本窗口,取字符串指针, wParam为索引, lParam为LPCSTR *(输出)
//------------------------------------------------------------------------
#define MSG_SET_PERSON        70   //由聊天模块发过来,设置私聊人名  (wParam 为字符指针)
#define MSG_SET_MSG           71   //由聊天模块发过来,设置聊天信息  (wParam 为字符指针)
#define MSG_GET_FACE          72   //由聊天模块发过来,取表情图片指针 (wParam 为IBitmap ** ,lParam 为index)
#define MSG_GET_COLOR         73   //由聊天模块发过来,取表情颜色指针 (wParam 为WPIXEL * ,lParam 为index)
//------------------------------------------------------------------------