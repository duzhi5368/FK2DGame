/**
*	created:		2012-7-2   21:46
*	filename: 		SysTime
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <time.h>
//------------------------------------------------------------------------
class CSysTime;
//------------------------------------------------------------------------
class CSysTimeSpan
{
public:
	CSysTimeSpan()
	{
		m_dMilliseconds = 0;
	}
	CSysTimeSpan(double dMilliseconds)
	{
		m_dMilliseconds = dMilliseconds;
	}
	CSysTimeSpan(long lDays, long lHours, long lMinutes, long lSeconds, long lMilliseconds = 0)
	{
		m_dMilliseconds = (((lDays * 24 + lHours) * 60 + lMinutes) * 60 + lSeconds) * 1000 + lMilliseconds;
	}
	CSysTimeSpan& operator+=(const CSysTimeSpan& timeSpan)
	{
		m_dMilliseconds += timeSpan.m_dMilliseconds;
		return *this;
	}
	CSysTimeSpan& operator-=(const CSysTimeSpan& timeSpan)
	{
		m_dMilliseconds -= timeSpan.m_dMilliseconds;
		return *this;
	}
	CSysTimeSpan operator+(const CSysTimeSpan& timeSpan) const
	{
		return CSysTimeSpan(m_dMilliseconds + timeSpan.m_dMilliseconds);
	}
	CSysTimeSpan operator-(const CSysTimeSpan& timeSpan) const
	{
		return CSysTimeSpan(m_dMilliseconds - timeSpan.m_dMilliseconds);
	}
	BOOL operator>(const CSysTimeSpan& right) const
	{
		return m_dMilliseconds > right.m_dMilliseconds;
	}
	BOOL operator<(const CSysTimeSpan& right) const
	{
		return m_dMilliseconds < right.m_dMilliseconds;
	}
	BOOL operator>=(const CSysTimeSpan& right) const
	{
		return m_dMilliseconds >= right.m_dMilliseconds;
	}
	BOOL operator<=(const CSysTimeSpan& right) const
	{
		return m_dMilliseconds <= right.m_dMilliseconds;
	}
	BOOL operator==(const CSysTimeSpan& right) const
	{
		return m_dMilliseconds == right.m_dMilliseconds;
	}
	operator double() const { return m_dMilliseconds; }
private:
	double m_dMilliseconds;
};
//------------------------------------------------------------------------
class CSysTime
{
public:
	CSysTime()
	{
		m_time.u.HighPart = 0;
		m_time.u.LowPart = 0;
	}
	CSysTime(LONG lHighPart, DWORD dwLowPart)
	{
		m_time.u.HighPart = lHighPart;
		m_time.u.LowPart = dwLowPart;
	}
	CSysTime(time_t t)
	{
		SYSTEMTIME gmt;
		gmt.wYear = 1970;
		gmt.wMonth = 1;
		gmt.wDay = 1;
		gmt.wHour = 0;
		gmt.wMinute = 0;
		gmt.wSecond = 0;
		gmt.wMilliseconds = 0;
		gmt.wDayOfWeek = 3;
		CSysTime time;
		time.SetGmt(gmt);
		*this = time + CSysTimeSpan(static_cast<double>(t) * 1000);
	}
	BOOL IsValidTime() const
	{
		return m_time.u.HighPart != 0 || m_time.u.LowPart != 0;
	}
	void SetFileGmt(const FILETIME& gmt)
	{
		m_time.u.HighPart = gmt.dwHighDateTime;
		m_time.u.LowPart = gmt.dwLowDateTime;
	}
	void GetFileGmt(FILETIME& gmt) const
	{
		gmt = *reinterpret_cast<const FILETIME*>(&this->m_time.u);
	}
	BOOL SetFileTime(const FILETIME& localTime)
	{
		return ::LocalFileTimeToFileTime(&localTime, reinterpret_cast<FILETIME*>(&m_time.u));
	}
	BOOL GetFileTime(FILETIME& localTime) const
	{
		return ::FileTimeToLocalFileTime(
			reinterpret_cast<const FILETIME*>(&this->m_time.u), &localTime);
	}
	BOOL SetGmt(const SYSTEMTIME& gmt)
	{
		return ::SystemTimeToFileTime(&gmt, reinterpret_cast<FILETIME*>(&m_time.u));
	}
	BOOL GetGmt(SYSTEMTIME& gmt) const
	{
		return ::FileTimeToSystemTime(
			reinterpret_cast<const FILETIME*>(&this->m_time.u), &gmt);
	}
	BOOL SetTime(const SYSTEMTIME& localTime)
	{
		FILETIME fileTime;
		if(::SystemTimeToFileTime(&localTime, &fileTime))
		{
			return SetFileTime(fileTime);
		}
		return FALSE;
	}
	BOOL GetTime(SYSTEMTIME& localTime) const
	{
		FILETIME fileTime;
		if(GetFileTime(fileTime))
		{
			return ::FileTimeToSystemTime(&fileTime, &localTime);
		}
		return FALSE;
	}
	void SetToCurrentTime()
	{
		SYSTEMTIME current;
		::GetSystemTime(&current);
		SetGmt(current);
	}
	BOOL operator>(const CSysTime& time) const
	{
		return m_time.u.HighPart > time.m_time.u.HighPart || 
			m_time.u.HighPart == time.m_time.u.HighPart && 
			(m_time.u.HighPart >= 0 && m_time.u.LowPart > time.m_time.u.LowPart ||
			m_time.u.HighPart < 0 && m_time.u.LowPart < time.m_time.u.LowPart);
	}
	BOOL operator<(const CSysTime& time) const
	{
		return m_time.u.HighPart < time.m_time.u.HighPart || 
			m_time.u.HighPart == time.m_time.u.HighPart && 
			(m_time.u.HighPart >= 0 && m_time.u.LowPart < time.m_time.u.LowPart ||
			m_time.u.HighPart < 0 && m_time.u.LowPart > time.m_time.u.LowPart);
	}
	BOOL operator==(const CSysTime& time) const
	{
		return m_time.u.HighPart == time.m_time.u.HighPart && 
			m_time.u.LowPart == time.m_time.u.LowPart;
	}
	CSysTime operator+(const CSysTimeSpan& timeSpan) const
	{
		double temp1 = static_cast<double>(0xffffffff) + 1;
		double temp = m_time.u.HighPart * temp1 + m_time.u.LowPart + static_cast<double>(timeSpan) * 10000;
		LONG temp2 = static_cast<LONG>(temp / temp1);
		return CSysTime(temp2, static_cast<DWORD>(temp - temp2 * temp1));
	}
	CSysTimeSpan operator-(const CSysTime& right) const
	{
		double temp1 = static_cast<double>(0xffffffff) + 1;
		return CSysTimeSpan((m_time.u.HighPart * temp1 + m_time.u.LowPart - 
			(right.m_time.u.HighPart * temp1 + right.m_time.u.LowPart)) / 10000);
	}
	CSysTime& operator+=(const CSysTimeSpan& timeSpan)
	{
		*this = *this + timeSpan;
		return *this;
	}
	CSysTime& operator-=(const CSysTimeSpan& timeSpan)
	{
		*this = *this + CSysTimeSpan(-timeSpan);
		return *this;
	}
	TCHAR* GetStrTime(TCHAR* pStrBuf, long lLen, BOOL bShowMs = FALSE) const
	{
		if(lLen <= 19 || bShowMs && lLen <= 23)
			return NULL;
		SYSTEMTIME localTime;
		if(GetTime(localTime))
		{
			if(bShowMs)
			{
				::wsprintf(pStrBuf, _T("%04i-%02i-%02i %02i:%02i:%02i %03i"), 
					localTime.wYear, localTime.wMonth, localTime.wDay, 
					localTime.wHour, localTime.wMinute, localTime.wSecond, 
					localTime.wMilliseconds);
			}
			else
			{
				::wsprintf(pStrBuf, _T("%04i-%02i-%02i %02i:%02i:%02i"), 
					localTime.wYear, localTime.wMonth, localTime.wDay, 
					localTime.wHour, localTime.wMinute, localTime.wSecond);
			}
			return pStrBuf;
		}
		return NULL;
	}
	operator LARGE_INTEGER() const
	{
		return m_time;
	}
	operator time_t() const			// time_t只能表示1970前后68年的时间
	{
		SYSTEMTIME gmt0;
		gmt0.wYear = 1970;
		gmt0.wMonth = 1;
		gmt0.wDay = 1;
		gmt0.wHour = 0;
		gmt0.wMinute = 0;
		gmt0.wSecond = 0;
		gmt0.wMilliseconds = 0;
		gmt0.wDayOfWeek = 3;
		CSysTime time0;
		time0.SetGmt(gmt0);
		return static_cast<time_t>(static_cast<double>(*this - time0) / 1000);
	}
	void GetLocalTm(tm& time) const
	{
		time_t t = *this;
		time = *::localtime(&t);
	}
	BOOL SetLocalTm(tm& t)
	{
		SYSTEMTIME time;
		time.wYear = 1900 + t.tm_year;
		time.wMonth = t.tm_mon + 1;
		time.wDay = t.tm_mday;
		time.wHour = t.tm_hour;
		time.wMinute = t.tm_min;
		time.wSecond = t.tm_sec;
		time.wMilliseconds = 0;
		time.wDayOfWeek = t.tm_wday;
		return SetTime(time);
	}
private:
	LARGE_INTEGER m_time;	// file GMT
};
//------------------------------------------------------------------------