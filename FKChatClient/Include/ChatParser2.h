/**
*	created:		2012-6-28   7:35
*	filename: 		ChatParser2
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
#include <list>
//------------------------------------------------------------------------
using std::string;
using std::list;
//------------------------------------------------------------------------
enum eChatItem
{
	eText,  // 普通文本
	eGoods  // 物品
};
//------------------------------------------------------------------------
#define  GOODS_SPEARATOR  (21)
//------------------------------------------------------------------------
class CChatParse 
{
	friend class CChatController;
public:
	struct SChatItem
	{
		int nType;         // 目前支持物品
		string strContent; // 消息内容
		string strShow;    // 显示的消息内容
		int nPos;         // 第一个位置,从0开始
		int nLen;         // 长度
	};
	typedef list<SChatItem> ChatItemList;
public:
	void Parse(string& str)
	{
		if( str.empty() )
		{
			ClearBackText();
			return ;
		}
		if( m_strText.empty() )
		{
			m_strText = str;
			ParseInitial(m_strText,m_ChatItemList);
			CountPos(m_ChatItemList);
			MakeText(m_ChatItemList);
		}else{
			//先去掉过去
			// 增加
			int nPos = str.find(m_strText);
			if( nPos >= 0)
			{
				OnAdd(m_strText,str);
				return;			
			}
			// 减少
			nPos = m_strText.find(str);
			if(nPos>=0)
			{
				OnDel(m_strText,str);
				return;
			}
			// 替换
			OnReplace(m_strText,str);

		}
	}
	string& GetText()
	{
		return m_strText;
	}
	void ClearBackText()
	{
		m_ChatItemList.clear();
		m_strText.clear();
	}
	void GetPacketBuf(char *pBuf,int& nLen)
	{

	}
	int GetItemCount(int nType)
	{
		int nCount = 0;
		ChatItemList::iterator itr  = m_ChatItemList.begin();
		for(;itr!=m_ChatItemList.end();itr++)
		{
			if( (*itr).nType == nType )
			{
				nCount++;
			}
		}
		return nCount; 
	}
	BOOL IsHasSpecialContent()
	{
		ChatItemList::iterator itr  = m_ChatItemList.begin();
		for(;itr!=m_ChatItemList.end();itr++)
		{
			if( (*itr).nType > eText )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
protected:
	void ParseInitial(string& str,ChatItemList& chatItemList)
	{
		chatItemList.clear();
		char *psz = (char *)str.c_str();
		string strTemp;
		SChatItem ChatITem1;
		SChatItem ChatITem2;
		while(*psz!='\0')
		{
			// 检查物品
			if( ParseGoods(&psz,ChatITem1) )
			{
				if( ParseText((char *)strTemp.c_str(),ChatITem2))
				{
					chatItemList.push_back(ChatITem2);
					strTemp.clear();
				}
				chatItemList.push_back(ChatITem1);
			}else{
				// 什么都不是，那就是文本
				strTemp += (*psz);
				psz++;
			}
		}
		if( ParseText((char *)strTemp.c_str(),ChatITem2))
		{
			chatItemList.push_back(ChatITem2);
			strTemp.clear();
		}
	}
protected:
	bool ParseGoods(char **pszText,SChatItem& ChatITem)
	{
		ChatITem.strContent.clear();
		ChatITem.strShow.clear();
		char *psz = *pszText;
		if((*psz!=GOODS_SPEARATOR))
		{
			return false;
		}
		for(int i=0;i<20;i++)
		{
			if(*(psz+i+1) == '\0')
			{
				return false;
			}
		}
		if(*(psz+21)!=GOODS_SPEARATOR)
		{
			return false;
		}
		*pszText = psz+22;
		ChatITem.nType =  eGoods;
		ChatITem.strContent += (*(psz+0));
		for(int j=1;j<21;j++)
		{ 
			ChatITem.strContent += (*(psz+j));
			ChatITem.strShow += (*(psz+j));
		}
		ChatITem.strContent += (*(psz+21));
		ChatITem.strShow ='['+ChatITem.strShow;
		ChatITem.strShow +=']';
		return true;
	}

	bool ParseText(char *pszText,SChatItem& ChatITem)
	{
		if( pszText == NULL
			|| *pszText=='\0')
		{
			return false;
		}
		ChatITem.nType = eText;
		ChatITem.strContent = pszText;
		ChatITem.strShow =  ChatITem.strContent;
		return true;
	}

	void CountPos(ChatItemList& chatITemlist)
	{
		int nPos = 0;
		ChatItemList::iterator itr  = chatITemlist.begin();
		for(;itr!=chatITemlist.end();itr++)
		{
			(*itr).nPos = nPos;
			(*itr).nLen = (*itr).strShow.length();
			nPos+=(*itr).strShow.length();
		}
	}

	void MakeText(ChatItemList& chatITemlist)
	{
		m_strText ="";
		ChatItemList::iterator itr  = chatITemlist.begin();
		for(;itr!=chatITemlist.end();itr++)
		{
			m_strText += (*itr).strShow;
		}
	}

	void MergeText()
	{
		ChatItemList::iterator itr,temp,ipre =  m_ChatItemList.begin();
		itr = ipre;
		++itr;
		while(itr!=m_ChatItemList.end())
		{
			if( (*itr).nType == eText && (*ipre).nType == eText )
			{
				//
				string str = (*ipre).strShow;
				//
				(*ipre).strContent += (*itr).strContent;
				(*ipre).strShow += (*itr).strShow;
				temp = itr;
				itr++;
				m_ChatItemList.erase(temp);

			}else{
				itr++;
				ipre++;
			}
		}
	}
	void OnAdd(string& strOld,string& strNew)
	{
		int nPos = strOld.length();
		string strAdd = strNew.substr(nPos);
		ChatItemList chatitemlist;
		ParseInitial(strAdd,chatitemlist);
		m_ChatItemList.insert(m_ChatItemList.end(),chatitemlist.begin(),chatitemlist.end());
		MergeText();
		CountPos(m_ChatItemList);
		MakeText(m_ChatItemList);
	}

	void OnReplace(string& strOld,string& strNew)
	{
		int nOldPrePos = 0,nOldBackPos = strOld.length()-1;
		int nNewBackPos = strNew.length()-1;
		for(nOldPrePos=0;nOldPrePos<strOld.length();nOldPrePos++)
		{
			if(strOld.at(nOldPrePos)!=strNew.at(nOldPrePos))
			{
				break;
			}
		}
		for(;nOldBackPos>nOldPrePos;nOldBackPos--,nNewBackPos--)
		{
			if(strOld.at(nOldBackPos)!=strNew.at(nNewBackPos))
			{
				break;
			}
		}
		//找到受影响的列表
		ChatItemList::iterator iter1 = m_ChatItemList.end(),iter2 = m_ChatItemList.end(),
			iter = m_ChatItemList.begin();
		ChatItemList chatlist;
		for(;iter!=m_ChatItemList.end();iter++)
		{
			if( nOldPrePos>=(*iter).nPos && nOldPrePos<=((*iter).nPos+(*iter).nLen-1)
				&& nOldBackPos>=(*iter).nPos && nOldBackPos<=((*iter).nPos+(*iter).nLen-1))
			{
				if( iter1 == m_ChatItemList.end())
				{
					iter1 = iter;
				}
				iter2 = iter;
			}
		}
		if(iter1 != m_ChatItemList.end() )
		{
			int nPos = (*iter1).nPos;
			int nLen = nNewBackPos-nPos+2;
			if( nLen<=0 )
			{
				return;
			}
			string str = strNew.substr(nPos,nLen);
			ParseInitial(str,chatlist);
			iter = ++iter2;
			m_ChatItemList.erase(iter1,iter2);
			m_ChatItemList.insert(iter,chatlist.begin(),chatlist.end());
			CountPos(m_ChatItemList);
			MakeText(m_ChatItemList);
		} 


	}

	void OnDel(string& strOld,string& strNew)
	{
		int nLength = strNew.length();
		ChatItemList::iterator itr = m_ChatItemList.begin();
		int nPos = -1;
		for(;itr!=m_ChatItemList.end();itr++)
		{
			if( (nLength-1>=(*itr).nPos) && (nLength-1)<=((*itr).nPos+(*itr).nLen-1) )
			{
				nPos = (*itr).nPos;
				m_ChatItemList.erase(itr,m_ChatItemList.end());
				break;
			}
		}
		if( nPos >= 0)
		{
			string str = strNew.substr(nPos);
			ChatItemList chatitemlist;
			ParseInitial(str,chatitemlist);
			m_ChatItemList.insert(m_ChatItemList.end(),chatitemlist.begin(),chatitemlist.end());
			MergeText();
			CountPos(m_ChatItemList);
			MakeText(m_ChatItemList);			
		}

	}
private:
	string m_strText;
	ChatItemList m_ChatItemList;

};
//------------------------------------------------------------------------