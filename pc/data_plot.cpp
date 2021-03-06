#include <stdlib.h>
#include <qwt-qt4/qwt_painter.h>
#include <qwt-qt4/qwt_plot_canvas.h>
#include <qwt-qt4/qwt_plot_marker.h>
#include <qwt-qt4/qwt_plot_curve.h>
#include <qwt-qt4/qwt_scale_widget.h>
#include <qwt-qt4/qwt_legend.h>
#include <qwt-qt4/qwt_scale_draw.h>
#include <qwt-qt4/qwt_math.h>
#include "data_plot.h"

//
//  Initialize main window
//
DataPlot::DataPlot(QWidget *parent):
    QwtPlot(parent)
{
    // Disable polygon clipping
    QwtPainter::setDeviceClipping(false);

    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

#if QT_VERSION >= 0x040000
#ifdef Q_WS_X11
    /*
       Qt::WA_PaintOnScreen is only supported for X11, but leads
       to substantial bugs with Qt 4.2.x/Windows
     */
    canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif
#endif

    alignScales();
    
    //  Initialize data
    for (int i = 0; i< PLOT_SIZE; i++)
    {
        d_x[i] = 0.5 * i;     // time axis
        d_y[i] = 0;
        d_z[i] = 0;
    }

    // Assign a title
    setTitle("Quadrotor");
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

    // Insert new curves
    QwtPlotCurve *cRight = new QwtPlotCurve("Gyro X");
    cRight->attach(this);

    QwtPlotCurve *cLeft = new QwtPlotCurve("Gyro Y");
    cLeft->attach(this);

    // Set curve styles
    cRight->setPen(QPen(Qt::red));
    cLeft->setPen(QPen(Qt::blue));

    // Attach (don't copy) data. Both curves use the same x array.
    cRight->setRawData(d_x, d_y, PLOT_SIZE);
    cLeft->setRawData(d_x, d_z, PLOT_SIZE);

#if 0
    //  Insert zero line at y = 0
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
    mY->setLineStyle(QwtPlotMarker::HLine);
    mY->setYValue(0.0);
    mY->attach(this);
#endif

    // Axis 
    setAxisTitle(QwtPlot::xBottom, "Time/seconds");
    setAxisScale(QwtPlot::xBottom, 0, 100);

    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, -100, 100);

}

//
//  Set a plain canvas frame and align the scales to it
//
void DataPlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}

void DataPlot::addData(int gyro_x, int gyro_y)
{

    d_y[PLOT_SIZE-1] = gyro_x;
    d_z[PLOT_SIZE-1] = gyro_y;

    for ( int i = 0; i < PLOT_SIZE-1; i++ ) {
        d_y[i] = d_y[i+1];
        d_z[i] = d_z[i+1];
    }

    // update the display
    replot();

}
