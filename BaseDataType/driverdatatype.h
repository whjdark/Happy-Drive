/*-----------------------------------------------------------------------------
  cmd head
-----------------------------------------------------------------------------*//*
 * @Author: whj
 * @Date: 2021-02-10 20:39:17
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:03:51
 * @Description: file content
 */

#ifndef DRIVERDATATYPE_H
#define DRIVERDATATYPE_H

#include <QObject>

namespace DriverDataType {
//常量
//浮点数-整型缩放因子
Q_CONSTEXPR double scaleFactor = 2 << 14;
//速度变换因子
Q_CONSTEXPR double velocityTransformFactor = 2000.0;
//电流变换因子
Q_CONSTEXPR double currentTransformFactor = 19.2;
//位置变换因子
Q_CONSTEXPR double positionTransformFactor = 1.0;
//编码器变换因子
Q_CONSTEXPR double encoderTransformFactor = 1.0;

//数据类型
//电流数据类型
typedef qint16 currentDataType;
//速度数据类型
typedef qint16 velocityDataType;
//位置数据类型
typedef qint32 positionDataType;
//编码器数据类型
typedef qint16 encoderDataType;

//运行模式
enum RunMode
{
  MODE0 = 0,
  MODE1 = 1,
  MODE2 = 2,
  MODE3 = 3,
  MODE4 = 4,
  MODE5 = 5,
  MODE6 = 6,

};

struct PIDParams
{
  quint32 kp;
  quint32 ki;
  quint32 kd;

  PIDParams() = default;
};

struct ADRCParams
{
  quint32 wo;
  quint32 wc;
  quint32 bi;

  ADRCParams() = default;
};

struct FOPDGESOParams
{
  quint32 kp;
  quint32 kd;
  quint32 mu;
  quint32 omegaO;
  quint32 b0;
  quint32 a1;
  quint32 a2;
  quint32 h;

  FOPDGESOParams() = default;
};

struct PIDKcParams
{
  quint32 currentKc;
  quint32 velocityKc;
  quint32 positionKc;

  PIDKcParams() = default;
};

struct UWYValue
{
  qint32 U;
  qint32 V;
  qint32 W;

  UWYValue() = default;
};

struct PIDValue
{
  qint32 ref; //指令值
  qint32 fdb; //反馈值
  qint32 err; //误差值

  PIDValue() = default;
};

struct EncoderData
{
  qint32 single; //编码器单圈数据值
  qint32 multi;  //编码器多圈数据值
  qint32 clock;  //编码器采集数据时钟值

  EncoderData() = default;
};

#pragma pack(1) //取消结构体对其
struct MonitorData
{
  // PIDValue position; //位置
  //  PIDValue speed;          //速度
  //  PIDValue current;        //电流
  //  qint32 voltage;          //电压
  //  UWYValue UVWCurrent;     //三相电流
  //  EncoderData encoderData; //编码器数据

  quint16 faultCome; //错误代码
  // quint16 warnCode;  //警告代码

  MonitorData() = default;
};
#pragma pack(0)

struct CONData
{
  quint32 runMode; //运行模式，因为指针访问对其，只好用4个字节
  PIDParams currentPID;        //电流环PID 0 1 2
  PIDParams velocityPID;       //速度环PID 3 4 5
  PIDParams positionPID;       //位置环PID 6 7 8
  ADRCParams currentADRC;      //电流环ADRC 9 10 11
  ADRCParams velocityADRC;     //速度环ADRC 12 13 14
  FOPDGESOParams velocityFOPD; //速度环FOPD 15 16 17 18 19 20 21 22
  PIDKcParams PIDKc;           //三个环PID Kc 23 24 25

  CONData() = default;
};

struct RunConfigData
{
  quint16 m_sampleFreq;   //采样频率
  quint16 m_samplePoints; //采样点数

  explicit RunConfigData(int sampleFreq, int samplePoints)
    : m_sampleFreq(sampleFreq)
    , m_samplePoints(samplePoints)
  {}
};

class MonitorDataType
{
public:
  explicit MonitorDataType();
  ~MonitorDataType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  MonitorData& data() { return monitorData; }

private:
  MonitorData monitorData;
};

class CONDataType
{
public:
  explicit CONDataType();
  ~CONDataType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  CONData& data() { return controllerData; }
  qint32& at(size_t i);

private:
  CONData controllerData;
};

class RunConfigType
{
public:
  explicit RunConfigType(int sampleFreq, int samplePoints);
  ~RunConfigType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  RunConfigData& data() { return runConfigData; }

private:
  RunConfigData runConfigData;
};

} // namespace DriverDataType
#endif // DRIVERDATATYPE_H
