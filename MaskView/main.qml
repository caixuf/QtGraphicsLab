import QtQuick 2.15
import QtQuick.Window 2.15
Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    property double defaultBackgroudOpacity: 0.5
    property double defaultRectangleOpacity: 0.4
    property bool defaultBackgroudState1: false
    property bool defaultBackgroudState2: true
    property color defaultBackgroudColor1: "black"
    property color defaultBackgroudColor2: "black"

    Image {
        id: imageA1
        source: "qrc:/image/image/a1.png"
        anchors.fill: parent
    }
    //第一个矩形组合
    Item {
        id: firstArea
        anchors.fill: parent
        property bool viewState: false
        Rectangle{
            id:firstAreaRect1
            x: parent.width / 2
            y: parent.height / 2
            width: parent.width/ 3
            height: parent.height / 3
            property int dragX: 0
            property int dragY: 0
            property bool dragging: false
            property bool isHang: false
            //color: "yellow"
            opacity: root.defaultRectangleOpacity
            color: "white"
            //给四角设置矩形
            onWidthChanged: {
                canvas.requestPaint();
                canvas.startX = secondAreaRect1.x + secondAreaRect1.width / 2;
                canvas.startY = secondAreaRect1.y + secondAreaRect1.height / 2;
                canvas.endX = firstAreaRect1.x + firstAreaRect1.width / 2;
                canvas.endY = firstAreaRect1.y + firstAreaRect1.height / 2;
                canvas.requestPaint();

            }
            onHeightChanged: {
                canvas.requestPaint();
                canvas.startX = secondAreaRect1.x + secondAreaRect1.width / 2;
                canvas.startY = secondAreaRect1.y + secondAreaRect1.height / 2;
                canvas.endX = firstAreaRect1.x + firstAreaRect1.width / 2;
                canvas.endY = firstAreaRect1.y + firstAreaRect1.height / 2;
                canvas.requestPaint();
            }

            MouseArea
            {
                anchors.fill: parent
                onPressed: {
                    parent.dragX = mouseX;
                    parent.dragY = mouseY;
                    parent.dragging = true;
                }
                onReleased: {
                    vLine.visible = false;
                    hLine.visible = false;
                    parent.dragging = false;
                }
                onPositionChanged: {
                    if(parent.dragging) {
                        var newX = parent.x + mouseX - parent.dragX;
                        var newY = parent.y + mouseY - parent.dragY;
                        if(newX >= 0 && newX <= root.width - parent.width && newY >= 0 && newY <= root.height - parent.height){
                            parent.x = newX;
                            parent.y = newY;
                            //移动谁就用谁的背景
                            root.defaultBackgroudState1 = true;
                            root.defaultBackgroudState2 = false
                            //移动谁就指向谁
                            canvas.startX = secondAreaRect1.x + secondAreaRect1.width / 2;
                            canvas.startY = secondAreaRect1.y + secondAreaRect1.height / 2;
                            canvas.endX = firstAreaRect1.x + firstAreaRect1.width / 2;
                            canvas.endY = firstAreaRect1.y + firstAreaRect1.height / 2;
                            canvas.requestPaint();
                        }

                        if(firstArea.viewState === false) return;
                        var centerX = parent.x + parent.width / 2;
                        var centerY = parent.y + parent.height / 2;

                        if(parent.x > root.width / 2 && parent.x - root.width / 2 <= 30) {
                            vLine.visible = true;
                        }
                        else if(parent.x + parent.width < root.width / 2 && root.width / 2 - parent.x - parent.width <= 30) {
                            vLine.visible = true;
                        }
                        else if(Math.abs(centerX - root.width / 2) <= 30){
                            vLine.visible = true;
                        }
                        else {
                            vLine.visible = false;
                        }
                        if(parent.y > root.height / 2 && parent.y - root.height / 2 <= 30) {
                            hLine.visible = true;
                        }
                        else if(parent.y + parent.height < root.height / 2 && root.height / 2 - parent.y - parent.height <= 30) {
                            hLine.visible = true;
                        }
                        else if(Math.abs(centerY - root.height / 2) <= 30) {
                            hLine.visible = true;
                        }
                        else {
                            hLine.visible = false;
                        }
                        //加入判断悬挂和离开的逻辑

                        //判断区域,优先吸附垂直的
                        if(parent.x > root.width / 2 && parent.x - root.width / 2 < 20) {
                            parent.x = root.width / 2;
                        }
                        else if(parent.x + parent.width < root.width / 2 && root.width / 2 - parent.x - parent.width < 20){
                            parent.x = root.width / 2 - parent.width;
                        }
                        else if(parent.y > root.height / 2 && parent.y - root.height / 2 <= 20) {
                            parent.y = root.height / 2;
                        }
                        else if(parent.y + parent.height < root.height / 2 && root.height / 2 - parent.y - parent.height <= 20) {
                            parent.y = root.height / 2 - parent.height;
                        }
                        else if(Math.abs(centerX - root.width / 2) <= 20){
                            parent.x = root.width / 2 - parent.width / 2;
                        }
                        else if(Math.abs(centerY - root.height / 2) <= 20){
                            parent.y = root.height / 2 - parent.height / 2;
                        }

                    }

                }
                onClicked: {
                    if(secondArea.viewState) return;
                    // 关闭右侧窗口
                    if(secondArea.viewState === true) {
                        secondAreaRect1.color = "white"
                        secondArea.viewState = false;
                    }
                    // 切换左侧窗口
                    if(firstArea.viewState === false) {
                        firstAreaRect1.color = "#00000000"
                        firstArea.viewState = true;
                    }
                    else {
                        firstAreaRect1.color = "white"
                        firstArea.viewState = false;
                    }
                    canvas.startX = secondAreaRect1.x + secondAreaRect1.width / 2;
                    canvas.startY = secondAreaRect1.y + secondAreaRect1.height / 2;
                    canvas.endX = firstAreaRect1.x + firstAreaRect1.width / 2;
                    canvas.endY = firstAreaRect1.y + firstAreaRect1.height / 2;
                    canvas.requestPaint();
                }
            }
            TResizeBorder {
                anchors.fill: parent
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.left: parent.left
                anchors.leftMargin: 6
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.top: parent.top
                anchors.topMargin: 6
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.top: parent.top
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: 6
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.bottom: parent.bottom
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.bottom: parent.bottom
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
            }
        }
        Rectangle {
            visible: root.defaultBackgroudState1
            color: root.defaultBackgroudColor1
            opacity: root.defaultBackgroudOpacity
            id:firstAreaRect2
            anchors.bottom: firstAreaRect1.top
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }
        Rectangle {
            visible: root.defaultBackgroudState1
            opacity: root.defaultBackgroudOpacity
            color: firstAreaRect2.color
            id:firstAreaRect3
            anchors.top: firstAreaRect2.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: firstAreaRect1.left
        }
        Rectangle {
            visible: root.defaultBackgroudState1
            opacity: root.defaultBackgroudOpacity
            color: firstAreaRect2.color
            id:firstAreaRect4
            anchors.top: firstAreaRect2.bottom
            anchors.bottom: parent.bottom
            anchors.left: firstAreaRect1.right
            anchors.right: parent.right
        }
        Rectangle {
            visible: root.defaultBackgroudState1
            opacity: root.defaultBackgroudOpacity
            color: firstAreaRect2.color
            id:firstAreaRect5
            anchors.top: firstAreaRect1.bottom
            anchors.bottom: parent.bottom
            anchors.left: firstAreaRect3.right
            anchors.right: firstAreaRect4.left
        }
    }

    Item {
        id: secondArea
        anchors.fill: parent
        property bool viewState: false
        Rectangle {
            id: secondAreaRect1
            x: parent.width / 2 - width
            y: parent.height / 2 - height
            width: parent.width/ 3
            height: parent.height / 3
            property int dragX: 0
            property int dragY: 0
            property bool dragging: false
            opacity: root.defaultRectangleOpacity
            color: "white"
            onWidthChanged: {
                canvas.startX = firstAreaRect1.x + firstAreaRect1.width / 2;
                canvas.startY = firstAreaRect1.y + firstAreaRect1.height / 2;
                canvas.endX = secondAreaRect1.x + secondAreaRect1.width / 2;
                canvas.endY = secondAreaRect1.y + secondAreaRect1.height / 2;
                canvas.requestPaint();
            }
            onHeightChanged: {
                canvas.startX = firstAreaRect1.x + firstAreaRect1.width / 2;
                canvas.startY = firstAreaRect1.y + firstAreaRect1.height / 2;
                canvas.endX = secondAreaRect1.x + secondAreaRect1.width / 2;
                canvas.endY = secondAreaRect1.y + secondAreaRect1.height / 2;
                canvas.requestPaint();

            }
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    parent.dragX = mouseX
                    parent.dragY = mouseY
                    parent.dragging = true
                }
                onReleased: {
                    vLine.visible = false;
                    hLine.visible = false;
                    parent.dragging = false
                }
                onPositionChanged: {
                    if(parent.dragging) {
                        //加入防碰撞检测
                        var newX = parent.x + mouseX - parent.dragX;
                        var newY = parent.y + mouseY - parent.dragY;
                        if(newX >= 0 && newX <= root.width - parent.width && newY >= 0 && newY <= root.height - parent.height){
                            root.defaultBackgroudState1 = false;
                            root.defaultBackgroudState2 = true;
                            parent.x = newX;
                            parent.y = newY;
                            canvas.startX = firstAreaRect1.x + firstAreaRect1.width / 2;
                            canvas.startY = firstAreaRect1.y + firstAreaRect1.height / 2;
                            canvas.endX = secondAreaRect1.x + secondAreaRect1.width / 2;
                            canvas.endY = secondAreaRect1.y + secondAreaRect1.height / 2;
                            canvas.requestPaint();

                        }
                        if(secondArea.viewState === false) return;
                        var centerX = parent.x + parent.width / 2;
                        var centerY = parent.y + parent.height / 2;
                        if(parent.x > root.width / 2 && parent.x - root.width / 2 <= 30) {
                            vLine.visible = true;
                        }
                        else if(parent.x + parent.width < root.width / 2 && root.width / 2 - parent.x - parent.width <= 30) {
                            vLine.visible = true;
                        }
                        else if(Math.abs(root.width / 2 - centerX) <= 30){
                            vLine.visible = true;
                        }
                        else {
                            vLine.visible = false;
                        }
                        if(parent.y > root.height / 2 && parent.y - root.height / 2 <= 30) {
                            hLine.visible = true;
                        }
                        else if(parent.y + parent.height < root.height / 2 && root.height / 2 - parent.y - parent.height <= 30) {
                            hLine.visible = true;
                        }
                        else if(Math.abs(centerY - root.height / 2) <= 30) {
                            hLine.visible = true;
                        }
                        else {
                            hLine.visible = false;
                        }
                        //加入判断悬挂和离开的逻辑

                        //判断区域,优先吸附垂直的
                        if(parent.x > root.width / 2 && parent.x - root.width / 2 < 20) {
                            parent.x = root.width / 2;
                        }
                        else if(parent.x + parent.width < root.width / 2 && root.width / 2 - parent.x - parent.width < 20){
                            parent.x = root.width / 2 - parent.width;
                        }
                        else if(parent.y > root.height / 2 && parent.y - root.height / 2 <= 20) {
                            parent.y = root.height / 2;
                        }
                        else if(parent.y + parent.height < root.height / 2 && root.height / 2 - parent.y - parent.height <= 20) {
                            parent.y = root.height / 2 - parent.height;
                        }
                        else if(Math.abs(centerX - root.width / 2) <= 20) {
                            parent.x = root.width / 2 - parent.width / 2;
                        }
                        else if(Math.abs(centerY - root.height / 2) <= 20) {
                            parent.y = root.height / 2 - parent.height / 2;
                        }
                    }
                }
                onClicked: {
                    if(firstArea.viewState) return;
                    // 关闭左侧窗口
                    if(firstArea.viewState === true) {
                        firstAreaRect1.color = "white"
                        firstArea.viewState = false;
                    }
                    // 切换右侧窗口
                    if(secondArea.viewState === false){
                        secondAreaRect1.color = "#00000000"
                        secondArea.viewState = true;
                    }
                    else{
                        secondAreaRect1.color = "white"
                        secondArea.viewState = false;
                    }
                    canvas.startX = firstAreaRect1.x + firstAreaRect1.width / 2;
                    canvas.startY = firstAreaRect1.y + firstAreaRect1.height / 2;
                    canvas.endX = secondAreaRect1.x + secondAreaRect1.width / 2;
                    canvas.endY = secondAreaRect1.y + secondAreaRect1.height / 2;
                    canvas.requestPaint();
                }
            }
            TResizeBorder{
                anchors.fill: parent
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.left: parent.left
                anchors.leftMargin: 6
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.top: parent.top
                anchors.topMargin: 6
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.top: parent.top
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: 6
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.bottom: parent.bottom
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
            }
            Rectangle{
                width: 12
                height: 6
                color: "yellow"
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.bottom: parent.bottom
            }
            Rectangle{
                width: 6
                height: 12
                color: "yellow"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
            }
        }
        Rectangle{
            visible: root.defaultBackgroudState2
            opacity: root.defaultBackgroudOpacity
            color: root.defaultBackgroudColor2
            id:secondAreaRect2
            anchors.bottom: secondAreaRect1.top
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }
        Rectangle{
            visible: root.defaultBackgroudState2
            opacity: secondAreaRect2.opacity
            color: secondAreaRect2.color
            id:secondAreaRect3
            anchors.top: secondAreaRect2.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: secondAreaRect1.left
        }
        Rectangle{
            visible: root.defaultBackgroudState2
            opacity: root.defaultBackgroudOpacity
            color: secondAreaRect2.color
            id:secondAreaRect4
            anchors.top: secondAreaRect2.bottom
            anchors.bottom: parent.bottom
            anchors.left: secondAreaRect1.right
            anchors.right: parent.right
        }
        Rectangle{
            visible: root.defaultBackgroudState2
            opacity: root.defaultBackgroudOpacity
            color: secondAreaRect2.color
            id:secondAreaRect5
            anchors.top: secondAreaRect1.bottom
            anchors.bottom: parent.bottom
            anchors.left: secondAreaRect3.right
            anchors.right: secondAreaRect4.left
        }
    }

    //创建中线
    Item{
        anchors.fill: parent
        Rectangle{
            id: hLine
            visible: false
            anchors.centerIn: parent
            width: parent.width
            height: 6
            color: "yellow"
        }
        Rectangle{
            id: vLine
            visible: false
            anchors.centerIn: parent
            width: 6
            height: parent.height
            color: "yellow"
        }
    }

    Canvas {
        id: canvas
        property double startX: firstAreaRect1.x + firstAreaRect1.width / 2;
        property double startY: firstAreaRect1.y + firstAreaRect1.height / 2;
        property double endX: secondAreaRect1.x + secondAreaRect1.width / 2;
        property double endY: secondAreaRect1.y + secondAreaRect1.height / 2;
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            var fromX = startX;
            var fromY = startY;
            var toX = endX;
            var toY = endY;
            var theta = 30; // 箭头尖端角度的一半
            var theta2 = 15;
            var headlen = Math.sqrt(Math.pow(toX - fromX, 2) + Math.pow(toY - fromY, 2)) / 12.0; // 箭头的长度
            var width = 3; // 线的宽度
            var headlen2 = headlen * Math.cos(theta * Math.PI / 180) / Math.cos(theta2 * Math.PI / 180);
            // 计算箭头尖端的角度
            var angle = Math.atan2(toY - fromY, toX - fromX);
            var angle1 = angle + (theta * Math.PI / 180);
            var angle2 = angle - (theta * Math.PI / 180);
            var angle3 = angle + (theta2 * Math.PI / 180);
            var angle4 = angle - (theta2 * Math.PI / 180);

            // 计算箭头的顶点坐标
            var topX = headlen * Math.cos(angle1);
            var topY = headlen * Math.sin(angle1);
            var botX = headlen * Math.cos(angle2);
            var botY = headlen * Math.sin(angle2);

            var topX2 = headlen2 * Math.cos(angle3);
            var topY2 = headlen2 * Math.sin(angle3);
            var botX2 = headlen2 * Math.cos(angle4);
            var botY2 = headlen2 * Math.sin(angle4);

            // 将坐标转换为相对于起点的位置
            var arrowTopX = toX - topX;
            var arrowTopY = toY - topY;
            var arrowBotX = toX - botX;
            var arrowBotY = toY - botY;

            var arrowTopX2 = toX - topX2;
            var arrowTopY2 = toY - topY2;
            var arrowBotX2 = toX - botX2;
            var arrowBotY2 = toY - botY2;

            // 绘制线段和箭头
            ctx.beginPath();
            ctx.moveTo(fromX, fromY);
            ctx.lineTo(arrowTopX2, arrowTopY2);
            ctx.lineTo(arrowTopX, arrowTopY);
            ctx.lineTo(toX, toY);
            ctx.lineTo(arrowBotX, arrowBotY);
            ctx.lineTo(arrowBotX2, arrowBotY2);
            ctx.lineTo(fromX, fromY);
            ctx.fillStyle = "#00ff00";
            ctx.fill();

            ctx.beginPath();
            ctx.strokeStyle = "yellow"
            ctx.moveTo(toX - 10, toY);
            ctx.lineTo(toX + 10, toY);
            ctx.moveTo(toX, toY - 10);
            ctx.lineTo(toX, toY + 10);
            ctx.stroke();

        }
    }

}
