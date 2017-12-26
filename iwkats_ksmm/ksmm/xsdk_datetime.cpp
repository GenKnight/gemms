
#include "xsdk_datetime.h"

#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>

int CalcDayDiff(int p_iBgnDate, int p_iEndDate)
{
  int iDayCount = 0;
  SYSTEMTIME stBgnDatetime = {0};
  SYSTEMTIME stEndDatetime = {0};
  memset(&stBgnDatetime, 0x00, sizeof(stBgnDatetime));
  memset(&stEndDatetime, 0x00, sizeof(stEndDatetime));

  stBgnDatetime.wYear = p_iBgnDate / 10000;
  stBgnDatetime.wMonth = (p_iBgnDate / 100 ) % 100;
  stBgnDatetime.wDay = p_iBgnDate % 100;

  stEndDatetime.wYear = p_iEndDate / 10000;
  stEndDatetime.wMonth = (p_iEndDate / 100 ) % 100;
  stEndDatetime.wDay = p_iEndDate % 100;

  iDayCount = (int)DatetimeDiff(stBgnDatetime, stEndDatetime, DT_DAY);
  return iDayCount;
}

void StringToDatetime(
  SYSTEMTIME &p_refstDatetime, 
  const char *p_pszDatetime, 
  const char *p_pszFormat
)
{
  long lDtPartIdx = 0L;
  char szDtPart[256];
  memset((void *)szDtPart, 0x00, sizeof(szDtPart));

  char chFmtChar = 0x00;
  int iFmtCharNum = 0;

  long lLongYear = -1L, lCentury = -1L, lYear = -1L, lMonth = -1L, lDay = -1L;
  long lHour = -1L, lMinute = -1L, lSecond = -1L, lMilliseconds = -1L;

  long lDatetimeIdx = 0L, lDatetimeLen = 0L;
  long lFormatIdx = 0L, lFormatLen = 0L;

  memset((void *)&p_refstDatetime, 0x00, sizeof(p_refstDatetime));

  // 输入参数判断
  if (p_pszDatetime == NULL || p_pszDatetime[0] == 0x00
    || p_pszFormat == NULL || p_pszFormat[0] == 0x00)
  {
    goto __end;
  }
  lDatetimeLen = long(strlen(p_pszDatetime));
  lFormatLen = long(strlen(p_pszFormat));
  if (lDatetimeLen <= 0L || lDatetimeLen < lFormatLen)
  {
    goto __end;
  }

  // 根据格式描述析取各部分的数值
  for (lDatetimeIdx = 0L, lFormatIdx = 0L, lDtPartIdx = 0L;
    lDatetimeIdx <= lDatetimeLen && lFormatIdx <= lFormatLen;
    lDatetimeIdx++, lFormatIdx++)
  {
    if (chFmtChar != 0x00 && p_pszFormat[lFormatIdx] != chFmtChar && iFmtCharNum > 0)
    {
      switch (chFmtChar)
      {
      case DT_CENTURY:
        if (lCentury != -1L) 
        {
          goto __end;
        }
        lCentury = atoi(szDtPart);
        break;
      case DT_YEAR:
        if (lYear != -1L) 
        {
          goto __end;
        }
        lYear = atoi(szDtPart);
        break;
      case DT_MONTH:
        if (lMonth != -1L) 
        {
          goto __end;
        }
        lMonth = (iFmtCharNum == 3) ? MONTH_NUMERIC(szDtPart) : atoi(szDtPart);
        break;
      case DT_DAY:
        if (lDay != -1L) 
        {
          goto __end;
        }
        lDay = atoi(szDtPart);
        break;
      case DT_HOUR:
        if (lHour != -1L) 
        {
          goto __end;
        }
        lHour = atoi(szDtPart);
        break;
      case DT_MINUTE:
        if (lMinute != -1L) 
        {
          goto __end;
        }
        lMinute = atoi(szDtPart);
        break;
      case DT_SECOND:
        if (lSecond != -1L) 
        {
          goto __end;
        }
        lSecond = atoi(szDtPart);
        break;
      case DT_MILLISECONDS:
        if (lMilliseconds != -1L) 
        {
          goto __end;
        }
        lMilliseconds = atoi(szDtPart);
        break;
      default:
        goto __end;
      }
      lDtPartIdx = 0L;
      memset((void *)szDtPart, 0x00, sizeof(szDtPart));
      chFmtChar = 0x00;
      iFmtCharNum = 0;
    }
    if (p_pszFormat[lFormatIdx] != 0x00
      && strchr("CYMDhmsn", p_pszFormat[lFormatIdx]) != NULL)
    {
      szDtPart[lDtPartIdx++] = p_pszDatetime[lDatetimeIdx];
      chFmtChar = p_pszFormat[lFormatIdx];
      iFmtCharNum++;
    }
  }
  lLongYear = lYear == -1L ? lYear : ((lCentury == -1L ? 0 : lCentury) * 100L + lYear);

  // 判断日期时间各部分数值的合法性
  if ((lLongYear != -1L && (lLongYear < 0L || lLongYear > 9999L))
    || (lMonth != -1L && (lMonth < 1L || lMonth > 12L))
    || (lLongYear != -1L && lMonth != -1L && lDay != -1L && (lDay < 1L || lDay > DAYS_OF_MONTH(lLongYear, lMonth)))
    || (lHour != -1L && (lHour < 0L || lHour > 23L))
    || (lMinute != -1L && (lMinute < 0L || lMinute > 59L))
    || (lSecond != -1L && (lSecond < 0L || lSecond > 59L))
    || (lMilliseconds != -1L && (lMilliseconds < 0L || lMilliseconds > 999L)))
  {
    goto __end;
  }

  // 将析取出来的数值存储至SYSTEMTIME结构
  p_refstDatetime.wYear = (unsigned short)(lLongYear < 0L ? 0L : lLongYear);
  p_refstDatetime.wMonth = (unsigned short)(lMonth < 1L ? 1L : lMonth);
  p_refstDatetime.wDay = (unsigned short)(lDay < 1L ? 1L : lDay);
  p_refstDatetime.wDayOfWeek = (unsigned short)(DAY_OF_WEEK(p_refstDatetime.wYear, p_refstDatetime.wMonth, p_refstDatetime.wDay));
  p_refstDatetime.wHour = (unsigned short)(lHour < 0L ? 0L : lHour);
  p_refstDatetime.wMinute = (unsigned short)(lMinute < 0L ? 0L : lMinute);
  p_refstDatetime.wSecond = (unsigned short)(lSecond < 0L ? 0L : lSecond);
  p_refstDatetime.wMilliseconds = (unsigned short)(lMilliseconds < 0L ? 0L : lMilliseconds);

__end:
  return;
}


