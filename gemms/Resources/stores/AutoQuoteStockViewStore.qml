import QtQuick 2.0
import QuickFlux 1.1

import "../actions"
import "../Component"
import "../views/AutoQuote"

import qiwookong 1.0

Store {

    QObjectList {
        id: autoQuoteViewList
    }

    Component {
        id: component
        AutoQuoteStockView {}
    }

    function getOrCreateAutoQuoteView(row, stock) {
        var bfind = false;
        var autoQuoteView = null;

        autoQuoteViewList.foreach(function(item){
            if(stock === item.stock) {
                autoQuoteView = item;
                bfind = true;
                return true;
            }
            return false;
        });

        if(!bfind || null === autoQuoteView) {
            autoQuoteView = component.createObject(null, {"row": row, "stock": stock});
            autoQuoteViewList.push(autoQuoteView);
        }
        autoQuoteView.row = row;
        return autoQuoteView;
    }

}
