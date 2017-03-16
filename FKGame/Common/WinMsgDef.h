/**
*	created:		2012-6-26   3:09
*	filename: 		WinMsgDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		窗口系统中数据与常量的定义
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 全屏消息
#define IDM_FULLSCREEN                  1016
// 玩家居中消息
#define IDM_CENTER                      1015
// 更改渲染模式消息
#define IDM_FLAG                        1017
// PING消息
#define IDM_PING                        1112
// 读取服务器信息
#define IDM_GET_SERVER_INFO             1113
//------------------------------------------------------------------------
// 界面设置窗口
#define WND_GRAPHICSSETTING                   183
const int WE_GRAPHICSSETTING_APPLYSETTING        = 1;    //发到设置窗口, 要求重新设置所有设定
const int WE_GRAPHICSSETTING_SETSKILLDATAFLAG    = 2;    //设置技能有效位,表示技能数据已经下来了
const int WE_WC_GRAPHICSSETTING_SETLOCKFIRE      = 3;    //设置自动攻击状态, WPARAM为状态
const int WE_AW_GRAPHICSSETTING_FULLSCREENMODE   = 4;    //设置全屏状态, WPARAM为状态
const int WE_WC_GRAPHICSSETTING_DISPACTORNAME    = 5;    //设置显示玩家名称, WPARAM为状态
const int WE_WC_GRAPHICSSETTING_DISPCREATUREHP   = 6;    //显示生物血条, WPARAM为状态
//------------------------------------------------------------------------
//格式 WE_ + CW/WC + 窗口名称 + 消息名称
const int WE_CW_ALLWND_SHOW				= 99;		//每当窗口显示或隐形时，都会发送此消息，WPARAM为1时，打开时发送，WPARAM为0时，隐形时发送

//通知窗口注册(取消注册)快捷键
const int WE_AW_ALLWND_REGACCELE          = 1000;     //当人物初始化完, 会向有需要的窗口发送此消息, 
const int WE_AW_ALLWND_UNREGACCELE        = 1001;     //切换地图时, 会向有需要的窗口发送此消息, 通知其取消注册快捷键


#define WND_CUSTOMMSGBOX				10			//自定义对话框
// 弹出框
const int WE_CW_MSGBOX_CANCEL				= 0;	//弹出一个对话框
const int WE_CW_MSGBOX_CONFIRM				= 1;	//弹出一个对话框
const int WE_CW_MSGBOX_SHOW					= 2;	//设置弹出消息，如果设定的为自定义消息框，WPARAM为数据大小，LARPAM为SETCUSTOMMSGBOX结构
const int WE_CW_MSGBOX_RETRY				= 3;    //重试
const int WE_CW_MSGBOX_QUIT					= 4;    //退出
const int WE_CW_MSGBOX_SETBTN				= 5;    //WPARM:1 = 确定，2 = 取消，3 = 重试， 4 = 退出。 LPARAM为对应的回调函数
const int WE_CW_MSGBOX_SETCONTEXT			= 6;    //设定自制对话框的上下文，如果设定了，如果用户处理了此对话框时，返回给用户
const int WE_CW_MSGBOX_HIDE					= 7;    //消亡当前自制消息框
const int WE_CW_MSGBOX_POP					= 8;	//让对方框停止或者开始弹出。WPARM:1 = 弹出，2 = 停止
const int WE_CW_MSGBOX_TIMER				= 9;	//设定窗口时间的死活。WPARM:1 = 运转，2 = 停止
const int WE_CW_MSGBOX_UPDATETEXT			= 10;	//更新当前正在显示的文本,(只更新文本,窗口不关闭) WPARM 为字符指针
const int WE_CW_MSGBOX_CLEAR				= 11;   //消除消息框队列中所有的消息,及关闭当前消息窗口(如果正在显示的话)
const int WE_AW_MSGBOX_GETCOURSE			= 12;	//查询窗口弹出的原因 WPARAM 为int* , 用于接收原因
const int WE_AW_MSGBOX_CLEARONCE			= 13;	//清除一类对话框 WPARAM 为原因, 如指定,则当前窗口原因与此参数一致时,才会清走
const int WE_AW_MSGBOX_SETCURSORTOBTN		= 14;	//将鼠标移到某个BTN上, wParam为按钮索引; 如果索引无效(按钮不显示),没有任何返应



enum    //弹出原因对话框与输入框的原因
{
	POPCOURSE_UNKNOWN = 0,    			//不明原因
	POPCOURSE_TASKLOG,        			//任务日志
	POPCOURSE_DIVIDEGOODS,    			//为拆分物品而打开
	POPCOURSE_DISCARDGOODS,     		//为丢弃而打开
	POPCOURSE_RESELECTACTOR,    		//为重选角色而打开
	POPCOURSE_EXITGAME,         		//为离开游戏而打开
	POPCOURSE_FORGETSKILL,				//为忘记技能而打开
	POPCOURSE_TEAM,						//队伍的原因
	POPCOURSE_QQLOGIN,					//QQ登录的原因	
};
#define MAKECOURSE(MAIN, SUB)  ( ((MAIN)&0x00ffff)  | (((SUB)&0x00ffff) << 16))


#define WND_INPUTWND					20			//输入窗口

#define WND_INTRO						30			// 引导画面
const int WE_AW_INTRO_CANCELMOVIE				= 10;  //玩家决定取消动画播放, 由对话框回调发过来
const int WE_AW_INTRO_TRYMOVIEAGAIN				= 11;  //玩家决定再尝试动画播放, 由对话框回调发过来


#define WND_SELECTWORLD					40			// 选择游戏世界窗口

//登陆框 WND_LOGIN
const int WE_WC_LOGIN_CONFIRM				= 0;	//请求登陆	wParam : 用户名 lParam : PASSWORDS结构指针
const int WE_WC_LOGIN_CANCEL				= 1;	//取消登陆
const int WE_WC_LOGIN_REGISTER				= 2;	//注册
const int WE_CW_LOGIN_USERNAME				= 7;	//默认登陆用户名
const int WE_CW_LOGIN_MSG					= 8;	//设置提示信息
const int WE_CW_LOGIN_LOCK       			= 12;   //锁定窗口,令用户不能选择
const int WE_CW_LOGIN_UNLOCK       			= 13;   //解除锁定窗口
const int WE_CW_LOGIN_WAIT       			= 14;   //登录时需要排队   wParam排在您前面的人数
const int WE_CW_LOGIN_ERRORINFO       		= 15;   //登录时,从服务器返回的错误消息
const int WE_AW_LOGIN_RELOGIN       		= 16;   //从anywhere发过来,要求重新连接登录服
const int WE_CW_LOGIN_CLEARPASSWORD    		= 17;   //控制器发过来, 要求清除密码框中的密码
const int WE_WW_LOGIN_SHOWCAPTIONWARNING	= 18;   //设置大小写警告的显示与否
const int WE_WW_LOGIN_SOFTKBINPUT       	= 19;   //用户从软键盘上输入了，wParam 为LPCSTR,指向用户按下的键名
const int WE_WW_LOGIN_QQLOGIN           	= 20;   //从引导窗口或视频窗口传来, 要求用QQ方式登录
const int WE_WC_LOGIN_SETPASSPORT         	= 21;   //设置密宝, wParam为是否使用密宝, lParam 为密宝指针LPCSTR
const int WE_WW_LOGIN_INITGRAPHICSCOMPLETE 	= 22;   //初始化何林图包完成
const int WE_WW_LOGIN_GETACCOUNTNAME    	= 23;   //取帐号名, wParam为缓冲区指针, lParam为缓冲区大小
const int WE_WC_SETEXTRAPASSWORD         	= 24;   //设置附加密码, wParam为附加密码LPCSTR


#define WND_LOGIN				    		50			// 登录窗口
#define WND_LOGIN_BAK						52			// 登录窗口的背景
const int WE_WW_LOGINBAK_SHOWLABEL      	= 40;   //显示底部的LABEL控件, wParam为是否显示, lParam为显示内容LPCSTR

#define WND_LOGIN_BAK_NOTICE				54			// 登录窗口公告


#define WND_MINIMAP						60			//小地图
//小地图窗口
const int WE_CW_MAP_REFRESH					= 0;
const int WE_CW_MAP_SETMAPAREA				= 1;
const int WE_WW_MAP_SETREGIONTIP			= 2;    //设置当前区域的提示, wPARAM为LPCSTR

const int WE_CW_CHAT_ACTIVATE       		= 9999;   //激活聊天对话框


#define WND_ACTORSELECT					80				// 角色选择窗口
#define WND_ACTORSELECT_BAK				81				// 角色选择窗口背景
//角色选择窗口 WND_ACTORSELECT
const int WE_WC_ACTOR_ENTERGAME				=	0;		//进入游戏
const int WE_WC_ACTOR_QUITGAME				=	1;		//退出游戏
const int WE_WC_ACTOR_DELETE				=	3;		//删除角色
const int WE_CW_ACTOR_ERROR                 =   4;       //错误信息 
const int WE_CW_REFRESH_ACTORINFO			=	7;		//通知窗口,刷新新的角色数据(wParam为缓冲区指针,lParam为数据长度)
const int WE_CW_ACTOR_SETLOCK			    =	8;		//通知窗口,锁定(wParam为锁定标置)
const int WE_AW_ACTORSELECT_CLEARPORT	  	=	9;		// 小退时发出来, 清除各个角色的端口信息, 防止用户在没有连通登录服前,重新进入游戏


// 角色创建窗口
#define WND_ACTORCREATE					82
//角色创建窗口 WND_ACTORCREATE
const int WE_WC_ACTORCREATE_QUEST	=	0;		//向控制器发出 创建角色请求
const int WE_CW_ACTORCREATE_ERROR   =   1;      //控制器发回来, 显示错误消息



//主菜单窗口
#define WND_MAIN                         90	
const int WE_CW_MAIN_SHORTCUT               = 0;   //控制器发过来,要求处理玩家的快捷指令,
												   //入口参数: wParam 快捷栏的行列数的地址, ex.  21表示2行1列
												   //          lParam 传一个int 的地址,用于返回技能ID
												   //出口参数: wParam 窗口管理器是否处理了该指令,例如技能等是不会处理的
												   //          lParam 窗口没有处理时,这个值是技能ID			

const int WE_WC_MAIN_SKILL               = 1;   //窗口传到控制器,表示玩家用鼠标点击了快捷栏技能
const int WE_WC_MAIN_GOODS               = 2;   //窗口传到控制器,表示玩家用鼠标点击了快捷栏物品 wParam为目标类型, lParam为UID的地址  
const int WE_AW_MAIN_SAVESHORTCUT        = 3;   //要求保存用户的快捷键庙设定
const int WE_AW_MAIN_LOADSHORTCUT        = 4;   //要求读取用户的快捷键庙设定
const int WE_CW_MAIN_SKILL_SELF          = 5;   //发到控制器对自己使用技能,  wParam为技能ID
const int WE_AW_MAIN_CHANGEPKMODE        = 6;   //由PK模块发过来,通知更换PK模式 . wParam为当前模式
const int WE_CW_MAIN_MSGCENTER_PUSH      = 7;   //控制器发过来,表示信息中心有信息进去了, wParam为队列号
const int WE_CW_MAIN_MSGCENTER_STOPFLASH = 8;   //控制器发过来,表示可以停止闪动了, wParam为队列号
const int WE_WC_MAIN_MSGCENTER_POP       = 9;   //发到控制器去 , 表示玩家点击了信息 , 弹出信息wParam为消息队列号
const int WE_AW_MAIN_GETSHORTCUTDATA     = 10;  //g_UserSetting发来,要求取SHORTCUT数据(只取前10项), wParam为一个数组指针
const int WE_AW_MAIN_SETSHORTCUTPAGE     = 11;  //设置快捷栏的页数, wParam为页数(例如下线时会发过来, 重置为第一页)



//职业技能窗口
#define WND_VOCATIONSKILL                        100       
const int WE_VOCATIONSKILL_LOAD               = 1;   //重新读取所有职业技能 wParam,为玩家职业,  lParam为技能类型,如(职业, 生活,工作)

//修炼技能窗口
#define WND_PRACTICESKILL                        102  
const int WE_PRACTISESKILL_LOAD               = 1;   //重新读取所有训练技能 wParam,为玩家职业,  lParam为技能类型,如(职业, 生活,工作)

//工作技能窗口
#define WND_WORKSKILL                            104  



// 系统设置
#define WND_SYSTEMMENU  					     106
const int WE_CW_SHOWMENU                 = 1;   //控制器发过来,要求重选角色
const int WE_WC_CLEARCMMMAND             = 2;   //窗口发到控制器,要求删除玩家的命令队列
const int WE_AW_SHOWEXITWND              = 3;   //不知什么地方发过来, 要求退出游戏



#define WND_SYSTEMMENU_SOUND                     107  


//额外的快捷栏窗口
#define WND_EXTRASHORTCUT                     115
const int WE_AW_EXTRASHORTCUT_SAVESHORTCUT        = 3;   //要求保存用户的快捷键庙设定
const int WE_AW_EXTRASHORTCUT_LOADSHORTCUT        = 4;   //要求读取用户的快捷键庙设定, wParam为职业ID 
const int WE_AW_EXTRASHORTCUT_BINDALLITEM         = 5;   //人物初始化完成后,MAIN窗口会发此消息过来
const int WE_CW_EXTRASHORTCUT_SHORTCUT            = 6;   //控制器发过来,要求处理玩家的快捷指令,
														 //入口参数: wParam 快捷栏的行列数的地址, ex.  21表示2行1列
														 //          lParam 传一个int 的地址,用于返回技能ID
														 //出口参数: wParam 窗口管理器是否处理了该指令,例如技能等是不会处理的
													     //          lParam 窗口没有处理时,这个值是技能ID			
const int WE_CW_EXTRASHORTCUT_CHANGESKILLSHAPE    = 7;   //玩家转换技能形态了, wParam为当前形态
const int WE_CW_EXTRASHORTCUT_SKILLDATAOK    = 8;   //技能模块发过来,表示技能数据已经从服务器下载到本地了
const int WE_AW_EXTRASHORTCUT_GETSHORTCUTDATA     = 10;  //g_UserSetting发来,要求取SHORTCUT数据(只取前10项), wParam为一个数组指针


//目标环窗口
#define WND_TARGETRIM                    120
const int WE_CW_TARGET_SET			         = 0;   //更新了新目标, WPARAM是一个TARGETINFO指针, WPARAM为NULL时隐藏窗口
const int WE_WC_TARGET_TRACE			     = 9;   //跟踪某一目标, WPARAM是一个生物UID地址指针, 后跟保持距离, 至小要为1
const int WE_AW_TARGET_CHANGEMONSTERHOLDER    = 10;  //通知目标环,检查怪物归属  wParam为一个SC_HOLDPRIZE_OBSERVE_CONTEXT指针
const int WE_AC_TARGET_SETTARGET              = 11;  //通知控制器目标环,更新目标  wParam 为一个UID指针
const int WE_AC_TEAMTARGET_ATTACK             = 12;  //由于组队窗口没有窗口ID, 这个消息放到目标环这里了 
                                                     //玩家的技能点中了队友窗口施法, wParam为目标UID地址
const int WE_CW_TARGET_CHANGECAPTION	  = 13;   //队长变了
const int WE_WC_TARGET_CANCELTARGET       = 14;      //通知控制器,取消目标环


//玩家信息窗口
#define WND_PLAYERINFO                    122
const int WE_CW_PLAYERINFO_SET			            = 0;   //更新了数据
const int WE_CW_PLAYERINFO_STARTLISTENBORN			= 1;   //开始监听人物出生消息
const int WE_CW_PLAYERINFO_CHANGECAPTION			= 2;   //队长变了


//装备窗口
#define WND_EQUIPMENT                    125
const int WE_CW_UPDATEEQUIP   			= 0;   // wParam - int* 需要更新的位置列表 // lParam - int 需要更新的个数
const int WE_AW_SETLIGHT                = 5;   // 点亮某个VIEWBOX    wParam 为IContainerGoods指针 
const int WE_AW_RESETLIGHT              = 6;   // 取消点亮全部VIEWBOX
const int WE_AW_SHOWEQUIPMENT           = 7;   // 显示人物的装备, wParam为人物UID的地址
const int WE_AW_SETTOOLTIP              = 8;   // 设置TOOLTIP, wParam为人物职业

//包裹窗口
#define WND_PACKAGE                      130
const int WE_CW_DISCARDGOODS   			= 4;   // 控制器发过来,要求丢弃鼠标上的物品
const int WE_AW_USEGOODS   		    	= 5;   // 不知从什么地方发过来,要求使用物品wParam为物品ID, lParam为物品UID指针(如果为NULL, 则会自动选同类的物品来使用) 


//拾取(宝箱)窗口
#define WND_TREASUREBOX                   135 
const int WE_TREASURE_PROGRESSBAR_SHOW   			= 2;   // 箱子模块传过来，要求显示进度条　wParam 为时间（ms), wParam为0则隐藏进度条
const int WE_TREASURE_OPEN           			    = 3;   // 箱子模块传过来，要求打开宝箱　wParam 箱子ＩＤ，lParam为ＵＩＤ地址
const int WE_TREASURE_CLOSE           			    = 4;   // 箱子模块传过来，要求关闭宝箱　wParam 箱子ＩＤ，lParam为ＵＩＤ地址
const int WE_CW_TREASURE_AUTOPICKUP		            = 5;   // 控制器发过来,要求自动捡宝箱

//进度条窗口
#define WND_PROCESSBAR                    137 
const int WE_PROCESSBAR_SHOW   		     	= 2;   // 要求显示进度条　wParam 为时间（ms) , lParam为描述(LPCSTR)
const int WE_PROCESSBAR_HIDE   		     	= 3;   // 要求隐藏进度条

//LUA专用进度条窗口
#define WND_LUAPROCESSBAR                    138 
//所用消息码同上面的进度条窗口一样,再多一个
const int WE_LUAPROCESSBAR_SETPOS            =  4;   //wParam 为x 座标, lParm为Y座标



//炼丹窗口
#define WND_PHARMACY                      145

//合成窗口
#define WND_COMPOSE                       154

//精炼窗口
#define WND_REFINE                        147 

//提炼窗口
#define WND_EPURATE                       149

//镶嵌窗口
#define WND_INLAY                         152

//商人窗口
#define WND_TRADE                         166

//仓库窗口
#define WND_STORE                         168

//玩家之间交易窗口
#define WND_DEALING                         170

//LOADING窗口
#define WND_LOADING                         171
const int WE_CW_LOGINLOADING                         = 1;  //控制器发来,表示角色的登录请求已经连通服务器了,要求把选人界面挡住

//死亡窗口
#define WND_DEAD                            173

//世界地图窗口
#define WND_WORLDMAP                        174 
const int WE_WORLDMAP_UPDATETEAMATE   		     	= 3;   //其它地方发过, 更新队友信息;wParam为UID[5] 的指针
const int WE_WORLDMAP_ADDTASKPOINT   		     	= 4;   //其它地方发过, 在大地图上增加显示一个任务点 wParam为一个LUATASKPOINT指针
const int WE_WORLDMAP_REMOVETASKPOINT   		    = 5;   //其它地方发过, 删除任务点
const int WE_WC_WORLDMAP_LONGMOVE   		        = 6;   //发到控制器, 作长距离移动(wparam为x, lparam 为y)  
const int WE_WC_WORLDMAP_LOADMAPBLOCK  		        = 9;   //发到控制器, 更新地图占位块 (无参数, 控制器自己到global里取)
 

//炼丹窗口(EX)
#define WND_PHARMACY_EX                      175

//合成窗口(EX)
#define WND_COMPOSE_EX                        176

//精炼窗口(EX)
#define WND_REFINE_EX                         177 

//提炼窗口(EX)
#define WND_EPURATE_EX                        178

//镶嵌窗口(EX)
#define WND_INLAY_EX                          179

//请求工作技能窗口
#define WND_REQUEST_WORKSKILL                 180

//其它玩家的装备窗口
#define WND_EQUIPMENT_OTHER                   182
const int WE_EQUIPMENTOTHER_UPDATEROLE        = 1;    //更新角色信息, wParam为IActor指针

//创建氏族窗口
#define WND_CREATE_CLAN                     184

//无氏族玩家的氏族面板
#define WND_NONCLAN_PANEL                   185

//氏族面板窗口
#define WND_CLAN_PANEL                      186

//氏族成员权限窗口
#define WND_CLAN_MEMBERACC                  187

//发送邀请加入氏族的窗口
#define WND_CLAN_SENDINV                    188

//接受邀请加入氏族的窗口
#define WND_CLAN_RECVINV                    189

//查看氏族详细信息的窗口
#define WND_CLAN_INTRO                      190

//氏族中的文字编辑窗口
#define WND_CLAN_TEXTEDIT                   191


#define WND_CLAN_PERSONACC                  192

#define WND_CLAN_LBATTEL                    193

#define WND_CLAN_MSGAR                      194


//自动回复消息
#define WND_AUTOREVIVE                      195
const int WE_AUTOREVIVE_LOADSETTING    = 1;    //读取回复设置

//宠物命令窗口
#define WND_PETCOMMAND                      198
const int WE_CW_PETCOMMAND_QUESTEXECUTE            = 1;     //控制器发过来,要求执行宠物指令
									        			    //入口参数: wParam 宠物指令												
const int WE_WC_PETCOMMAND_EXECUTE                 = 2;     //窗口发到按制器,要求执行宠物指令
												            //入口参数: wParam 宠物指令		

//卷轴落下窗口
#define WND_DROPSCROLL                      199             //卷轴落下窗口
const int WE_WW_DROPSCROLL_ADD                     = 1;     //服务器发下来,要求添加卷轴任务
															//入口参数: wParam 为 LUAADDSCROLL指针	
const int WE_WW_DROPSCROLL_REMOVE                  = 2;     //服务器发下来,要求删除卷轴任务
															//入口参数: wParam 为 LUAADDSCROLL指针
const int WE_WW_DROPSCROLL_CLEARALL                = 3;     //要求删除所有卷轴任务, 小退时处理

//播放视频的窗口
#define WND_MOVIE                           200 
const int WE_WW_MOVIE_FINISH                         = 1;   //MMSYSTEM.dll发过来, 表示视频播放完成了

//制作人员名单窗口
#define WND_CREATOR                         201

//软键盘窗口
#define WND_SOFTKEYBOARD                         202

//检验码窗口
#define  WND_VALIDATIONCODE                      203
const int WE_WC_VALIDATIONCODE_SUBMIT		= 1;    // 确定
const int WE_WC_VALIDATIONCODE_REFRESH		= 2;	// 刷新


//检验码窗口
#define  WND_GENERALPASSWORD                       205
const int WE_AW_GENERALPASSWORD_LAUNCH       	= 1;    // 显示密码窗口, wParam为IWndCallback 指针, lParam为要显示的字符串指针


#define WND_CLAN_CREATEUNION                       206 

#define WND_CLAN_HOMESTEAD                         209   //氏族家园窗口

#define WND_CLAN_SKILL                             211   //氏族技能窗口
const int WE_AW_CLANDATA_UPDATA                  = 3;     //收到服务器氏族的新数据, 要更新氏族技能数据, wParam为LUACLANDATA指针
const int WE_AW_CLANSKILLDATA_UPDATA             = 4;     //收到服务器氏族技能的新数据, 要更新氏族技能数据, wParam为LUACLANSKILLDATA指针

#define WND_EXTRAPASSWORD                          213   //附加密码窗口
const int WE_WC_GETEXTRAPASSWORD                 = 2;     //设置附加密码, wParam为一个缓冲区指针, lParam为缓冲区大小

#define WND_COMPOSEV2                              214   //合成窗口V2 版

#define WND_FUSIONV2                              215   //提炼窗口V2 版

#define WND_PHARMACYV2                              216   //炼丹窗口V2 版

#define WND_INLAYV2                               217   //镶嵌窗口V2 版

#define WND_REFINEV2                               218  //精炼窗口V2 版

#define WND_EPURATEV2                              219   //提炼窗口V2版

#define WND_TRADECITY                              220   //商城窗口
