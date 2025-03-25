#include "infodlg.h"
#include <qgraphicseffect.h>

InfoDlg::InfoDlg(QWidget *parent)
    : WindowFramelessWidget<QDialog>(parent)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.widget_title);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, false);
    auto shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(0x202020);
    shadow->setBlurRadius(10);
    this->setGraphicsEffect(shadow);
    this->setContentsMargins(8, 8, 8, 8);

}

InfoDlg *InfoDlg::getInstance() {
    static auto inst = new InfoDlg;
    return inst;
}

void InfoDlg::setStateStr(bool error, const QString &str) {
//    if(isVisible()) {
//        if(stateCache.size() >= 20) {
//            return;
//        }
//        auto info = qMakePair(error, str);
//        if(!stateCache.contains(info)) {
//            stateCache.push_front(info);
//        }
//    } else {
        ui.btn_state->setChecked(error);
        ui.label_str->setText(str);
//    }
}

void InfoDlg::on_btn_close_clicked() {
    if(stateCache.isEmpty()) {
        this->hide();
    } else {
        auto info = stateCache.back();
        ui.btn_state->setChecked(info.first);
        ui.label_str->setText(info.second);
        stateCache.pop_back();
    }
}
