import QtQuick 2.0
import QuickFlux 1.1

import "../actions"
import "../Component"

import qiwookong 1.0

Store {

    QObjectList {
        id: autoQuoteObjectList
    }

//    Component {
//        id: component
//        AutoQuoteObject { }
//    }

//    function getOrCreateAutoQuoteObject(row, stock) {
//        var bfind = false;
//        var autoQuoteObject = null;
//        autoQuoteObjectList.foreach(function(item) {
//            if(stock === item.stock) {
//                autoQuoteObject = item;
//                return true;
//            }
//            return false;
//        });
//        if(!bfind || null === autoQuoteObject) {
//            autoQuoteObject = component.createObject(null, {"row": row, "stock": stock});
//            autoQuoteObject.stock = stock;
//            autoQuoteObjectList.push(autoQuoteObject);
//        }
//        return autoQuoteObject;
//    }
}
