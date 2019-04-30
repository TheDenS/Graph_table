#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QResizeEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(400,400);
    this->setMaximumSize(1920,1080);
    currentIndex = -1;
    arrowSize = 25;

    plotInit();

    connect(ui->plot, &QCustomPlot::mousePress, this, &MainWindow::slotMousePress);
    connect(ui->plot, &QCustomPlot::mouseMove, this, &MainWindow::slotMouseMove);
    connect(ui->plot, &QCustomPlot::plottableClick, this, &MainWindow::graphClicked);
    connect(ui->plot, &QCustomPlot::mouseRelease,this,&MainWindow::mouseRelease);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotInit()
{
    ui->plot->addGraph();
    graph = ui->plot->graph(0);

    ui->plot->setInteraction(QCP::iSelectPlottables,true);
    graph->setSelectable(QCP::stSingleData);
    ui->plot->setInteraction(QCP::iSelectItems,true);
    ui->plot->setSelectionTolerance(10);
    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");
    ui->plot->xAxis->setRange(-1, 60);
    ui->plot->yAxis->setRange(0, 60);
    ui->plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
    ui->plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,15));

    arrow_1 = new QCPItemLine(ui->plot);
    arrow_1->start->setCoords(0,0);
    arrow_1->end->setCoords(1, 0);
    arrow_1->setHead(QCPLineEnding::esSpikeArrow);
    arrow_1->setVisible(false);

    arrow_2 = new QCPItemLine(ui->plot);
    arrow_2->start->setCoords(0,0);
    arrow_2->end->setCoords(0, 1);
    arrow_2->setHead(QCPLineEnding::esSpikeArrow);
    arrow_2->setVisible(false);

    points<<QPoint(0,50)<<QPoint(10,50)<<QPoint(10,30)<<QPoint(20,30)<<QPoint(20,40)<<QPoint(30,40)<<QPoint(30,20)<<QPoint(40,20)<<QPoint(40,10)<<QPoint(50,10);
    rePlot(QPointF(0,0));
    showTable();
}

void MainWindow::rePlot(QPointF point)
{
    QVector<double> x(10),y(10);
    for(int i =0; i< points.count();i++)
    {
        x[i]=points.at(i).x();
        y[i]=points.at(i).y();

    }

    ui->plot->graph(0)->setData(x,y);

    arrow_1->start->setPixelPosition(point);
    arrow_2->start->setPixelPosition(point);
    arrow_1->end->setPixelPosition(QPointF(point.x(),point.y()-arrowSize).toPoint());
    arrow_2->end->setPixelPosition(QPointF(point.x()+arrowSize,point.y()).toPoint());

    ui->plot->replot();
}

void MainWindow::showTable()
{
    QStringList horizontalHeader;
    horizontalHeader.append("X");
    horizontalHeader.append("Y");
    ui->table->setHorizontalHeaderLabels(horizontalHeader);


    ui->table->setRowCount(points.count());
    ui->table->setColumnCount(2);

    for(int i = 0; i< points.count();i++)
    {
        ui->table->setItem(i,0,new QTableWidgetItem(QString::number(points.at(i).x())));
        ui->table->setItem(i,1,new QTableWidgetItem(QString::number(points.at(i).y())));
    }
    ui->table->setColumnWidth(0,45);
    ui->table->setColumnWidth(1,45);
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    int w,h;
    w = event->size().width();
    h = event->size().height();

    ui->plot->setGeometry(20,20,w-180,h-60);
    ui->plot->replot();
    ui->table->setGeometry(w-140,20,120,h-60);


}

void MainWindow::slotMousePress(QMouseEvent *event)
{
    QPointF point;
    double coordX = event->pos().x();
    double coordY = event->pos().y();
    point = arrow_1->start->pixelPosition().toPoint();

    //Vertical arrow
    if(arrow_1->selected())
    {
        point.setY(coordY);
        if(currentIndex!=-1)
        {
            points.replace(currentIndex,arrow_1->start->coords().toPoint());
            rePlot(point);
            showTable();
        }
    }

    //Horizontal arrow
    if(arrow_2->selected())
    {
        point.setX(coordX);
        if(currentIndex!=-1)
        {
            points.replace(currentIndex,arrow_1->start->coords().toPoint());
            rePlot(point);
            showTable();
        }
    }
}

void MainWindow::slotMouseMove(QMouseEvent *event)
{
    if(QApplication::mouseButtons()) slotMousePress(event);
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
    currentIndex = dataIndex;
    double coordX = plottable->interface1D()->dataPixelPosition(dataIndex).x();
    double coordY = plottable->interface1D()->dataPixelPosition(dataIndex).y();
    QPointF p(coordX,coordY);

    arrow_1->setVisible(true);
    arrow_1->start->setPixelPosition(p);
    arrow_1->end->setPixelPosition(QPointF(coordX,coordY-arrowSize));

    arrow_2->setVisible(true);
    arrow_2->start->setPixelPosition(p);
    arrow_2->end->setPixelPosition(QPointF(coordX+arrowSize,coordY));

    ui->plot->replot();
}

void MainWindow::mouseRelease(QMouseEvent *event)
{
    if(arrow_1->selected()||arrow_2->selected())
    {
        arrow_1->setVisible(false);
        arrow_2->setVisible(false);
        arrow_1->setSelected(false);
        arrow_2->setSelected(false);
        currentIndex = -1;
    }
    ui->plot->replot();
}

void MainWindow::on_table_cellChanged(int row, int column)
{
    if(row<points.count())
    {
        QPoint p;
        if(column>0)
        {
            p.setX(ui->table->item(row,0)->text().toInt());
            p.setY(ui->table->item(row,1)->text().toInt());
        }
        else
        {
            p.setX(ui->table->item(row,0)->text().toInt());
            p.setY(points.at(row).y());
        }
        if(p!=points.at(row))
        {
            points.replace(row,p);
            rePlot(p);
        }
    }
}
