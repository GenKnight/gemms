import QtQuick 2.0
import QuickFlux 1.1

import "../actions"
import "../Component"

import qiwookong 1.0

Store {

    QObjectList {
        id: stockObjectList
    }

    Component {
        id: stockComponent
        StockObject {}
    }

    function getOrCreateStockObject(stock) {
        var bfind = false;
        var stockObject = null;
        stockObjectList.foreach(function(item) {
            if(stock === item.stock) {
                stockObject = item;
                return true;
            }
            return false;
        });
        if(!bfind || null === stockObject) {
            stockObject = stockComponent.createObject(null, {"stock": stock});
            stockObject.stock = stock;
            stockObjectList.push(stockObject);
        }
        return stockObject;
    }
}
