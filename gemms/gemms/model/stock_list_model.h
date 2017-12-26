
#include "core/iwkats_core.h"

#include "ksmm/common_define.hpp"

#include <QReadWriteLock>
#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

class StockListModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum DataType {
    KszStkCode = Qt::UserRole + 1,      ///< 证券代码
    KszStkName,                         ///< 证券名称
    KchStkEx,               ///< 证券交易所
    KszStkBd,               ///< 证券板块
    KchCodeBizAction,       ///< 股票类型
    KszCombiName,           ///< 组合名称
    KszAssetName,           ///< 资产单元名称
  };

  StockListModel(QObject* parent = nullptr);

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE StockObject* getStockObjectByRow(int row);
  Q_INVOKABLE StockObject* getStockObjectByStock(const QString& stock);

public:
  // qml 移除某行
  Q_INVOKABLE void remove(const QString& stock);

  // qml 增加某行
  Q_INVOKABLE void add(const QString& stock);

  Q_INVOKABLE QString getStockName(const QString& stock);

private:
 StockObjectPtr GetStockObjectByRow(int row) const;

private:
  QReadWriteLock rwlock_;
//  QHash<QString, ASSET_CODE*> model_stocks_;
  QHash<QString, StockObjectPtr> total_stocks_;
  QHash<QString, StockObjectPtr> model_stocks_;
};

IWKATS_NAMESPACE_END
