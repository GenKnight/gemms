
var db

function initDatabase() {
    console.debug('begin initDatabase')
    db = LocalStorage.openDatabaseSync("GemmsDB", "1.0", "MarketMakerPane remembers its position", 100000);
    db.transaction(function(tx) {
        tx.executeSql('CREATE TABLE IF NOT EXISTS data(name TEXT, value TEXT, appId INT, state TEXT)')
    });
}

function storeData() {
    if(!db) {
        return;
    }
    root.state = "0"

    // 检查是否需要记录
    var bInit = false
    for(var i = 0; i < 8; ++i) {
        if(root.dataArray[i].stock !== "") {
            bInit = true;
            break;
        }
    }

    db.transaction(function(tx) {
        // 如果是载入的并且是初始状态，则从记录中删除
        if(!bInit && !root.isNew) {
            tx.executeSql('delete from data where name="GemmsData" and appId=?',[root.appId])
        } else {
            if(!root.isNew) {
                print('... gemms exists, update it')
                tx.executeSql('UPDATE data set value=?, state=? where name="GemmsData" and appId=?',
                                       [JSON.stringify(dataArray),root.state, root.appId])
            } else {
                // TODO insert
                print('... gemms does not exists, create it, appId: ' + root.appId + ", state:" + root.state)
                tx.executeSql('REPLACE INTO data VALUES (?,?,?,?)',
                              ['GemmsData', JSON.stringify(dataArray), root.appId, root.state]);
            }
        }
    });

    console.debug("... store gemms data success!")
}

function readData() {
    if(!db) {
        return;
    }

    db.transaction(function(tx){
        print('... read gemms object')
        var result = tx.executeSql('select * from data where name="GemmsData" and state == "0" order by appId asc');
        console.debug("read totle count: " + result.rows.length)

        // 获取最小不使用得面板
        if(result.rows.length > 0) {
            root.isNew = false
            root.appId =  result.rows.item(0).appId
            root.state = "1"
            root.dataArray = JSON.parse(result.rows.item(0).value)

            // 更新当前appId为使用中
            db.transaction(function(tx) {
                tx.executeSql('UPDATE data set state=? where name="GemmsData" and appId=?', [root.state, root.appId])
            })

        } else {
            root.isNew = true
            for(var i = 0; i < 8; ++i) {
                var data = {
                    stock: ""
                }
                root.dataArray.push(data);
            }

            // 获取当前最大序号
           db.transaction(function(tx) {
               var result = tx.executeSql('select count(*) as total from data where name="GemmsData"')
               if(result.rows.length > 0) {
                   // 数据库当前最大的id和当前打开最大的id取最大的
                   var dbMax = Math.max(Number(result.rows.item(0).total) + 1, root.appId);
                   root.appId = dbMax;
               }
           })
        }
        console.debug("init appId:" + root.appId + ", state:" + root.state + ", dataArray" + JSON.stringify(root.dataArray));
    });
}

function cacheHandle(index, stock) {
    if(index < 0 || index >= 8) {
        return
    }

    console.debug("add " + index + " cache")
    var obj = root.dataArray[index]
    obj.stock = stock
    console.debug(JSON.stringify(root.dataArray))
}

function removeHandle(index) {
    if(index < 0 || index >= 8) {
        return
    }

    console.debug("remove " + index + " cache")
    var obj = root.dataArray[index]
    obj.stock = ""
    console.debug(JSON.stringify(root.dataArray))
}
