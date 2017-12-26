#pragma once

#include "core/iwkats_core.h"

#include "KSMMApi/Define.h"

#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

#define MARKET_BOOK_MAX_SIZE 21

struct Tick {
  double price;
  int vol;
};

class MarketBookModel : public QAbstractListModel {
  Q_OBJECT
  Q_ENUMS(MarketType)
  Q_PROPERTY(MarketType type READ type WRITE setType NOTIFY typeChanged)
  Q_PROPERTY(QString stock READ stock WRITE setStock)
public:
  enum DataType {
    Price = Qt::UserRole + 1,
    Volume
  };

  enum MarketType {
    UnknowType,
    ZSMarketType,      // 投资
    TZMarketType,      // 做市
  };

  MarketBookModel(QObject* parent = nullptr);
  void setType(MarketType type) { type_ = type; }
  MarketType type() const { return type_;}

  QString stock() const { return code_; }
  void setStock(const QString& code);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void resetModel();

public slots:
  void slotNqTickArrive(std::shared_ptr<STK_NQ_TICK> pNQHQInfo);
  void slotZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo);

signals:
  void typeChanged();

private:
  void clearAllTickBook(std::array<Tick, MARKET_BOOK_MAX_SIZE>& book_tick);

private:
  struct Private;
  std::shared_ptr<Private> d_;

  QString code_{""};
  MarketType type_{UnknowType};
};

IWKATS_NAMESPACE_END
