#include "lcwidget.h"

LCWidget::LCWidget(QWidget *parent)
    : QWidget{parent,Qt::FramelessWindowHint}
{
    QScreen *screen = QApplication::primaryScreen();
    move((screen->geometry().width() - width())/2,(screen->geometry().height() - height())/2);

    setAttribute(Qt::WA_TranslucentBackground);

    setMouseTracking(true); // 启用鼠标跟踪功能
    AppButton = new QLabel("M", this);
    AppButton->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    AppButton->setStyleSheet("QLabel {background-color: #007bff;border-radius: 4px;color: #fff;}QLabel:hover {background-color: #0069d9;}QLabel:pressed {background-color: #005cbf;}");
    AppButton->show();

    minimize_button = new QLabel("V", this);
    minimize_button->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    minimize_button->setStyleSheet("QLabel {background-color: #007bff;border-radius: 4px;color: #fff;}QLabel:hover {background-color: #0069d9;}QLabel:pressed {background-color: #005cbf;}");
    minimize_button->show();

    maximize_button = new QLabel("∧", this);
    maximize_button->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    maximize_button->setStyleSheet("QLabel {background-color: #007bff;border-radius: 4px;color: #fff;}QLabel:hover {background-color: #0069d9;}QLabel:pressed {background-color: #005cbf;}");
    maximize_button->show();

    exit_button = new QLabel("X", this);
    exit_button->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    exit_button->setStyleSheet("QLabel {background-color: #007bff;border-radius: 4px;color: #fff;}QLabel:hover {background-color: #0069d9;}QLabel:pressed {background-color: #005cbf;}");
    exit_button->show();

}

LCWidget::~LCWidget()
{


}



void LCWidget::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, width(), height()), 20, 20);

    QRectF contentRect = QRectF(10, 10, width() - 20, height() - 20);
    QPainterPath contentPath;
    contentPath.addRoundedRect(contentRect, 10, 10);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    painter.fillPath(path, QColor(255, 255, 255, 255));
    painter.setBrush(QColor(255, 255, 255, 255));

    painter.fillPath(contentPath, QColor(255, 255, 255,255));
    painter.drawPath(path);

    AppButton->setGeometry(10,10,20,20);
    minimize_button->setGeometry(width()-90,10,20,20);
    maximize_button->setGeometry(width()-60,10,20,20);
    exit_button->setGeometry(width()-30,10,20,20);
}

void LCWidget::enterEvent(QEnterEvent* event)
{
    if(mapFromGlobal(QCursor::pos()).y() < 10 || mapFromGlobal(QCursor::pos()).y() > height() - 10)
    {
        if(cursor() != Qt::SizeVerCursor)setCursor(Qt::SizeVerCursor);
        QTimer::singleShot(200,this,[=]
        {
            enterEvent(event);
        });
    }
    else if(mapFromGlobal(QCursor::pos()).x() < 10 || mapFromGlobal(QCursor::pos()).x() > width() - 10)
    {
        if(cursor() != Qt::SizeHorCursor)setCursor(Qt::SizeHorCursor);
        QTimer::singleShot(200,this,[=]
        {
            enterEvent(event);
        });
    }
    else
        setCursor(Qt::ArrowCursor);
}

void LCWidget::leaveEvent(QEvent* event)
{

}

void LCWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(move_top)
    {
        resize(old_width,old_height + old_CursorPos.y() - QCursor().pos().y());
        move(QPoint(old_CursorPos.x(),event->globalPos().y()) + offset_pos);
    }
    else if(move_bottom)
    {
        resize(old_width,old_height - old_CursorPos.y() + QCursor().pos().y());
    }
    else if(move_left)
    {
        resize(old_width + old_CursorPos.x() - QCursor().pos().x(),old_height);
        move(QPoint(event->globalPos().x(),old_CursorPos.y()) + offset_pos);
    }
    else if(move_right)
    {
        resize(old_width - old_CursorPos.x() + QCursor().pos().x(),old_height);
    }
    else if(move_all)
        move(event->globalPos() + offset_pos);

}

void LCWidget::mousePressEvent(QMouseEvent *event)
{
    old_pos = pos();
    old_CursorPos = QCursor().pos();
    offset_pos = old_pos - old_CursorPos;
    old_width = width();
    old_height = height();

    if(minimize_button->geometry().contains(event->pos()))
    {
        showMinimized();
        return;
    }

    if(maximize_button->geometry().contains(event->pos()))
    {
        if(isMaximized())
            showNormal();
        else
            showMaximized();
        return;
    }

    if(exit_button->geometry().contains(event->pos()))
    {
        QApplication::exit(0);
        return;
    }

    if(event->button() == Qt::LeftButton)
    {
        if(event->y() < 10)
            move_top = true;
        else if(event->y() < 30)
        {
            move_all = true;
            setWindowOpacity(0.5);
        }
        else if(event->y() > height() - 10)
            move_bottom = true;

        if(event->x() < 10)
            move_left = true;
        else if(event->x() > width() - 10)
            move_right = true;
    }
}

void LCWidget::mouseReleaseEvent(QMouseEvent *event)
{
    move_all = false;
    move_top = false;
    move_bottom = false;
    move_left = false;
    move_right = false;
    setWindowOpacity(1);
}
