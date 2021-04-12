/*
 * @Author: whj
 * @Date: 2021-03-02 14:35:16
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:04:06
 * @Description: file content
 */

#ifndef DATATRACER_H
#define DATATRACER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>

#include "../BaseDataType/bitconverter.h"
#include "../BaseDataType/driverdatatype.h"
#include "../CONconfiger/CONconfigdialog.h"
#include "../QCustomPlot/qcustomplot.h"
#include "../oscilloscope/oscilloscope.h"
#include "../runconfig/runconfigdialog.h"
#include "../xcomm/xcomm.h"
#include "FFTWindow/fftwindow.h"
#include "selectlinestyledialog/selectlinestyledialog.h"
#include "statisticsWindow/statisticswindow.h"

namespace Ui {
class DataTracer;
}

class TracerWidget : public QWidget
{
  Q_OBJECT

public:
  enum Channels
  {
    Ch1,
    Ch2,
    Ch3,
    Ch4,
    Ch5,
    Ch6,
    Ch7,
    Ch8,
    ChUNK,
  };
  Q_ENUM(Channels)

  enum LegendPosition
  {
    BottomRight,
    TopRight,
    BottomLeft,
    TopLeft,
  };
  Q_ENUM(LegendPosition)

  enum Themes
  {
    LightTheme,
    DarkTheme,
  };
  Q_ENUM(Themes)

  enum DataTransformType
  {
    CURRENT,
    VELOCITY,
    POSITION,
    ENCODER,
  };
  Q_ENUM(DataTransformType)

public:
  explicit TracerWidget(QWidget* parent, XComm* xcomm);
  ~TracerWidget();

  void setStartButtonOn() { on_startButton_clicked(); }
  void setStopButtonOn() { on_stopButton_clicked(); }
  int setConfigButtonOn() { return on_configButton_clicked(); }

private Q_SLOTS:
  void slotProccessCmd(const quint16 cmd, const QByteArray& data);
  void slotSelectionChanged();

  void on_startButton_clicked();
  void on_stopButton_clicked();
  int on_configButton_clicked();
  void on_readButton_clicked();
  void on_autoSetButton_clicked();
  void on_closeAllchannelButton_clicked();
  void on_showAllchannelButton_clicked();
  void on_clearDataButton_clicked();
  void on_defaultButton_clicked();

  void on_xMaxDoubleSpinBox_valueChanged(double xMax);
  void on_xMinDoubleSpinBox_valueChanged(double xMin);
  void on_yMaxDoubleSpinBox_valueChanged(double yMax);
  void on_yMinDoubleSpinBox_valueChanged(double yMin);
  void on_xTickSpinBox_valueChanged(int xTick);
  void on_yTickSpinBox_valueChanged(int yTick);

  void on_exportDataButton_clicked();
  void on_exportImgButton_clicked();
  void on_importDataButton_clicked();

  void on_ch1Box_stateChanged(int state);
  void on_ch2Box_stateChanged(int state);
  void on_ch3Box_stateChanged(int state);
  void on_ch4Box_stateChanged(int state);
  void on_ch5Box_stateChanged(int state);
  void on_ch7Box_stateChanged(int state);
  void on_ch6Box_stateChanged(int state);
  void on_ch8Box_stateChanged(int state);

  void on_legendBox_stateChanged(int state);
  void on_legendPosComboBox_currentIndexChanged(int index);

  void on_openGLBox_stateChanged(int state);
  void on_themeBox_currentIndexChanged(int index);

  void on_tracerCheckBox_stateChanged(int state);

  void on_FFTButton_clicked();
  void on_statsButton_clicked();

  void on_OscTitleLineEdit_editingFinished();
  void on_xTitleLineEdit_editingFinished();
  void on_yTitleLineEdit_editingFinished();

  void on_subAxisCheckBox_stateChanged(int state);

  void slotxAxisChanged(const QCPRange& range);
  void slotyAxisChanged(const QCPRange& range);

  void slotLegendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);

  void slotTitleDoubleClick(QMouseEvent* event);
  void slotAxisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);

  void slotMousePress();
  void slotMouseWheel();

  void on_ch1ComboBox_currentIndexChanged(int index);
  void on_ch2ComboBox_currentIndexChanged(int index);
  void on_ch3ComboBox_currentIndexChanged(int index);
  void on_ch4ComboBox_currentIndexChanged(int index);
  void on_ch5ComboBox_currentIndexChanged(int index);
  void on_ch6ComboBox_currentIndexChanged(int index);
  void on_ch7ComboBox_currentIndexChanged(int index);
  void on_ch8ComboBox_currentIndexChanged(int index);

  void slotUpdateStats();

