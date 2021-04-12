/*
 * @Author: whj
 * @Date: 2021-02-01 09:49:35
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:02:55
 * @Description: file content
 */

#ifndef BITCONVERTER_H
#define BITCONVERTER_H

#include <QByteArray>
#include <array>
#include <cmath>
#include <cstdint>

/**
    @brief convert array2byte or byte2array
    @note
*/
namespace BitConverter {
double
ba2double(const QByteArray& ba);
int
ba2Int(const QByteArray& ba);

/**
 * @description:
 * @param {const T value}
 * @return {QByteArray}
 */
template<typename T>
QByteArray
num2Ba(const T value)
{
  QByteArray ba;
  // resize QByteArray for memcpy
  ba.resize(sizeof(value));
  memcpy_s(ba.data(), ba.size(), &value, sizeof(value));
  return ba;
}
} // namespace BitConverter
#endif // BITCONVERTER_H
