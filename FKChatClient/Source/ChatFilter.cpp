/**
*	created:		2012-6-28   6:45
*	filename: 		ChatFilter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天过滤器，过滤掉不合适的内容
*/
//------------------------------------------------------------------------
#include "../Include/ChatFilter.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
// 命令表
const SCmd CMD_TABLE[] = 
{
	{"ping", &CChatFilter::OnPing}
};
//------------------------------------------------------------------------
CChatFilter::CChatFilter(void)
{
	// 加载命令
	for (int i = 0; i < sizeof(CMD_TABLE) / sizeof(SCmd); i++)
	{
		m_cmds[CMD_TABLE[i].pName] = CMD_TABLE[i].funcCmd;
	}
}
//------------------------------------------------------------------------
CChatFilter::~CChatFilter(void)
{
}

//-------------------------------------------------------------------
// 描  述：对文本进行过滤
// 参  数：[输入] const char * pszText - 需要过滤的文本
// 返回值：string - 返回过滤后的文本
//-------------------------------------------------------------------
string CChatFilter::Filter(const char * pszText)
{
	int nLen = strlen(pszText);

	if (nLen == 0)
	{
		return "";
	}

	// 去掉前面的空格
	while (isascii(*pszText) && isspace(*pszText))
	{
		pszText++;
	}

	const char * pStart = pszText;
	const char * pEnd = pStart;

	// 去掉后面的空格
	while (*pszText != '\0')
	{
		if (!isascii(*pszText) || !isspace(*pszText))
		{
			pEnd = pszText + 1;
		}

		pszText++;
	}

	// 去掉前后空格后得到的文本
	nLen = pEnd - pStart;
	string strText(pStart, nLen);
	
	// 计算去掉颜色值后文本的长度
	pszText = strText.c_str();
	while (*pszText != '\0')
	{
		if (*pszText == '&' && isdigit(*(pszText + 1)))
		{
			if (isdigit(*(pszText + 2)))
			{
				pszText += 3;
				nLen -= 3;
			}
			else
			{
				pszText += 2;
				nLen -= 2;
			}
		}
		else
		{
			pszText++;
		}
	}
	if (nLen <= 0)
	{
		return "";
	}

	// 计算使用表情的个数
	int nFaceCount = 0;
	pszText = strText.c_str();
	while (*pszText != '\0')
	{
		if (*pszText == '#' && isdigit(*(pszText + 1)))
		{
			nFaceCount++;
		}

		pszText++;
	}
	if (nFaceCount > 12)
	{
		CChatClient::Instance()->DisplayFloatMessage(SMT_FLOAT, "你使用了太多的表情！");
		return "";
	}

	// 解析命令
	if (ParseCmd(strText))
	{
		return "";
	}

	return strText;	
}

//-------------------------------------------------------------------
// 描  述：解析玩家输入的命令 
//-------------------------------------------------------------------
BOOL CChatFilter::ParseCmd(string strText)
{
	if (strText.size() < 2 || strText[0] != '/')
	{
		return FALSE;
	}
	
	// 获取命令
	string strCmd;
	for (int i = 1; i < strText.size(); i++)
	{
		char c = strText[i];

		if (isspace(c) || c == '\0')
		{
			break;
		}

		strCmd += tolower(c);
	}

	CmdMapItr itr = m_cmds.find(strCmd);
	if (itr != m_cmds.end())
	{
		// 执行命令
		return (this->*((*itr).second))(strText.c_str() + strCmd.size());
	}
	
	return FALSE;
}

//-------------------------------------------------------------------
// 描  述：Ping命令 
//-------------------------------------------------------------------
BOOL CChatFilter::OnPing(const char * pszText)
{
	CChatClient::Instance()->PingStart();
	return FALSE;
}
//------------------------------------------------------------------------