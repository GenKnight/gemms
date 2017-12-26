
import QtQuick 2.0

QtObject {
    id: qObject

    default property alias data: qObject.__data

    property list<QtObject> __data

    function insert(index, item) {
       __data = __qmlListPropertyInsert(__data, index, item);
    }

    function remove(index) {
        var removeItem = __data[index];
        __qmlListPropertyRemove(__data, index);
        return removeItem;
    }

    function push(item) {
        __data.push(item)
    }

    /* 获取指定 key 的元素 */
    function foreach(_pred) {
        for(var iter in __data) {
            if(_pred(__data[iter])) {
                break;
            }
        }
    }

    /*! \internal */
    function __qmlListPropertyInsert(qmllistproperty, index, item) {
        if(index > qmllistproperty.length) return qmllistproperty;
        var  qmllistproperty_ = [];
        var length = qmllistproperty.length + 1;

        var flag = false;           // insert ?
        for(var iter = 0; iter < length; ++iter) {
            if(iter === index) {
                 qmllistproperty_.push(item);
                flag = true;
            } else {
                if(flag) {
                     qmllistproperty_.push(qmllistproperty[iter-1]);
                } else {
                     qmllistproperty_.push(qmllistproperty[iter]);
                }
            }
        }
        return qmllistproperty_;
    }

    /*! \internal */
    function __qmlListPropertyRemove(qmllistproperty, index) {
        if(index > qmllistproperty.length-1) return qmllistproperty;
        var  qmllistproperty_ = [];
        for(var iter in qmllistproperty) {
            if(iter === index) {
                continue;
            } else {
                 qmllistproperty_.push(qmllistproperty[iter]);
            }
        }
        return qmllistproperty_;
    }
}
