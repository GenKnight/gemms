#include "asset_share_model.h"

#include "ksmm/ksmm_trader.h"

#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN
// 查询持仓
AssetShareModel::AssetShareModel(QObject* parent)
    :QAbstractListModel(parent)
{
    connect(Trader::Spi(), &KSMMTradingSpi::sigQueryShareArrive, this, &AssetShareModel::onAssetShareArrive);
}

int AssetShareModel::rowCount(const QModelIndex& parent) const
{
    return asset_shares_.size();
}

QVariant AssetShareModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    auto row = index.row();
    if(row < 0 || row >= rowCount()) {
        return QVariant();
    }

    auto p_asset_share = asset_shares_[row];
    switch(role) {
    case KnProjectId     : return p_asset_share->nProjectId   ;
    case KszProjectName  : return core_util::toUtf8(p_asset_share->szProjectName);
    case KnAssetId       : return p_asset_share->nAssetId     ;
    case KszAssetName    : return core_util::toUtf8(p_asset_share->szAssetName);
    case KnCombiId       : return p_asset_share->nCombiId     ;
    case KchStkex        : return GetStkExMean(p_asset_share->chStkex);
    case KszStkbd        : return GetSzStkBdMean(p_asset_share->szStkbd);
    case KszStkCode      : return p_asset_share->szStkCode    ;
    case KszStkName      : return core_util::toUtf8(p_asset_share->szStkName);
    case KchStkCls       : return GetStkClsMean(p_asset_share->chStkCls);
    case KllStkPrebln    : return p_asset_share->llStkPrebln  ;
    case KllStkBln       : return p_asset_share->llStkBln     ;
    case KllStkAvl       : return p_asset_share->llStkAvl     ;
    case KdStkBcostRlt   : return QString::number(p_asset_share->dStkBcostRlt, 'f', 0);
    case KdStkIncome     : return p_asset_share->dStkIncome   ;
    case KdStkIncomePre  : return p_asset_share->dStkIncomePre;
    case KdStkFpl        : return QString::number(p_asset_share->dStkFpl, 'f', 0);
    case KdMktVal        : return QString::number(p_asset_share->dMktVal, 'f', 0);
    default:
        return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> AssetShareModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[KnProjectId]     = core_util::toUtf8("项目ID");
    roles[KszProjectName]  = core_util::toUtf8("项目名称");
    roles[KnAssetId]       = core_util::toUtf8("资产单元ID");
    roles[KszAssetName]    = core_util::toUtf8("资产单元名称");
    roles[KnCombiId]       = core_util::toUtf8("组合ID");
    roles[KchStkex]        = core_util::toUtf8("证券交易所");
    roles[KszStkbd]        = core_util::toUtf8("交易板块");
    roles[KszStkCode]      = core_util::toUtf8("证券代码");
    roles[KszStkName]      = core_util::toUtf8("证券名称");
    roles[KchStkCls]       = core_util::toUtf8("证券类别");
    roles[KllStkPrebln]    = core_util::toUtf8("证券昨日余额");
    roles[KllStkBln]       = core_util::toUtf8("证券余额");
    roles[KllStkAvl]       = core_util::toUtf8("证券可用数量");
    roles[KdStkBcostRlt]   = core_util::toUtf8("证券买入成本(实时)");
    roles[KdStkIncome]     = core_util::toUtf8("今日实际盈亏金额");
    roles[KdStkIncomePre]  = core_util::toUtf8("昨日实际盈亏金额");
    roles[KdStkFpl]        = core_util::toUtf8("今日浮动盈亏金额");
    roles[KdMktVal]        = core_util::toUtf8("市值");

    return roles;
}

QStringList AssetShareModel::customRoleNames()
{

    QStringList rolesList;
    rolesList << core_util::toUtf8("证券代码")
              << core_util::toUtf8("证券名称")
              << core_util::toUtf8("证券余额")
              << core_util::toUtf8("证券可用数量")
              << core_util::toUtf8("证券昨日余额")
              << core_util::toUtf8("证券买入成本(实时)")
              << core_util::toUtf8("市值")
              << core_util::toUtf8("今日实际盈亏金额")
              << core_util::toUtf8("昨日实际盈亏金额")
              << core_util::toUtf8("今日浮动盈亏金额")
              << core_util::toUtf8("项目ID")
              << core_util::toUtf8("项目名称")
              << core_util::toUtf8("资产单元ID")
              << core_util::toUtf8("资产单元名称")
              << core_util::toUtf8("组合ID")
              << core_util::toUtf8("证券交易所")
              << core_util::toUtf8("交易板块")
              << core_util::toUtf8("证券类别");

    return rolesList;
}

void AssetShareModel::refresh(const QString& stock)
{
    beginResetModel();
    asset_shares_.clear();
    valid_checks_.clear();
    endResetModel();

    stock_ = stock;

    if(stock_.isEmpty()) {
        std::vector<ASSET_CODE*> asset_codes;
        Trader::Spi()->GetAssetCodes(asset_codes);
        for(auto& asset_code : asset_codes) {
            Trader::Api()->queryAssetShare(asset_code->szStkCode);
        }
    } else {
        Trader::Api()->queryAssetShare(stock_);
    }
}

void AssetShareModel::onAssetShareArrive(SharedVecPtr<ASSET_SHARE> pAssetShare)
{
    auto it = pAssetShare.data()->begin();
    for(; it != pAssetShare.data()->end(); ++it) {
        if(stock_.isEmpty() || 0 == strcmp(stock_.toStdString().data(), (*it)->szStkCode)) {

            // 如果当前没有此股票的持仓，则插入，否则视为重复
            const auto& item = (*it);

            if(valid_checks_.contains(item->szStkCode)) {
                // 当前model中已经有该委托，更新
                auto update_row = valid_checks_.value(item->szStkCode);
                asset_shares_[update_row] = item;
                emit dataChanged(index(update_row), index(update_row));
            } else {
                // 插入
                auto insert_row = rowCount();
                beginInsertRows(QModelIndex(), insert_row, insert_row);
                valid_checks_.insert(item->szStkCode, insert_row);
                asset_shares_.push_back(item);
                endInsertRows();
            }
        }
    }
}

IWKATS_NAMESPACE_END
