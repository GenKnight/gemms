#ifndef __XSDK_DATETIME_H__
#define __XSDK_DATETIME_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#if !defined(OS_IS_WINDOWS)
  #if defined(_WIN32) || defined(_WIN64)
    #define OS_IS_WINDOWS
  #endif  // defined(_WIN32) || defined(_WIN64)
#endif  // !defined(OS_IS_WINDOWS)

#if defined(OS_IS_WINDOWS)
  #include <windows.h>
//  #define snprintf              _snprintf
  #if _MSC_VER < 1400
    #define vsnprintf           _vsnprintf
  #endif  // _MSC_VER < 1400
#endif  // defined(OS_IS_WINDOWS)

#define DT_CENTURY              'C'
#define DT_YEAR                 'Y'
#define DT_MONTH                'M'
#define DT_DAY                  'D'
#define DT_HOUR                 'h'
#define DT_MINUTE               'm'
#define DT_SECOND               's'
#define DT_MILLISECONDS         'n'

#if !defined(IS_LEAPYEAR)
#define IS_LEAPYEAR(year)       (((year) % 400) == 0 || (((year) % 4) == 0 && ((year) % 100) != 0))
#endif  // IS_LEAPYEAR

#if !defined(DAYS_OF_YEAR)
#define DAYS_OF_YEAR(year, month, day) \
( \
  ( \
    (month) ==  1 ?   0 : \
    (month) ==  2 ?  31 : \
    (month) ==  3 ?  59 : \
    (month) ==  4 ?  90 : \
    (month) ==  5 ? 120 : \
    (month) ==  6 ? 151 : \
    (month) ==  7 ? 181 : \
    (month) ==  8 ? 212 : \
    (month) ==  9 ? 243 : \
    (month) == 10 ? 273 : \
    (month) == 11 ? 304 : \
    (month) == 12 ? 334 : 0 \
  ) + ((month) > 2 && IS_LEAPYEAR(year) ? 1 : 0L) \
  + (day) \
)
#endif  // DAYS_OF_YEAR
  
#if !defined(DAYS_OF_MONTH)
#define DAYS_OF_MONTH(year, month) \
( \
  ((month) == 1 || (month) == 3 || (month) == 5 || (month) == 7 || (month) == 8 || (month) == 10 || (month) == 12) ? 31 : \
  ((month) == 4 || (month) == 6 || (month) == 9 || (month) == 11) ? 30 : \
  ((month) == 2 && (!IS_LEAPYEAR(year))) ? 28 : \
  ((month) == 2 && IS_LEAPYEAR(year)) ? 29 : 0 \
)
#endif  // DAYS_OF_MONTH

#if !defined(DAY_OF_WEEK)
#define DAY_OF_WEEK(year, month, day) \
( \
  ( \
    (day) \
    + ((year) - (14 - (month)) / 12) \
    + ((year) - (14 - (month)) / 12) / 4 \
    - ((year) - (14 - (month)) / 12) / 100 \
    + ((year) - (14 - (month)) / 12) / 400 \
    + (31 * ((month) + 12 * ((14 - (month)) / 12) - 2) / 12) \
  ) % 7 \
)
#endif  // DAY_OF_WEEK

#if !defined(DATETIME_IS_VALID)
#define DATETIME_IS_VALID(stDatetime) \
( \
  (stDatetime.wYear < 0 || stDatetime.wYear > 9999) \
  || (stDatetime.wMonth < 1 || stDatetime.wMonth > 12) \
  || (stDatetime.wDay < 1 || stDatetime.wDay > DAYS_OF_MONTH(stDatetime.wYear, stDatetime.wMonth)) \
  || (stDatetime.wHour < 0 || stDatetime.wHour > 23) \
  || (stDatetime.wMinute < 0 || stDatetime.wMinute > 59) \
  || (stDatetime.wSecond < 0 || stDatetime.wSecond > 59) \
  || (stDatetime.wMilliseconds < 0 || stDatetime.wMilliseconds > 999) \
)
#endif  // DATETIME_IS_VALID

#if !defined(TIME_INTERVAL)
#define TIME_INTERVAL(t1, t2) \
((t1.wHour <= t2.wHour)\
 ? ((t2.wHour * 3600 + t2.wMinute * 60 + t2.wSecond) - (t1.wHour * 3600 + t1.wMinute * 60 + t1.wSecond))\
 : (86400 - (t1.wHour * 3600 + t1.wMinute * 60 + t1.wSecond) + (t2.wHour * 3600 + t2.wMinute * 60 + t2.wSecond)))
#endif  // TIME_INTERVAL

#if !defined(MONTH_ENG_NAME)
#define MONTH_ENG_NAME(month) \
( \
  (month) ==  1 ? "Jan" : \
  (month) ==  2 ? "Feb" : \
  (month) ==  3 ? "Mar" : \
  (month) ==  4 ? "Apr" : \
  (month) ==  5 ? "May" : \
  (month) ==  6 ? "Jun" : \
  (month) ==  7 ? "Jul" : \
  (month) ==  8 ? "Aug" : \
  (month) ==  9 ? "Sep" : \
  (month) == 10 ? "Oct" : \
  (month) == 11 ? "Nov" : \
  (month) == 12 ? "Dcl" : "" \
)
#endif  // MONTH_ENG_NAME

