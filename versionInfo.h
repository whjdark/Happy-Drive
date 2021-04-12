/*
 * @Author: whj
 * @Date: 2021-04-08 11:28:23
 * @LastEditors: whj
 * @LastEditTime: 2021-04-08 11:29:51
 * @Description: file content
 */

#ifndef VERSION_H
#define VERSION_H

#define MAIN_VERSION "protypeV4"
#define GIT_VERSION "0000"

#define SOFTWARE_NAME "Happy Driver"
#define COPYRIGHT "Copyright 2020-2021, whj"

#define BUILD_VERSION GIT_VERSION
#define SOFTWARE_VERSION MAIN_VERSION

#if QT_NO_DEBUG
#define SOFTWARE_TITLE (SOFTWARE_NAME " V" SOFTWARE_VERSION)
#else
#define SOFTWARE_TITLE                                                         \
  (SOFTWARE_NAME " V" SOFTWARE_VERSION " (Build " BUILD_VERSION "-Debug)")
#endif

#endif
