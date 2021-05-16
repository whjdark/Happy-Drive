#include "BaseDataType/driverdatatype.h"

namespace DriverDataType {

// controller data type start
CONDataType::CONDataType()
  : m_controllerData()
{}

auto
CONDataType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&m_controllerData),
            sizeof(m_controllerData));
  return Ba;
}

void
CONDataType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&m_controllerData, sizeof(m_controllerData), ba.data(), ba.size());
}

void
CONDataType::resetData()
{
  memset(&m_controllerData, 0, sizeof(m_controllerData));
}

auto
CONDataType::at(size_t i) -> qint32&
{ //以指针形式访问结构体
  Q_ASSERT_X(i < sizeof(m_controllerData) / sizeof(qint32),
             "CtrllerDataParser at(size_t i)",
             "index out of array");
  auto* p = reinterpret_cast<qint32*>(&m_controllerData);
  return p[i];
}
// controller data type end

// monitor data type start
MonitorDataType::MonitorDataType()
  : m_monitorData()
{}

auto
MonitorDataType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&m_monitorData), sizeof(m_monitorData));
  return Ba;
}

void
MonitorDataType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&m_monitorData, sizeof(m_monitorData), ba.data(), ba.size());
}

void
MonitorDataType::resetData()
{
  memset(&m_monitorData, 0, sizeof(m_monitorData));
}
// monitor data type end

// run config datatype start
RunConfigType::RunConfigType(int sampleFreq, int samplePoints)
  : m_runConfigData(sampleFreq, samplePoints)
{}

auto
RunConfigType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&m_runConfigData), sizeof(m_runConfigData));
  return Ba;
}

void
RunConfigType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&m_runConfigData, sizeof(m_runConfigData), ba.data(), ba.size());
}

void
RunConfigType::resetData()
{
  memset(&m_runConfigData, 0, sizeof(m_runConfigData));
}
// run config data type end

// Sweeping config datatype start
SweepConfigType::SweepConfigType()
  : m_sweepConfigData()
{}

auto
SweepConfigType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&m_sweepConfigData),
            sizeof(m_sweepConfigData));
  return Ba;
}

void
SweepConfigType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&m_sweepConfigData, sizeof(m_sweepConfigData), ba.data(), ba.size());
}

void
SweepConfigType::resetData()
{
  memset(&m_sweepConfigData, 0, sizeof(m_sweepConfigData));
}
// Sweeping config data type end

} // namespace DriverDataType
