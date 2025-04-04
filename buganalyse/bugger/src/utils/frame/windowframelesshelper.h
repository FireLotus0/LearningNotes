#pragma once

#include <qobject.h>
#include <qevent.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <type_traits>
#include <qgraphicseffect.h>

/**
     * @brief 记录当前鼠标在窗口中所在的区域
     */
enum MouseRegion {
    Window_Left_Border,            ///< 窗口左边框
    Window_Right_Border,           ///< 窗口右边框
    Window_Top_Border,             ///< 窗口上边框
    Window_Bottom_Border,          ///< 窗口下边框
    Window_Left_Top_Corner,        ///< 窗口左上角
    Window_Right_Top_Corner,       ///< 窗口右上角
    Window_Left_Bottom_Corner,     ///< 窗口左下角
    Window_Right_Bottom_Corner,    ///< 窗口右上角
    Window_Internal_Area           ///< 窗口内部区域
};

template<typename T>
class WindowFramelessWidget;

class WindowFramelessHelper : public QWidget
{
public:
    explicit WindowFramelessHelper(QWidget* parent = nullptr);

    void initWidgetMember(QWidget* wid);

    void pressEvent(QMouseEvent *event, QWidget* widget);

    void moveEvent(QMouseEvent *event);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void releaseEvent(QMouseEvent *event);

    MouseRegion getMouseRegion(QPoint mousePos);

    void changeCursorShapeByRegion();

    void stretchWindowByMouse(QMouseEvent *event);

    void setMoveWidget(QWidget* widget);

    void setWindowResizeable(bool resiable);
private:
    bool leftButtonPressed = false; ///< 窗口拖动状态标志
    QPoint start;   ///< 窗口拖动的起始位置
    MouseRegion currentMouseRegion = MouseRegion::Window_Internal_Area;     ///< 当前鼠标所在区域
    QWidget* widget;
    QWidget* moveWidget{nullptr};
    bool windowResizeable = true;
};

template<typename T>
class WindowFramelessWidget : public T {
public:
    explicit WindowFramelessWidget(QWidget* parent = nullptr) : T(parent){
        //去除默认边框

        this->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint); //| Qt::X11BypassWindowManagerHint

        //开启鼠标悬停事件
        this->setAttribute(Qt::WA_Hover,true);
        this->installEventFilter(&framelessHelper);
        framelessHelper.initWidgetMember((QWidget*)this);

        this->setAttribute(Qt::WA_DeleteOnClose);
        this->setAttribute(Qt::WA_ShowModal);
        auto shadow = new QGraphicsDropShadowEffect(this);
        shadow->setOffset(0, 0);
        shadow->setColor(0x202020);
        shadow->setBlurRadius(10);
        this->setGraphicsEffect(shadow);
        this->setContentsMargins(8, 8, 8, 8);
    }

    /**
     * @brief 窗口拖动开始
     * @note 函数功能全面说明
     *     - 实现窗口拖动开始的判定
     */
    void mousePressEvent(QMouseEvent *event) override {
        framelessHelper.pressEvent(event, moveAreaWidget);
    }

    /**
     * @brief 窗口拖动
     * @note 函数功能全面说明
     *     - 实现窗口拖动过程中窗口位置的计算
     */
    void mouseMoveEvent(QMouseEvent *event) override {
        framelessHelper.moveEvent(event);
    }

    /**
     * @brief 窗口拖动结束
     * @note 函数功能全面说明
     *     - 实现窗口拖动结束的处理
     */
    void mouseReleaseEvent(QMouseEvent *event) override {
        framelessHelper.releaseEvent(event);
    }

    /**
     * @brief 根据鼠标位置，计算当前鼠标在窗口中的区域
     * @param mousePos 鼠标位置
     * @return 窗口区域
     */
    MouseRegion getMouseRegion(QPoint mousePos) {
        return framelessHelper.getMouseRegion(mousePos);
    }

    /**
     * @brief 根据鼠标所在窗口区域，改变鼠标形状
     */
    void changeCursorShapeByRegion() {
        framelessHelper.changeCursorShapeByRegion();
    }

    /**
     * @brief 根据鼠标拉伸窗口，计算窗口size
     */
    void stretchWindowByMouse(QMouseEvent *event) {
        framelessHelper.stretchWindowByMouse(event);
    }

    /**
     * @brief 设置拖动区域组件
     * @param widget
     */
    void setMoveAreaWidget(QWidget* widget) {
        moveAreaWidget = widget;
        framelessHelper.setMoveWidget(widget);
        //当T = QDialog时，设置界面上的按钮autoDefault属性为false，避免按下回车键，导致触发按钮点击事件
        auto btns = this->template findChildren<QPushButton*>();
        for(const auto& btn : btns) {
            btn->setAutoDefault(false);
        }
    }

    void setWindowResizeable(bool resizeable) {
        framelessHelper.setWindowResizeable(resizeable);
    }

    friend class WindowFramelessHelper;
private:
    WindowFramelessHelper framelessHelper;
    QWidget* moveAreaWidget;    ///< 在此区域点击鼠标可进行拖动
};