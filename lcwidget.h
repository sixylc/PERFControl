#ifndef LCWIDGET_H
#define LCWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QTimer>

class LCWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LCWidget(QWidget *parent = nullptr);
    ~LCWidget();


signals:

protected:
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    bool move_all = false;
    bool move_top = false;
    bool move_bottom = false;
    bool move_left = false;
    bool move_right = false;
    QPoint offset_pos;
    QPoint old_CursorPos;
    QPoint old_pos;
    int old_width;
    int old_height;

    QLabel *AppButton;
    QLabel *minimize_button;
    QLabel *maximize_button;
    QLabel *exit_button;

};

#endif // LCWIDGET_H
