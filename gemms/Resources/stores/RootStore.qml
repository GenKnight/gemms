import QtQuick 2.0
import QuickFlux 1.1

import qiwookong 1.0

Store {

    // It hold the stockQuote list data model
    property alias stockQuoteListStore: stockQuoteListStore
    StockQuoteListStore {
        id: stockQuoteListStore
    }


    property alias mainQuoteStore: mainQuoteStore
    MainQuoteStore {
        id: mainQuoteStore
    }

    // 做市面板
    property alias stockQuoteViewStore: stockQuoteViewStore
    StockQuoteViewStore {
        id: stockQuoteViewStore
    }

    // 做市行情 model
    property alias zsMarketBook: zsMarketBook
    MarketBookStore {
        id: zsMarketBook
        type: MarketBookModel.ZSMarketType
    }

    // 投资行情 model
    property alias tzMarketBook: tzMarketBook
    MarketBookStore {
        id: tzMarketBook
        type: MarketBookModel.TZMarketType
    }

    // 股票信息
    property alias stockObjectStore: stockObjectStore
    StockObjectStore {
        id: stockObjectStore
    }

    property alias autoQuoteStockListStore: autoQuoteStockListStore
    AutoQuoteStockListStore {
        id: autoQuoteStockListStore
    }

    // 自动重复报价
    property alias autoQuoteObjectStore: autoQuoteObjectStore
    AutoQuoteObjectStore {
        id: autoQuoteObjectStore
    }

    // 自动重复股票报价 view store
    property alias autoQuoteStockViewStore: autoQuoteStockViewStore
    AutoQuoteStockViewStore {
        id: autoQuoteStockViewStore
    }
}