char * DatetimeToString(
  char *p_pszDatetime, 
  long p_lBufferSize, 
  const char *p_pszFormat, 
  const SYSTEMTIME &p_refDateTime
)
{
  bool bRetCode = false;

  char chFmtChar = 0x00;
  int iFmtCharNum = 0;

  long lDtPartLen = 0L;
  char szDtPart[256];
  memset((void *)szDtPart, 0x00, sizeof(szDtPart));

  long lDestBuffIdx = 0L;
  long lFormatIdx = 0L, lFormatLen = 0L;

  // 输入参数判断
  if (p_pszDatetime == NULL || p_lBufferSize <= 0L)
  {
    goto __end;
  }
  memset((void *)p_pszDatetime, 0x00, sizeof(char) * p_lBufferSize);

  if (p_pszFormat == NULL || p_pszFormat[0] == 0x00)
  {
    goto __end;
  }
  lFormatLen = long(strlen(p_pszFormat));

  // 根据格式描述输出日期数据
  for (lFormatIdx = 0L; lFormatIdx <= lFormatLen; lFormatIdx++)
  {
    if (chFmtChar != 0x00 && p_pszFormat[lFormatIdx] != chFmtChar && iFmtCharNum > 0)
    {
      memset((void *)szDtPart, 0x00, sizeof(szDtPart));
      switch (chFmtChar)
      {
      case DT_CENTURY:
      case DT_YEAR:
        _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, 
          (chFmtChar == 'C') ? int(p_refDateTime.wYear <= 0 ? 0 : p_refDateTime.wYear) / 100 :
          (chFmtChar == 'Y' && iFmtCharNum <= 2) ? int(p_refDateTime.wYear <= 0 ? 0 : p_refDateTime.wYear) % 100 
                                                 : int(p_refDateTime.wYear <= 0 ? 0 : p_refDateTime.wYear));
        break;
      case DT_MONTH:
        if (iFmtCharNum == 3)
        {
          _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%s", MONTH_ENG_NAME(p_refDateTime.wMonth));
        }
        else
        {
          _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, int(p_refDateTime.wMonth <= 0 ? 1 : p_refDateTime.wMonth));
        }
        break;
      case DT_DAY:
        _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, int(p_refDateTime.wDay <= 0 ? 1 : p_refDateTime.wDay));
        break;
      case DT_HOUR:
        _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, int(p_refDateTime.wHour <= 0 ? 0 : p_refDateTime.wHour));
        break;
      case DT_MINUTE:
        _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, int(p_refDateTime.wMinute <= 0 ? 0 : p_refDateTime.wMinute));
        break;
      case DT_SECOND:
        _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, int(p_refDateTime.wSecond <= 0 ? 0 : p_refDateTime.wSecond));
        break;
      case DT_MILLISECONDS:
        _snprintf_s(szDtPart, _countof(szDtPart), sizeof(szDtPart) - 1, "%0*d", iFmtCharNum, int(p_refDateTime.wMilliseconds <= 0 ? 0 : p_refDateTime.wMilliseconds));
        break;
      default:
        goto __end;
      }
      lDtPartLen = long(strlen(szDtPart));
      if ((lDestBuffIdx + lDtPartLen) > (p_lBufferSize - 1L))
      {
        goto __end;
      }
      memcpy((void *)(p_pszDatetime + lDestBuffIdx), szDtPart, lDtPartLen);
      lDestBuffIdx += lDtPartLen;

      chFmtChar = 0x00;
      iFmtCharNum = 0;
    }
    if (p_pszFormat[lFormatIdx] != 0x00 
      && strchr("CYMDhmsn", p_pszFormat[lFormatIdx]) != NULL)
    {
      chFmtChar = p_pszFormat[lFormatIdx];
      iFmtCharNum++;
    }
    else 
    {
      p_pszDatetime[lDestBuffIdx++] = p_pszFormat[lFormatIdx];
    }
  }

  bRetCode = true;

