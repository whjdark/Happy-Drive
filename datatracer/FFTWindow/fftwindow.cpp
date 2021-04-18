#include "fftwindow.h"
#include "ui_fftwindow.h"

FFTWindow::FFTWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::FFTWindow)
  , m_FFT(new AmAndPhGraph(this))
{
  ui->setupUi(this);
  initFFTWidget();
  initToolBar();
}

FFTWindow::~FFTWindow()
{
  delete ui;
}

void
FFTWindow::initFFTWidget()
{
  setCentralWidget(m_FFT);
  // set main title
  m_FFT->setTitle(QStringLiteral("DFT result"));
  // set Amplitude&tPhase graph xy title
  m_FFT->setAmplitudeTitle(QStringLiteral(), QStringLiteral("Amplitude"));
  m_FFT->setPhaseTitle(QStringLiteral("frequency(Hz)"),
                       QStringLiteral("Phase(deg)"));
  m_FFT->setTracerVisible(false); //默认关闭游标
}

void
FFTWindow::initToolBar()
{
  //工具栏
  QToolBar* toolBar = this->addToolBar(QStringLiteral("工具栏"));
  toolBar->setIconSize(QSize(25, 25));
  toolBar->setMovable(false);
  QAction* exportDataAction = toolBar->addAction(QStringLiteral("保存数据"));
  QAction* saveImageAction = toolBar->addAction(QStringLiteral("保存图片"));
  QAction* autoScaleAction = toolBar->addAction(QStringLiteral("自动缩放"));
  QAction* switchTracerAction = toolBar->addAction(QStringLiteral("游标"));
  //工具栏动作
  using FW = FFTWindow;
  using APG = AmAndPhGraph;
  //导出数据
  connect(exportDataAction, &QAction::triggered, this, &FW::slotExportData);
  //导出图片
  connect(saveImageAction, &QAction::triggered, this, &FW::slotSaveImage);
  //自动缩放
  connect(autoScaleAction, &QAction::triggered, m_FFT, &APG::autoScaleAxis);
  //开启/关闭游标
  connect(switchTracerAction, &QAction::triggered, this, &FW::slotSwitchTracer);
}

void
FFTWindow::slotExportData()
{
  // have data in graph ?
  if (m_amplitude.size() == 0 || m_phase.size() == 0) {
    // no data in graph
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("无数据"));
    return;
  }
  //保存文件对话框
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("保存数据"),
    qApp->applicationDirPath() % QLatin1String("FFT_") % timeStr %
      QLatin1String(".csv"),
    QStringLiteral("Files (*.csv *.txt)"));
  if (fileName.isEmpty()) {
    return;
  }
  //保存文件
  QFile file(fileName);
  //以只写方式打开
  // Truncate清空文件
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("打开/创建文件失败！"));
    return;
  }
  //写入每一行数据到文件
  //如果am和ph数据长度不一致此处可能有段错误
  //频率|幅值|相位
  int i = 0;
  Q_FOREACH (const auto& pt, m_amplitude) {
    QTextStream out(&file);
    out << pt.key << ',' << pt.value << ',' << m_phase[i].value << "\n";
    i++;
  }
  file.close();
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
}

void
FFTWindow::slotSaveImage()
{
  QPixmap p = m_FFT->grab();
  QImage img = p.toImage();
  if (img.isNull()) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("截取图像失败"));
  }
  //保存文件对话框
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("保存数据图像"),
    qApp->applicationDirPath() % QLatin1String("FFT_") % timeStr %
      QLatin1String(".png"),
    QStringLiteral("Images (*.png)"));
  if (fileName.isEmpty()) {
    return;
  }
  //保存文件
  if (img.save(fileName)) { // 打开/创建图像成功
    QMessageBox::information(
      this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
  } else { // 打开/创建图像失败
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("创建图像失败！"));
  }
}

void
FFTWindow::slotSwitchTracer()
{
  if (m_FFT->isTracerVisible()) {
    m_FFT->setTracerVisible(false);
  } else {
    m_FFT->setTracerVisible(true);
  }
}

auto
FFTWindow::doFFT(const DataVector& data) -> int
{
  if (data.isEmpty()) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("空数据"));
    return -1;
  }
  //为fft输入计算分配空间
  int N = data.size(); //采样点数
  double* in = (double*)fftw_malloc(sizeof(double) * N);
  if (in == nullptr) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("分配输入空间失败"));
    return -1;
  }
  int index = 0;
  Q_FOREACH (const auto& pt, data) {
    //数据写入缓冲区
    in[index] = pt.value;
    index++;
  }
  //为fft输出分配空间
  fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
  if (out == nullptr) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("分配输出空间失败"));
    return -1;
  }
  //进行fft变换，fftw_plan_dft_c2r_1d函数进行反变换
  //输入纯实数 real2complexs
  fftw_plan p = FFTW3_H::fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
  fftw_execute(p);
  //计算分辨率(频率间隔)
  //采样频率，由两个点的dx算出
  double fs = 1 / (data[1].key - data[0].key);
  double f0 = fs / N; //分辨率(频率间隔)f0
  // clear storage
  m_amplitude.clear();
  m_phase.clear();
  m_amplitude.reserve(N / 2);
  m_phase.reserve(N / 2);
  for (int i = 0; i < N / 2; i++) {
    //根据FFT计算的复数计算振幅谱
    double f = i * f0; //横坐标 频率HZ
    double Re = out[i][0];
    double Im = out[i][1];
    double Xk = qSqrt(qPow(Re, 2) + qPow(Im, 2)); //幅值
    double Xm = Xk * 2 / N;                       //幅值量化变换
    m_amplitude.append(QCPGraphData(f, Xm));
    //根据FFT计算的复数计算相位谱
    double phi = qAtan2(Im, Re);      //相位
    double phiDeg = phi * 180 / M_PI; //化成deg(°)位
    m_phase.append(QCPGraphData(f, phiDeg));
  }
  //直流信号的处理
  m_amplitude[0].value = m_amplitude[0].value / 2;
  //绘制bode图
  m_FFT->showGraph(m_amplitude, m_phase);
  //释放buffer
  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
  // success
  return 0;
}
