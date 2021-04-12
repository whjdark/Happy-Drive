/*
 * @Author: whj
 * @Date: 2021-01-31 20:14:17
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:04:12
 * @Description: file content
 */

#include <QApplication>

#include "mainwindow.h"

#pragma GCC diagnostic ignored "-Wunknown-pragmas"

// void*
// operator new(size_t size)
//{
//  qDebug() << "SIZE: " << size << endl;
//  return malloc(size);
//}

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  // 设置界面风格
  a.setStyle(QStyleFactory::create(QStringLiteral("fusion")));
  //设置当前路径为exe文件下
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  //程序全局字体
  //  QFont font;
  //  font.setFamily("微软雅黑 Light");
  //  a.setFont(font);

  //实例化mainwindow
  MainWindow w;
  w.show();
  return a.exec();
}