__end:
  if (!bRetCode)
  {
    memset((void *)p_pszDatetime, 0x00, sizeof(char) * p_lBufferSize);
  }
  return p_pszDatetime;
}


void DatetimeAdd(
  SYSTEMTIME &p_refstODatetime, 
  char p_chDtPart, 
  long long p_llInterval, 
  const SYSTEMTIME &p_refstIDatetime
)
{
  bool bRetCode = false;

  long long llYear = -1LL, llMonth = -1LL, llDay = -1LL;
  long long llHour = -1LL, llMinute = -1LL, llSecond = -1LL, llMilliseconds = -1LL;

  long long llInterval = 0LL;
  llInterval = p_llInterval;

  // 输入参数判断
  if (DATETIME_IS_VALID(p_refstIDatetime))
  {
    goto __end;
  }

  p_refstODatetime = p_refstIDatetime;
  
  // 根据p_chDtPart指示逐部分计算
  switch (p_chDtPart)
  {
  case DT_MILLISECONDS:
    llMilliseconds = p_refstIDatetime.wMilliseconds;
    llMilliseconds += llInterval;
    llSecond = llMilliseconds / 1000LL;
    llMilliseconds = llMilliseconds % 1000LL;
    if (llMilliseconds < 0LL)
    {
      llSecond--;
      llMilliseconds += 1000LL;
    }
    llInterval = llSecond;
    p_refstODatetime.wMilliseconds = (unsigned short)(llMilliseconds);
    if (llInterval == 0LL)
    {
      break;
    }
  case DT_SECOND:
    llSecond = p_refstIDatetime.wSecond;
    llSecond += llInterval;
    llMinute = llSecond / 60LL;
    llSecond = llSecond % 60LL;
    if (llSecond < 0LL)
    {
      llMinute--;
      llSecond += 60LL;
    }
    llInterval = llMinute;
    p_refstODatetime.wSecond = (unsigned short)(llSecond);
    if (llInterval == 0L) 
    {
      break;
    }
  case DT_MINUTE:
    llMinute = p_refstIDatetime.wMinute;
    llMinute += llInterval;
    llHour = llMinute / 60LL;
    llMinute = llMinute % 60LL;
    if (llMinute < 0LL)
    {
      llHour--;
      llMinute += 60LL;
    }
    llInterval = llHour;
    p_refstODatetime.wMinute = (unsigned short)(llMinute);
    if (llInterval == 0LL) 
    {
      break;
    }
  case DT_HOUR:
    llHour = p_refstIDatetime.wHour;
    llHour += llInterval;
    llDay = llHour / 24LL;
    llHour = llHour % 24LL;
    if (llHour < 0LL)
    {
      llDay--;
      llHour += 24LL;
    }
    llInterval = llDay;
    p_refstODatetime.wHour = (unsigned short)(llHour);
    if (llInterval == 0LL) 
    {
      break;
    }
  case DT_DAY:
    {
      long long JD = 0L, L = 0L, N = 0L, I = 0L, J = 0L;

      llDay = p_refstIDatetime.wDay;
      llMonth = p_refstIDatetime.wMonth;
      llYear = p_refstIDatetime.wYear;

      // 将Gregorian Date转换为Julian Date
      JD = (1461 * (llYear + 4800 + (llMonth - 14) / 12 )) / 4 
           + (367 * (llMonth - 2 - 12 * ((llMonth - 14) / 12))) / 12 
           - (3 * ((llYear + 4900 + (llMonth - 14) / 12) / 100)) / 4 
           + llDay - 32075;

      JD += llInterval;

      // 将Julian Date转换为Gregorian Date
      L = JD + 68569;
      N = (4 * L) / 146097;
      L = L - ((146097 * N + 3) / 4);
      I = 4000 * (L + 1) / 1461001;
      L = L - ((1461 * I) / 4) + 31; 
      J = (80 * L) / 2447;
      llDay = L - (2447 * J) / 80;
      llMonth = J + 2 - 12 * (J / 11);
      llYear = 100 * (N - 49) + I + (J / 11);

      // 运算溢出
      if (llYear < 0LL || llYear > 9999LL)
      {
        goto __end;
      }
    }
    p_refstODatetime.wDay = (unsigned short)(llDay);
    p_refstODatetime.wMonth = (unsigned short)(llMonth);
    p_refstODatetime.wYear = (unsigned short)(llYear);
    p_refstODatetime.wDayOfWeek = DAY_OF_WEEK(p_refstODatetime.wYear, p_refstODatetime.wMonth, p_refstODatetime.wDay);
    llInterval = 0LL;
    break;
  case DT_MONTH:
    llMonth = p_refstIDatetime.wYear * 12 + p_refstIDatetime.wMonth;
    llMonth += llInterval;
    p_refstODatetime.wYear = (unsigned short)((llMonth % 12) == 0 ? llMonth / 12 - 1 : llMonth / 12);
    p_refstODatetime.wMonth = (unsigned short)((llMonth % 12) == 0 ? 12 : llMonth % 12);
    // 调整日期的日部分数值
    {
      long lDaysOfMonth = 0L;
      lDaysOfMonth = DAYS_OF_MONTH(p_refstODatetime.wYear, p_refstODatetime.wMonth);
      p_refstODatetime.wDay = p_refstODatetime.wDay > (unsigned short)(lDaysOfMonth) ? (unsigned short)(lDaysOfMonth) : p_refstODatetime.wDay;
      p_refstODatetime.wDayOfWeek = DAY_OF_WEEK(p_refstODatetime.wYear, 
                                                p_refstODatetime.wMonth, 
                                                p_refstODatetime.wDay);
    }
    break;
  case DT_YEAR:
    llYear = p_refstIDatetime.wYear;
    llYear += llInterval;
    // 运算溢出
    if (llYear < 0LL || llYear > 9999LL)
    {
      goto __end;
    }
    
    p_refstODatetime.wYear = (unsigned short)(llYear);
    // 调整日期的日部分数值
    {
      long lDaysOfMonth = 0L;
      lDaysOfMonth = DAYS_OF_MONTH(p_refstODatetime.wYear, p_refstODatetime.wMonth);
      p_refstODatetime.wDay = p_refstODatetime.wDay > (unsigned short)(lDaysOfMonth) ? (unsigned short)(lDaysOfMonth) : p_refstODatetime.wDay;
      p_refstODatetime.wDayOfWeek = DAY_OF_WEEK(p_refstODatetime.wYear, 
                                                p_refstODatetime.wMonth, 
                                                p_refstODatetime.wDay);
    }
    break;
  default:
    goto __end;
  }

  bRetCode = true;

__end:
  if (!bRetCode)
  {
    memset((void *)&p_refstODatetime, 0x00, sizeof(p_refstODatetime));
  }
  return;
}


