import QtQuick 2.7
import QtQml 2.2

Item {
    signal timerStop();
    signal timerTriggered(string  time);

    property int hour: 0
    property int minute: 0
    property int second: 0

    Timer {
        id: timer
        interval: 1000
        repeat: true
        running: false

        onTriggered: {
            second += 1;
            if(0 === second % 60) {
                second %= 60;
                minute += 1;

                if(0 === minute % 60) {
                    hour += 1;
                    minute %= 60;

                    if(0 === hour % 24) {
                        reset();
                    }
                }
            }

            var showSecond = second;
            if(second < 10) {
                showSecond = "0" + second;
            }

            var showMinute = minute;
            if(minute < 10) {
                showMinute = "0" + minute;
            }

            var showHour = hour;
            if(hour < 10) {
                showHour = "0" + hour;
            }

            var time = qsTr(showHour + ":" + showMinute + ":" + showSecond);
            timerTriggered(time);
        }
    }

    function reset() {
        hour = 0;
        minute = 0;
        second = 0;
    }

    function startTimer() {
        if(!timer.running) {
            reset();
            timer.start();
        }
    }

    function stopTimer() {
        if(timer.running) {
            reset();
            timer.stop();
            timerStop();
        }
    }
}


