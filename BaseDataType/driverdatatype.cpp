#include "BaseDataType/driverdatatype.h"

namespace DriverDataType {

// controller data type start
CONDataType::CONDataType()
  : controllerData()
{}

auto
CONDataType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&controllerData), sizeof(controllerData));
  return Ba;
}

void
CONDataType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&controllerData, sizeof(controllerData), ba.data(), ba.size());
}

void
CONDataType::resetData()
{
  memset(&controllerData, 0, sizeof(controllerData));
}

auto
CONDataType::at(size_t i) -> qint32&
{ //以指针形式访问结构体
  Q_ASSERT_X(i < sizeof(controllerData) / sizeof(qint32),
             "CtrllerDataParser at(size_t i)",
             "index out of array");
  auto* p = reinterpret_cast<qint32*>(&controllerData);
  return p[i];
}
// controller data type end

// monitor data type start
MonitorDataType::MonitorDataType()
  : monitorData()
{}

auto
MonitorDataType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&monitorData), sizeof(monitorData));
  return Ba;
}

void
MonitorDataType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&monitorData, sizeof(monitorData), ba.data(), sizeof(ba.data()));
}

void
MonitorDataType::resetData()
{
  memset(&monitorData, 0, sizeof(monitorData));
}
// monitor data type end

// run config datatype start
RunConfigType::RunConfigType(int sampleFreq, int samplePoints)
  : runConfigData(sampleFreq, samplePoints)
{}

auto
RunConfigType::toByteArray() -> QByteArray
{
  QByteArray Ba;
  Ba.append(reinterpret_cast<char*>(&runConfigData), sizeof(runConfigData));
  return Ba;
}

void
RunConfigType::byteArrayToStruct(const QByteArray& ba)
{
  memcpy_s(&runConfigData, sizeof(runConfigData), ba.data(), sizeof(ba.data()));
}

void
RunConfigType::resetData()
{
  memset(&runConfigData, 0, sizeof(runConfigData));
}
// run config data type end

} // namespace DriverDataType
