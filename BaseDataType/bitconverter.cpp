/*
 * @Author: whj
 * @Date: 2021-02-15 11:31:15
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:01:13
 * @Description: file content
 */
#include "BaseDataType/bitconverter.h"

namespace BitConverter {
/**
 * @description:  将字节数据转换成int(16 32)类型
 * @param ba int字节数据
 * @return 转换后的int
 */
auto
ba2Int(const QByteArray& ba) -> int
{
  Q_ASSERT_X(ba.size() == sizeof(qint16) || ba.size() == sizeof(qint32),
             "ba2Int",
             "not support int type or empty array"); // int16 int32 is ok
  int out = 0;
  if (ba.size() == 2) {
    qint16 tmp = 0;
    memcpy_s(&tmp, sizeof(tmp), ba.data(), ba.size());
    //    tmp |= static_cast<quint8>(ba[0]);
    //    tmp |= static_cast<quint8>(ba[1]) << 8;
    out = tmp;
  } else if (ba.size() == 4) {
    qint32 tmp = 0;
    memcpy_s(&tmp, sizeof(tmp), ba.data(), ba.size());
    //    tmp |= static_cast<quint8>(ba[0]);
    //    tmp |= static_cast<quint8>(ba[1]) << 8;
    //    tmp |= static_cast<quint8>(ba[2]) << 16;
    //    tmp |= static_cast<quint8>(ba[3]) << 24;
    out = tmp;
  }
  return out;
}

/**
 * @description: 将字节数据转换成float32类型
 * @param ba float字节数据
 * @return 转换后的float
 */
auto
ba2double(const QByteArray& ba) -> double
{
  Q_ASSERT_X(ba.size() == sizeof(float) || ba.size() == sizeof(double),
             "ba2double",
             "not support float type or empty array"); // float32 float64 is ok

  float outFloatVar = 0;
  memcpy_s(&outFloatVar, sizeof(outFloatVar), ba.data(), ba.size());
  return outFloatVar;
}
} // namespace BitConverter
