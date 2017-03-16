# FK2DGame
A complete 2D MMOARPG game! God knows how hard I work... 

**Ever tried. Ever failed. No matter. Try Again. Fail again. Fail better.**

FK2DGame
		FK_1_Maker				- 应用程序：将 .CSV 文件转换为 .FK1 文件格式的工具
		FK_2_Maker				- 应用程序：将 .BMP .JPG 文件转换为 .FK2 文件格式的工具
		FKAxis					- 动态链接库：时间相关。秒、毫秒、纳秒精确。时间策略器，定时器。
		FKBasicSys				- 动态链接库：动画、动画资源加载和管理、动画管理对象，逻辑对象，对象池，逻辑对象的创建和管理，魔法技能对象，角色同步相关，模拟粒子系统。
		FKCanvas				- 动态链接库
			ASM加速				- 对于图片色彩，8位16位32位图片绘制加速，字体绘制加速。
			BMP处理				- 基本BMP处理，RLE BMP处理，带Alpha通道的BMP处理。
			基本支持			- 一些对字体，图片格式的定义。
			脏矩形表			- 脏矩形表。
			字体渲染			- 字体渲染结构的基本信息定义。
		FKChatClient			- 动态链接库：常规游戏聊天窗口相关，包括Html保存，表情图片，角色、道具、技能字符串超链接，公告信息记录，富文本聊天框，字符过滤等。
		FKCommon				- 动态链接库
			Math				- 角度，弧度，四元数，射线，数学函数，线段，平面，二维矢量，三维矢量，四维矢量等类。
			Other				- 字符串，字符串数组，引用计数共享指针，临界区等。
		FKController			- 动态链接库
			加密部分			- RSA，SHA_1，MD5，CRC
			数字签名			- 服务器签名验证部分，网卡绑定部分。
			其他				- 屏幕截屏，服务器安全检查（服务器被攻击后的检测记录），游戏输入操作转译。
			逻辑流程			- 游戏登陆流程，场景切换流程，移动同步流程，角色挂机后的操作控制流程。
			数据管理			- 数据单元，流式数据，数据块。
		FKCrashDump				- 动态链接库：Dump文件生成器
		FKEvent					- 动态链接库：为实现一些动画而设计的脚本事件解析器
		FKGame					- 应用程序：游戏主体。大量使用其他模块的接口组合而成
			Adapter				- 显示器的分辨率和色彩深度全屏等属性相关。
			Clan				- 帮会氏族相关
			ClientWorld			- 客户端游戏世界
			Common				- 部分对象结构以及游戏内常用组件对象
			Config				- 部分资源配置读取
			Container			- 抽象容器类
			Controller			- 用户I/O类
			CoolTime			- 冷却时间类
			CrashHelper			- Dump处理类
			DataArchive 		- 内存数据操作类
			DataPacker			- 数据打包/解包
			DebugHelper			- Debug模式下的一些Log支持
			Effect				- 各种游戏逻辑Buffer状态
			Event				- 事件服务器，订阅消息事件，发布者消息事件
			Friend				- 好友模块
			GameApp				- 游戏App对象
			GameGlobal			- 游戏全局静态对象接口管理器
			KeywordFilter		- 关键字过滤器
			MapInfo				- 地图块单元，地图层级等地图基本信息类
			MapView				- 地图以及地图资源对象渲染相关接口
			Network				- 网络通讯相关接口
			PerformanceCheck	- 性能检查
			PrintScreen			- 截屏
			ProcessLimitGuard	- 进程锁
			SceneObj			- 场景对象相关类接口
			Script				- 脚本以及脚本动态更新相关
			Skill				- 技能相关逻辑接口
			Sound				- 音频音效播放接口
			SystemInfo			- 硬件，系统以及平台信息相关
			Team				- 组队相关接口
			Thread				- 线程相关接口
			Timer				- 定时器相关接口
			WndSys				- UI窗口相关接口
			WorkSkill			- 生活技能相关接口
		FKGameModule			- 动态链接库：游戏模块封装
		FKMapView				- 动态链接库：包括地图块管理，地形管理，小地图，地图对象管理，地图天气管理，双轮廓法寻路相关。
		FKResManager			- 动态链接库：资源对象，资源脚本管理，以及资源打包加密解密。
		FKScriptUpdate			- 动态链接库：APHash哈希函数，.cse .csv格式的配置脚本解析以及动态更新库。
		FKSocket				- 动态链接库：简单的模拟网络线程库。
		FKThingClient			- 动态链接库
			Common				- 模板数组分配器，ID生成器，HashTable，版本管理类，对象池管理，以及部分场景静态对象。
			状态				- 生物死亡，移动，乘骑，坐下，站立等行为状态类，以及状态机。
			Other				- 掉落归属，场景内动态逻辑对象工厂，生物类基本命令组，生物类基本外观组，场景生物管理树等。
			组件				- 玩家装备部件，乘骑部件，召唤物部件，主角装备，乘骑，背包等部件相关。
			行为				- 生物接受服务器网络消息，以及客户端操作消息后的行为操作管理。
			逻辑对象			- 玩家，盒子，地图通道，装备，主角，马匹，药品，怪物，摊位，元宝，凭证对象类。
		FKTrace					- 动态链接库：用于Log输出
		FKWndMgr				- 动态链接库：UI窗口管理器，以及逻辑窗体。
		FKWndSys				- 动态链接库：一些基本的UI控件，例如按钮，单、复选框，进度条，输入栏，ListBox，超文本链接，静态图片，模态窗口等。
