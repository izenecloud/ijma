/** \file iconv_utils.h
 * Definition of class Iconv.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jan 15, 2010
 */

#ifndef JMA_ICONV_UTILS_H
#define JMA_ICONV_UTILS_H

#ifdef HAVE_JMA_CONFIG_H
#include "jma_config.h"
#endif

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include "windows.h"
#endif

#include "knowledge.h" // Knowledge::EncodeType

#include <string>

namespace jma
{

class Iconv
{
 private:
#ifdef HAVE_ICONV
  iconv_t ic_;
#else
  int ic_;
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
  DWORD from_cp_;
  DWORD to_cp_;
#endif

 public:
  explicit Iconv();
  virtual ~Iconv();
  bool open(Knowledge::EncodeType from, Knowledge::EncodeType to);
  bool convert(std::string *);
};
}

#endif // JMA_ICONV_UTILS_H
