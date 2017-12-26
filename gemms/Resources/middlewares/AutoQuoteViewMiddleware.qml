import QtQuick 2.0
import QuickFlux 1.1
import QtQuick.Dialogs 1.2
import "../actions"
import "../stores"
import "../views/AutoQuote"

Middleware {

    property RootStore store: MainStore
    property AutoQuoteView autoQuoteView: null

    Component {
        id: component
        AutoQuoteView {}
    }

    function openAutoQuoteView() {
        if(null === autoQuoteView) {
            autoQuoteView = component.createObject(null, {});
        }

        autoQuoteView.show();
        autoQuoteView.raise();
    }

    function dispatch(type, message) {
        if(ActionTypes.openAutoQuoteView === type) {
            openAutoQuoteView();
            return;
        }
        next(type, message);
    }
}
