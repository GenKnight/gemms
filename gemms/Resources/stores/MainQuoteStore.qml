import QtQuick 2.0
import QuickFlux 1.1

import "../actions"
import "../Component"

import qiwookong 1.0

Store {

    property int mainQuoteId: 0

    function getCurrQuoteId() {
        mainQuoteId += 1;
        return mainQuoteId
    }

    QObjectList {
        id: mainQuoteList
    }

    function openOrCreateMainQuoteView() {
        var bfindOrCreate = false;
        var mainQuoteView = null;

        mainQuoteList.foreach(function(item){
            if(!item.visible) {
                mainQuoteView = item;
                bfindOrCreate = true;
                return true;
            }
            return false;
        });

        if(!bfindOrCreate || null === mainQuoteView) {
            // 判断当前股票是否已经全部打开，如果全部打开则不创建新的窗口
            if(stockQuoteListStore.model.rowCount() > 0) {
                var component = Qt.createComponent("qrc:/Resources/views/MainQuoteView.qml")
                if(Component.Ready === component.status) {
                    mainQuoteView = component.createObject(null, {"visible": false});
                    mainQuoteList.push(mainQuoteView);
                    mainQuoteView.closing.connect(function(close){
                        close.accept = false;
                        mainQuoteView.visible = false;
                    });
                    bfindOrCreate = true;
                } else {
                    console.log(component.errorString())
                }
            } else {
                // 打开第一个窗口
                mainQuoteList.foreach(function(item){
                    if(item.visible) {
                        mainQuoteView = item;
                        return true;
                    }
                    return false;
                });
            }
        }

        mainQuoteView.visible = true;
        mainQuoteView.raise();
        mainQuoteView.showMaximized();
        return bfindOrCreate;
    }

}
