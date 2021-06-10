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

#include <QtGlobal>

namespace DriverDataType {
//超小常量
Q_CONSTEXPR double eps = 10e-8;

//浮点数-整型缩放因子 same as dsp
Q_CONSTEXPR double _IQ15 = 2 << 14;
Q_CONSTEXPR double _IQ14 = 2 << 13;
Q_CONSTEXPR double _IQ13 = 2 << 12;
Q_CONSTEXPR double _IQ12 = 2 << 11;
Q_CONSTEXPR double _IQ11 = 2 << 10;
Q_CONSTEXPR double _IQ10 = 2 << 9;
Q_CONSTEXPR double _IQ9 = 2 << 8;
Q_CONSTEXPR double _IQ8 = 2 << 7;

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
//幅值数据类型
typedef qint32 amplitudeDataType;
//相位数据类型
typedef qint16 phaseDataType;

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
  MODE7 = 7,

  MODE_FRT_MECH = 100,
  MODE_FRT_ELEC = 101,
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

#pragma pack(1) //取消结构体对齐
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

#pragma pack(1) //取消结构体对齐
struct FRTConfigData
{
  quint16 m_runMode;  //运行模式
  qint16 m_ref;       //参考值
  qint16 m_amplitude; //幅值
  quint16 m_minRange; //起始频率
  quint16 m_maxRange; //终止频率
  quint16 m_step;     //步长

  FRTConfigData() = default;
};
#pragma pack(0)

//与EtherCAT主站通讯的结构数据

struct server_sendData_t
{
  uint16_t statusWrod[6];
  uint8_t actualOperationMode[6];
  uint8_t alState;
  int32_t cia402ErrorCode;
  uint8_t journeyPercent; // percentage of PTP, Lin, Cir ......

  int32_t actualPosition[6];
  double jointAngles[6];
  double jointVelocity[6];
  double jointAcceleration[6];
  double jointTorque[6];
  double cartPosition[6];

  int32_t hyperson_fx;
  int32_t hyperson_fy;
  int32_t hyperson_fz;
  int32_t hyperson_mx;
  int32_t hyperson_my;
  int32_t hyperson_mz;
};

struct server_recvData_t
{
  uint8_t client_id;
  int32_t targetPosition[6];
  int32_t targetVelocity[6];
  int16_t targetTorque[6];
  uint8_t targetOperationMode[6];

  uint8_t motionCommandSize = 0;

  bool startSignal;
  bool stopSignal;
  bool enable;
  bool shutDown;
  bool faultReset;

  int8_t motionMode; // 0 manual_Joint_Space
                     // 1 manual_Cartesian_Space
                     // 2 continuous

  uint8_t velocityPercent;
  uint8_t accelerationPercent;

  int8_t jogButton; //  1   Axis1 +
                    //  -1  Axis1 -
                    //  2   Axis2 +
                    //  -2  Axis2 -
                    //  3   Axis3 +
                    //  4   Axis3 -

  //    Point endPoint;
  //    Point middlePoint;

  uint8_t ethercatStateMechine[6];
  int32_t ethercatErrorCode[6];
};

struct motionCommand
{
  uint8_t mode; // 0 -- PTP, 1 -- Lin, 2 -- Cir
  double PosMid[6];
  double PosFin[6];
  uint8_t velPercent;
  uint8_t accPercent;
};

//进一步封装struct

class MonitorDataType
{
public:
  explicit MonitorDataType();
  ~MonitorDataType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  MonitorData& data() { return m_monitorData; }

private:
  MonitorData m_monitorData;
};

class CONDataType
{
public:
  explicit CONDataType();
  ~CONDataType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  CONData& data() { return m_controllerData; }
  qint32& at(size_t i);

private:
  CONData m_controllerData;
};

class RunConfigType
{
public:
  explicit RunConfigType(int sampleFreq, int samplePoints);
  ~RunConfigType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  RunConfigData& data() { return m_runConfigData; }

private:
  RunConfigData m_runConfigData;
};

class FRTConfigType
{
public:
  explicit FRTConfigType();
  ~FRTConfigType() = default;

  QByteArray toByteArray();
  void byteArrayToStruct(const QByteArray& ba);
  void resetData();
  FRTConfigData& data() { return m_FRTConfigData; }

private:
  FRTConfigData m_FRTConfigData;
};

class ECATDataType
{
public:
  explicit ECATDataType();
  ~ECATDataType() = default;

  QByteArray toByteArray();
  void ba2Struct(const QByteArray& ba);
  void resetData();
  server_sendData_t& data() { return m_eCATData; }

private:
  server_sendData_t m_eCATData;
};

} // namespace DriverDataType
#endif // DRIVERDATATYPE_H
