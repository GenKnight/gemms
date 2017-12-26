import QtQuick 2.0
import QuickFlux 1.1

import "../actions"
import "../Component"

import qiwookong 1.0

Store {

    QObjectList {
        id: stockQuoteItemList
    }

    function openOrCreateStockQuoteView(stock) {
        var bfind = false;
        var stockQuoteView = null;

        stockQuoteItemList.foreach(function(item){
            if(stock === item.stock) {
                stockQuoteView = item;
                bfind = true;
                return true;
            }
            return false;
        });

        if(!bfind || null === stockQuoteView) {
            var component = Qt.createComponent("qrc:/Resources/views/MainQuote/StockQuoteView.qml")
            if(Component.Ready === component.status) {
                stockQuoteView = component.createObject(null, {"stock": stock});
                stockQuoteItemList.push(stockQuoteView);
            } else {
                console.log(component.errorString())
            }
        }
        return stockQuoteView;
    }

}
