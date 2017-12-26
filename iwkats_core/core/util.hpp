/********************************************************************
*  Created:    2017/10/12 14:42
*  File name:  util.hpp
*  Author:     harvey
*  Purpose:
*
*  Copyright 2010-2013, All Rights Reserved.
********************************************************************/
#pragma once

#include <QString>
#include <QByteArray>

#include <limits>
#include <ratio>
#include <cstdlib>
#include <chrono>
#include <functional>
#include <thread>
#include <sstream>
#include <regex>
#include <mutex>
#include <iterator>
#include <iomanip>
#include <ctime>
#include <future>
#include <unordered_map>

#include <cereal/archives/json.hpp>

#include "iwkats_core_global.h"

#include "logging.h"

using TimePoint = std::chrono::system_clock::time_point;

using std::chrono::milliseconds;
typedef unsigned int uint;

/*
* @return TimePoint	current time
*/
inline TimePoint currentTimePoint() {
  return  std::chrono::system_clock::now();
}

// operator overload to output TimePoint
inline std::ostream& operator<<(std::ostream &str, const TimePoint& timePoint) {
  std::ios::fmtflags f(str.flags());
  auto time = std::chrono::system_clock::to_time_t(timePoint);
  str << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "."
      << std::setfill('0') << std::setw(3) << std::chrono::time_point_cast<milliseconds>(timePoint).time_since_epoch().count() % 1000;
  str.flags(f);
  return str;
}

inline std::string currentTimePointString() {
  std::stringstream ss;
  ss << currentTimePoint();
  return ss.str();
}

inline std::string timePointString(const TimePoint& time) {
  std::stringstream ss;
  ss << time;
  return ss.str();
}

inline std::string operator + (const std::string& a, char* b) {
  return a + std::string(b);
}

inline std::string operator + (char* a, const std::string& b) {
  return std::string(a) + b;
}


/*
* @return TimePoint beginning of day
* e.g. for 2015-06-07 08:25:00.200, the function returns 2015-06-07 00:00:00.000
*/
inline TimePoint dayBegin(const TimePoint& timePoint = currentTimePoint()) {
  // convert TimePoint to time_t and then to tm
  time_t time_struct = std::chrono::system_clock::to_time_t(timePoint);
  tm time_info = *localtime(&time_struct);
  // zero hour, minute, second
  // (millisecond is automatically truncated since there is no millisecond field in tm)
  time_info.tm_hour = time_info.tm_min = time_info.tm_sec = 0;
  time_struct = mktime(&time_info);
  if(time_struct == -1)
    throw std::bad_cast();
  // convert it back to TimePoint
  return std::chrono::system_clock::from_time_t(time_struct);
}

/*
* @return duration between beginning of day and timePoint
* e.g. for 2015-06-07 08:25:00.200, the function returns 8*3,600,000 + 25*60,000 + 200 milliseconds
*/
inline milliseconds sinceBeginOfDay(const TimePoint& timePoint = currentTimePoint()) {
  // convert TimePoint to time_t and then to tm
  time_t time_struct = std::chrono::system_clock::to_time_t(timePoint);
  tm time_info = *localtime(&time_struct);
  // zero hour, minute, second
  // (millisecond is automatically truncated since there is no millisecond field in tm)
  time_info.tm_hour = time_info.tm_min = time_info.tm_sec = 0;
  time_struct = mktime(&time_info);
  if(time_struct == -1)
    throw std::bad_cast();
  // compute difference
  return std::chrono::duration_cast<milliseconds>(timePoint - std::chrono::system_clock::from_time_t(time_struct));
}

inline TimePoint stringToTimePointMs(const std::string& dateStr, const std::string& timeStr, const std::string& msStr = "0") {
  tm t;
  std::istringstream ss(dateStr.substr(0, 4) + '-' + dateStr.substr(4, 2) + '-' + dateStr.substr(6, 2) + " " + timeStr);
  ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
  t.tm_isdst = 0; // not daylight saving time
  auto tmp = std::chrono::system_clock::from_time_t(mktime(&t));
  return  tmp + milliseconds(std::stoi(msStr));
}

/*
* Convert a std::string to int64_t (long long)
*/
inline int64_t stoi64(const std::string &str) {
  return std::strtoull(str.c_str(), nullptr, 10);
}

/*
* Free a two-dimensional array of char
*/
inline void freeCharPtrArray(char** arrayOfCharPtr, int length) {
  for(auto i = 0; i < length; ++i) {
    delete arrayOfCharPtr[i];
  }
  delete arrayOfCharPtr;
}