private:
  void initChart();
  void initChannels();
  void initPannel();
  void initStatisticsWindow();
  void initToolBox();
  void initConnections();

  void readChBoxContent();
  void storeTransformedData(Channels waitForUdpdateCh,
                            const QByteArray& data,
                            DataTransformType type);

  void openChannel(Channels channel);
  void closeChannel(Channels channel);

  void addDataToCh(Channels channel, const qreal x, const qreal y);
  void relaceChannelData(Channels channel, const QVector<QCPGraphData>& data);

  Channels selectChannel();
  void setChannelDispay(Channels ch, int state);

  void setDarkTheme();
  void setLightTheme();

  void setSubGrid(bool enable);

  XComm::CmdType chComboIndexMapToCmd(int index);

private:
  //通道颜色参数
  const QColor channelColors[16] = {
    QColor(255, 20, 20),   // Channel1Color
    QColor(255, 127, 80),  // Channel2Color
    QColor(30, 144, 255),  // Channel3Color
    QColor(218, 165, 32),  // Channel4Color
    QColor(255, 0, 255),   // Channel5Color
    QColor(147, 112, 219), // Channel6Color
    QColor(0, 255, 255),   // Channel7Color
    QColor(50, 120, 158),  // Channel8Color
    QColor(128, 0, 128),   // Channel9Color
    QColor(46, 139, 87),   // Channel10Color
    QColor(152, 251, 152), // Channel11Color
    QColor(0, 255, 127),   // Channel12Color
    QColor(0, 0, 255),     // Channel13Color
    QColor(135, 206, 250), // Channel14Color
    QColor(255, 192, 203), // Channel15Color
    QColor(127, 255, 0)    // Channel16Color
  };
  //通道参数列表
  const QStringList channelList{
    // use c++11 initializer_list
    "速度给定值",       "速度反馈值",       "速度调节器输出值", "位置给定值",
    "位置反馈值",       "位置调节器输出值", "电流给定值",       "电流反馈值",
    "电流调节器输出值", "编码器时钟数值",   "U相电流采样值",    "V相电流采样值",
    "W相电流采样值"
  };
  //默认刻度数
  Q_CONSTEXPR static int defalutXTick = 18, defalutYTick = 16;
  //默认轴刻度范围大小
  Q_CONSTEXPR static qreal defalutMaxX = 5;
  Q_CONSTEXPR static qreal defalutMaxY = 1000;
  Q_CONSTEXPR static qreal defalutMinX = 0;
  Q_CONSTEXPR static qreal defalutMinY = 0;
  //通道数
  Q_CONSTEXPR static int maxChannels = 8;
  // OpenGL加速
  Q_CONSTEXPR static bool defaultOpenGL = false;
  //图例
  Q_CONSTEXPR static bool defaultLegend = true;
  //游标
  Q_CONSTEXPR static bool defaultTracerVisible = false;
  // main标题
  const QString defautGraphTitle = QStringLiteral("graph");
  // x标题
  const QString defaultXTitle = QStringLiteral("Time(s)");
  // y标题
  const QString defaultYTitle = QStringLiteral("Amplitude");
  //检查接收数据数量是否与请求的数量一致时间间隔
  Q_CONSTEXPR static int defaultCheckRecvInterval = 1000; //  1.0s
  //刷新统计信息表时间间隔
  Q_CONSTEXPR static int defaultUpdateStatusInterval = 1500; //  1.5s

  Ui::DataTracer* ui;
  XComm* m_xcomm;
  CONConfigQDialog* m_controllerConfiger;
  RunConfigDialog* m_runConfiger;
  StatisticsWindow* m_statisticsWindow;
  FFTWindow* m_fftWindow;
  //通道请求顺序队列
  QQueue<Channels> m_chReqOrderQueue;
  //每个通道的数据
  std::array<QVector<QCPGraphData>, maxChannels> m_points;
  //标题
  QCPTextElement* m_title;
  qreal m_minAxisX = defalutMaxX, m_maxAxisX = defalutMaxY; // X坐标轴刻度范围
  qreal m_minAxisY = defalutMinX, m_maxAxisY = defalutMinY; // Y坐标轴刻度范围
  // timers
  QTimer* m_updateStatsTimer;
};

#endif // DATATRACER_H
