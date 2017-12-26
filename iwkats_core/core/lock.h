/********************************************************************
 *  Created:    2017/10/19 12:44
 *  File name:  lock.hpp
 *  Author:     harvey
 *  Purpose:
 *
 *  Copyright 2010-2013, All Rights Reserved.
 ********************************************************************/
#pragma once

#include "iwkats_core_global.h"

#include <QReadWriteLock>
#include <QFile>

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT RLockGuard {
public:
  RLockGuard(QReadWriteLock& lock);
  ~RLockGuard();

private:
  QReadWriteLock& lock_;
};

class IWKATS_CORE_EXPORT WLockGuard {
public:
  WLockGuard(QReadWriteLock& lock);
  ~WLockGuard();

private:
  QReadWriteLock& lock_;
};

class IWKATS_CORE_EXPORT FileGuard {
public:
  FileGuard(QFile& file);
  ~FileGuard();

private:
  QFile& file_;
};

IWKATS_NAMESPACE_END