#if !defined(MONTH_NUMERIC)
#define MONTH_NUMERIC(pszMonthEngName) \
( \
  strcmp(pszMonthEngName, "Jan") == 0 ?  1 : \
  strcmp(pszMonthEngName, "Feb") == 0 ?  2 : \
  strcmp(pszMonthEngName, "Mar") == 0 ?  3 : \
  strcmp(pszMonthEngName, "Apr") == 0 ?  4 : \
  strcmp(pszMonthEngName, "May") == 0 ?  5 : \
  strcmp(pszMonthEngName, "Jun") == 0 ?  6 : \
  strcmp(pszMonthEngName, "Jul") == 0 ?  7 : \
  strcmp(pszMonthEngName, "Aug") == 0 ?  8 : \
  strcmp(pszMonthEngName, "Sep") == 0 ?  9 : \
  strcmp(pszMonthEngName, "Oct") == 0 ? 10 : \
  strcmp(pszMonthEngName, "Nov") == 0 ? 11 : \
  strcmp(pszMonthEngName, "Dcl") == 0 ? 12 : 0 \
)
#endif  // MONTH_NUMERIC

//------------------------------------------------------------------------------
// 功能描述：将给定格式的时间字符串析取出时间的各部分数值，并存入SYSTEMTIME结构中，
//           时间字符串格式说明字符定义如下：
//               CC : 世纪、YY : 年份、MM : 月份、DD : 日期
//               hh : 时、mm : 分、ss : 秒、nnn : 毫秒
// 参数说明：
//      [OUT] p_refstDateTime   时间字符串析取后的数据输出存储结构
//      [IN]  p_pszDatetime     时间字符串
//      [IN]  p_pszFormat       时间字符串的格式描述
// 返回说明：
//      无                      使用DATETIME_IS_VALID(p_refstODatetime)判断析取是否成功
void StringToDatetime(
  SYSTEMTIME &p_refstDatetime,
  const char *p_pszDatetime,
  const char *p_pszFormat
);

//------------------------------------------------------------------------------
// 功能描述：将存储于SYSTEMTIME结构中的时间数据，按给定的格式描述输出时间字符串
//           时间字符串格式说明字符定义同StringToDatetime函数中的描述。
// 参数说明：
//      [OUT] p_pszDatetime     输出数据的空间地址
//      [IN]  p_lBufferSize     p_pszDatetime的空间大小
//      [IN]  p_pszFormat       输出时间字符串的格式说明
//      [IN]  p_refDateTime     存储时间数据的SYSTEMTIME结构
// 返回说明：
//      p_pszDatetime
char * DatetimeToString(
  char *p_pszDatetime,
  long p_lBufferSize,
  const char *p_pszFormat,
  const SYSTEMTIME &p_refDateTime
);

//------------------------------------------------------------------------------
// 功能描述：根据指定日期时间的部位进行加减运算
// 参数说明：
//      [OUT] p_refstODatetime  运算后的数据输出存储结构
//      [IN]  p_chDtPart        指示进行运算的日期时间的部位
//                                DT_YEAR         : 年
//                                DT_MONTH        : 月
//                                DT_DAY          : 日
//                                DT_HOUR         : 时
//                                DT_MINUTE       : 分
//                                DT_SECOND       : 秒
//                                DT_MILLISECONDS : 毫秒
//      [IN]  p_llInterval      加减运算变化量
//      [IN]  p_refstIDatetime  待运算的日期时间数据存储结构
// 返回说明：
//      无                      使用DATETIME_IS_VALID(p_refstODatetime)判断运算是否成功
void DatetimeAdd(
  SYSTEMTIME &p_refstODatetime,
  char p_chDtPart,
  long long p_llInterval,
  const SYSTEMTIME &p_refstIDatetime
);

//------------------------------------------------------------------------------
// 功能描述：根据指定日期时间的部位计算两个日期时间的差值
// 参数说明：
//      [IN]  p_refstBgnDatetime开始时间
//      [IN]  p_refstEndDatetime结束时间
//      [IN]  p_chDtPart        指示进行运算的日期时间的部位
//                                DT_YEAR         : 年
//                                DT_MONTH        : 月
//                                DT_DAY          : 日
//                                DT_HOUR         : 时
//                                DT_MINUTE       : 分
//                                DT_SECOND       : 秒
//                                DT_MILLISECONDS : 毫秒
// 返回说明：
//      计算结果，如果计算失败，将返回LLONG_MAX(=0x7fffffffffffffff(9223372036854775807LL))
long long DatetimeDiff(
  const SYSTEMTIME &p_refstBgnDatetime,
  const SYSTEMTIME &p_refstEndDatetime, 
  char p_chDtPart = DT_DAY
);

int CalcDayDiff(int p_iBgnDate, int p_iEndDate);

#ifdef __cplusplus
}
#endif

#endif  // __XSDK_DATETIME_H__
