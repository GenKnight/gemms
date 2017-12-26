#include "lock.h"

IWKATS_NAMESPACE_BEGIN

RLockGuard::RLockGuard(QReadWriteLock& lock) : lock_(lock) {
  lock_.lockForRead();
}

RLockGuard::~RLockGuard() {
  lock_.unlock();
}

WLockGuard::WLockGuard(QReadWriteLock& lock) : lock_(lock) {
  lock_.lockForWrite();
}

WLockGuard::~WLockGuard() {
  lock_.unlock();
}

FileGuard::FileGuard(QFile & file)
  :file_(file) {

}

FileGuard::~FileGuard() {
  file_.close();
}

IWKATS_NAMESPACE_END

