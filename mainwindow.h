#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include <QStandardItemModel>
#include <QStandardItem>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void plotInit();
    void rePlot(QPointF point);
    void showTable();
protected:
    void resizeEvent(QResizeEvent * event);

private:
    Ui::MainWindow *ui;
    QVector <QPoint> points;
    QCPGraph *graph;
    QCPItemLine *arrow_1, *arrow_2;
    int currentIndex;
    int arrowSize;

private slots:
    void slotMousePress(QMouseEvent * event);
    void slotMouseMove(QMouseEvent * event);

    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

    void mouseRelease(QMouseEvent * event);

    void on_table_cellChanged(int row, int column);
};

#endif // MAINWINDOW_H