/*
* Convert a comma-separated-valued std::string to a vector of std::string
* e.g. "AAPL,105.2,1e6" => vector<std::string> {"AAPL", "105.2", "1e6"}
*/
inline std::vector<std::string> csvstrToVector(const std::string& csvstr) {
  std::stringstream   lineStream(csvstr);
  std::string         cell;

  std::vector<std::string> mData;
  while(getline(lineStream, cell, ',')) {
    mData.push_back(cell);
  }

  return mData;
}


inline std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty = true) {
  std::vector<std::string> result;
  if(delim.empty()) {
    result.push_back(s);
    return result;
  }
  std::string::const_iterator substart = s.begin(), subend;
  while(true) {
    subend = search(substart, s.end(), delim.begin(), delim.end());
    std::string temp(substart, subend);
    if(keep_empty || !temp.empty()) {
      result.push_back(temp);
    }
    if(subend == s.end()) {
      break;
    }
    substart = subend + delim.size();
  }
  return result;
}


class KeyErrorException : public std::logic_error {
public:
  KeyErrorException(const std::string& msg) : logic_error(msg) {}
};

/*
* A thin wrapper for std::unordered_map which provides
* 1) explicit exception when the key does not exist
* 2) easy way to initialize a key-value pair if the key previously did not exist
* 3) helper function to tell if a key exists
* 4) helper function to remove a key-value by key
*/
template<typename K, typename V>
class EasyMap : public std::unordered_map<K, V> {
public:
  bool contains(const K& key) {
    auto it = this->find(key);
    return it != this->cend();
  }

  V& get(const K& key) {
    auto it = this->find(key);
    if(it == this->cend()) {
      throw KeyErrorException("EasyMap Key not found");
    }
    return it->second;
  }

  /*
    * Note: V should have a default constructor!
    */
  V& getOrSet(const K& key) {
    if(!contains(key))
      this->emplace(key, V());
    auto it = this->find(key);
    return it->second;
  }

  bool remove(const K& key) {
    auto it = this->find(key);
    if(it == this->end()) {
      return false;
    }
    this->erase(it);
    return true;
  }
};

/*
* A thin wrapper for std::unordered_map which provides
* 1) explicit exception when the key does not exist
* 2) easy way to initialize a key-value pair if the key previously did not exist
* 3) helper function to tell if a key exists
* 4) helper function to remove a key-value by key
*/
template<typename K, typename V>
class OrderedEasyMap : public std::map<K, V> {
public:
  bool contains(const K& key) {
    auto it = this->find(key);
    return it != this->cend();
  }

  V& get(const K& key) {
    auto it = this->find(key);
    if(it == this->cend()) {
      throw KeyErrorException("Key not found");
    }
    return it->second;
  }

  /*
    * Note: V should have a default constructor!
    */
  V& getOrSet(const K& key) {
    if(!contains(key))
      this->emplace(key, V());
    auto it = this->find(key);
    return it->second;
  }

  bool remove(const K& key) {
    auto it = this->find(key);
    if(it == this->end()) {
      return false;
    }
    this->erase(it);
    return true;
  }
};

class DataErrorException : public std::logic_error {
public:
  DataErrorException(const std::string& msg) : logic_error(msg) {}
};

/*
* Data structure storing a series of data, designed to
* 1) indexed (regular and reversed) access to stored data
* 2) explicit exception for out-of-range value access
* 3) close integration with functions defined in func.hpp
*/
template<typename T>
class DataSeries {
protected:
  std::vector<T> data;
public:
  DataSeries() {}

  virtual ~DataSeries() {}
  void append(const T&d) {
    data.push_back(d);
  }

  void emplace(const T& d) {
    data.emplace_back(d);
  }

  template<typename P>
  void append(const P& raw, std::function<T(const P&)> transform) {
    data.push_back(transform(raw));
  }

  /*
    * reversed access
    * @param index: 0 for the last value, 1 for the second last value, ...
    */
  T const& operator()(std::size_t index) const {
    if(index < 0 || index >= data.size())
      throw DataErrorException("Reverse index to the DataSeries should between 0 and " + std::to_string(data.size()));
    return data[data.size() - index - 1];
  }

  /*
    * regular access
    * @param index: just like the regular vector index
    */
  T const& operator[](std::size_t index) const {
    if(index < 0 || index >= data.size())
      throw DataErrorException("Index to the DataSeries should between 0 and " + std::to_string(data.size()));
    return data[index];
  }

  size_t size() const {
    return data.size();
  }

  void clear() {
    data.clear();
  }

  template <class Archive>
  void serialize(Archive & archive) {
    archive(data);
  }
};

template <typename T>
class SharedVecPtr {
public:
  SharedVecPtr() {
    vec_ = std::make_shared<std::vector<std::shared_ptr<T>>>();

  }

