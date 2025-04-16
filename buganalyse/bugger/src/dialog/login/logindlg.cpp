#include "logindlg.h"

LoginDlg::LoginDlg(QWidget *parent) : WindowFramelessWidget(parent)
{
    ui.setupUi(this);
    setMoveAreaWidget(ui.title);
    setAttribute(Qt::WA_DeleteOnClose);
}

void LoginDlg::init() {
    ui.widget_error->setVisible(false);
}