long long DatetimeDiff(
  const SYSTEMTIME &p_refstBgnDatetime,  
  const SYSTEMTIME &p_refstEndDatetime, 
  char p_chDtPart /*= DT_DAY*/
)
{
  long long llDtDiff = 9223372036854775807LL;
  long long llBgnJD = 0LL, llEndJD = 0LL;

  if (DATETIME_IS_VALID(p_refstBgnDatetime) 
    || DATETIME_IS_VALID(p_refstEndDatetime))
  {
    goto __end;
  }
  
  switch (p_chDtPart)
  {
  case DT_YEAR:
    llDtDiff = (long long)(p_refstEndDatetime.wYear) - (long long)(p_refstBgnDatetime.wYear);
    break;
  case DT_MONTH:
    llDtDiff = ((long long)(p_refstEndDatetime.wYear) * 12LL + (long long)(p_refstEndDatetime.wMonth)) -
               ((long long)(p_refstBgnDatetime.wYear) * 12LL + (long long)(p_refstBgnDatetime.wMonth));
    break;
  case DT_DAY:
  case DT_HOUR:
  case DT_MINUTE:
  case DT_SECOND:
  case DT_MILLISECONDS:
    llBgnJD = (long long)((1461 * (p_refstBgnDatetime.wYear + 4800 + (p_refstBgnDatetime.wMonth - 14) / 12 )) / 4 
                        + (367 * (p_refstBgnDatetime.wMonth - 2 - 12 * ((p_refstBgnDatetime.wMonth - 14) / 12))) / 12 
                        - (3 * ((p_refstBgnDatetime.wYear + 4900 + (p_refstBgnDatetime.wMonth - 14) / 12) / 100)) / 4 
                        + p_refstBgnDatetime.wDay - 32075);
    llEndJD = (long long)((1461 * (p_refstEndDatetime.wYear + 4800 + (p_refstEndDatetime.wMonth - 14) / 12 )) / 4 
                        + (367 * (p_refstEndDatetime.wMonth - 2 - 12 * ((p_refstEndDatetime.wMonth - 14) / 12))) / 12 
                        - (3 * ((p_refstEndDatetime.wYear + 4900 + (p_refstEndDatetime.wMonth - 14) / 12) / 100)) / 4 
                        + p_refstEndDatetime.wDay - 32075);
    switch (p_chDtPart)
    {
    case DT_DAY:
      llDtDiff = llEndJD - llBgnJD;
      break;
    case DT_HOUR:
      llDtDiff = (llEndJD * 24LL + (long long)(p_refstEndDatetime.wHour)) 
                 - (llBgnJD * 24LL + (long long)(p_refstBgnDatetime.wHour));
      break;
    case DT_MINUTE:
      llDtDiff = (llEndJD * 1440LL + p_refstEndDatetime.wHour * 60LL + (long long)(p_refstEndDatetime.wMinute)) 
                 - (llBgnJD * 1440LL + p_refstBgnDatetime.wHour * 60LL + (long long)(p_refstBgnDatetime.wMinute));
      break;
    case DT_SECOND:
      llDtDiff = (llEndJD * 86400LL + p_refstEndDatetime.wHour * 3600LL + p_refstEndDatetime.wMinute * 60LL 
                 + (long long)(p_refstEndDatetime.wSecond)) 
                 - (llBgnJD * 86400LL + p_refstBgnDatetime.wHour * 3600LL + p_refstBgnDatetime.wMinute * 60LL 
                 + (long long)(p_refstBgnDatetime.wSecond));
      break;
    case DT_MILLISECONDS:
      llDtDiff = (llEndJD * 86400000LL + p_refstEndDatetime.wHour * 3600000LL 
                 + p_refstEndDatetime.wMinute * 60000LL + p_refstEndDatetime.wSecond *  1000LL 
                 + (long long)(p_refstEndDatetime.wMilliseconds))       
                 - (llBgnJD * 86400000LL + p_refstBgnDatetime.wHour * 3600000LL
                 + p_refstBgnDatetime.wMinute * 60000LL + p_refstBgnDatetime.wSecond * 1000LL
                 + (long long)(p_refstBgnDatetime.wMilliseconds));
      break;
    }
    break;
  default:
    goto __end;
  }

__end:
  return llDtDiff;
}