  void reset(T* ptr, int n) {
    for(auto i = 0; i < n; ++i) {
      auto data = std::make_shared<T>();
      memcpy(data.get(), &ptr[i], sizeof(T));
      vec_->push_back(data);
    }
  }

  std::shared_ptr<std::vector<std::shared_ptr<T>>> data() const {
    return vec_;
  }

private:
  std::shared_ptr<std::vector<std::shared_ptr<T>>> vec_;
};

inline void waitTill(std::function<bool(void)> eval, milliseconds wait = std::chrono::milliseconds(100)) {
  while(true) {
    if(eval())
      break;
    std::this_thread::sleep_for(wait);
  }
}

// 同步阻塞等待某个操作一段时间, 此处第一个参数不能是局部参数
template <typename T>
inline bool waitFor(std::future<T>& f,
					std::function<bool(void)> eval,
					std::chrono::seconds wait = std::chrono::seconds(5),
					milliseconds check = std::chrono::milliseconds(100)) {

  try {
    f = std::async(std::launch::async, [&]() {
      while(true) {
        if(eval()) {
          break;
        }
        std::this_thread::sleep_for(check);
      }
    });
    if(f.wait_for(wait) == std::future_status::ready) {
      return true;
    }
  } catch(std::exception& e) {
    LOG(ERROR) << "computer not support multi-core";
  }
  return false;
}

template <typename T>
inline bool DeepCopy(T** dst, T* src) {
  if(src) {
    *dst = new T();
    memset(*dst, 0, sizeof(T));
    memcpy(*dst, src, sizeof(T));
    return true;
  }
  return false;
}

template<typename T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) {
  return N;
}

// 返回应用类型 c++11
template <typename Contrainer, typename Index>
auto authAndAccess(Contrainer& c, Index i) -> decltype(c[i]) {
  return c[i];
}

// 将有作用域 enum 转化成对应的潜在类型 c++11
template <typename E>
constexpr typename std::underlying_type<E>::type toUType(E enumerator) noexcept {
  return static_cast<typename std::underlying_type::type>(enumerator);
}

// 将有作用域 enum 转化成对应的潜在类型 c++14
template <typename E>
constexpr std::underlying_type_t<E> toUType14(E enumerator) noexcept {
  return static_cast<std::underlying_type_t<E>>(enumerator);
}

struct IWKATS_CORE_EXPORT core_util {
  static QByteArray toUtf8(const char* src) {
    return QString::fromLocal8Bit(src).toUtf8();
  }

  static int CurrentDate() {
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm stm;
    localtime_s(&stm, &tt);

    return stm.tm_year * 10000 + stm.tm_mon * 100 + stm.tm_mday;
  }

  static bool IsCtpTradingTime() {
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm stm;
    localtime_s(&stm, &tt);

    // 周六周日
    if(0 == stm.tm_wday || 6 == stm.tm_wday) {
      return false;
    }
    bool ret = false;
    switch(stm.tm_hour) {
      case 9:
      {
        ret = true;
        break;
      }
      case 10:
        if(stm.tm_min <= 15 || stm.tm_min >= 30) {
          ret = true;
        }
        break;
      case 11:
      {
        if(stm.tm_min <= 30) {
          ret = true;
        }
        break;
      }
      case 13:
      case 14:
      {
        ret = true;
        break;
      }
      case 15:
      {
        if(stm.tm_min <= 59) {
          ret = true;
        }
        break;
      }
      case 21:
      case 22:
      case 23:
      case 24:
      case 0:
      case 1:
      {
        ret = true;
        break;
      }
      case 2:
      {
        if(stm.tm_min <= 30) {
          ret = true;
        }
        break;
      }
      default:
      {
        ret = false;
        break;
      }
    }
    return ret;
  }

};

template <typename T>
T checkNumericLimits(const T& value) {
  if((std::numeric_limits<T>::max)() == value || (std::numeric_limits<T>::min)() == value) {
    return T{0};
  }
  return value;
}

template <typename T>
bool toJson(T& value, std::string& result) {
  try {
    std::ostringstream oss;
    cereal::JSONOutputArchive archive(oss);
    archive(CEREAL_NVP(value));
    result = oss.str();
    result += "}";
  } catch(std::exception& e) {
    LOG_ERROR << e.what();
    return false;
  }
  return true;
}

template <typename T>
bool fromJson(T& value, const std::string& json) {
  try{
    std::istringstream iss(json);
    cereal::JSONInputArchive archive(iss);
    archive(CEREAL_NVP(value));
  }catch(std::exception& e) {
    LOG_ERROR << e.what();
    return false;
  }
  return true;
}
