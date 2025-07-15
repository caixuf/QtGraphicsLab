import QtQuick 2.7
Item {
    id: root
    width: parent.width
    height: parent.height
    property var control: parent
    TDragRect {
        posType: posLeftTop
        onPosChange: {
            //不要简化这个判断条件
            if (control.x + xOffset < control.x + control.width)
                control.x += xOffset;
            if (control.y + yOffset < control.y + control.height)
                control.y += yOffset;
            if (control.width - xOffset > 0)
                control.width-= xOffset;
            if (control.height -yOffset > 0)
                control.height -= yOffset;
        }
    }
    TDragRect {
        posType: posTop
        x: 6
        width: parent.width - 12
        height: 2
        onPosChange: {
            if (control.y + yOffset < control.y + control.height)
                control.y += yOffset;
            if (control.height - yOffset > 0)
                control.height -= yOffset;
        }
    }
    TDragRect {
        posType: posRightTop
        x: parent.width - width
        onPosChange: {
            //向左拖动时，xOffset为负数
            if (control.width + xOffset > 0)
                control.width += xOffset;
            if (control.height - yOffset > 0)
                control.height -= yOffset;
            if (control.y + yOffset < control.y + control.height)
                control.y += yOffset;
        }
    }
    TDragRect {
        posType: posLeft
        y: 6
        height: parent.height - 12
        width: 2
        onPosChange: {
            if (control.x + xOffset < control.x + control.width)
                control.x += xOffset;
            if (control.width - xOffset > 0)
                control.width -= xOffset;
        }
    }
    TDragRect {
        posType: posRight
        x: parent.width - 2
        y: 6
        height: parent.height - 12
        width: 2
        onPosChange: {
            if (control.width + xOffset > 0)
                control.width += xOffset;
        }
    }
    TDragRect {
        posType: posLeftBottom
        y: parent.height - height
        onPosChange: {
            if (control.x + xOffset < control.x + control.width)
                control.x += xOffset;
            if (control.width - xOffset > 0)
                control.width-= xOffset;
            if (control.height + yOffset > 0)
                control.height += yOffset;
        }
    }
    TDragRect {
        posType: posBottom
        x: 6
        y: parent.height - 2
        width: parent.width - 12
        height: 2
        onPosChange: {
            if (control.height + yOffset > 0)
                control.height += yOffset;
        }
    }
    TDragRect {
        posType: posRightBottom
        x: parent.width - width
        y: parent.height - height
        onPosChange: {
            if (control.width + xOffset > 0)
                control.width += xOffset;
            if (control.height + yOffset > 0)
                control.height += yOffset;
        }
    }
}
