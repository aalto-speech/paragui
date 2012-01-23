/************************************************************************
 * $Id: ychar.h,v 1.2 2012/04/27 18:19:57 senarvi Exp $
 *
 * ------------
 * Description:
 * ------------
 * ychar.h
 *
 * A unicode lightweight character class
 *
 * (C) Copyright 2004 Arabeyes, Mohammed Yousif
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2012/04/27 18:19:57 $
 *  $Author: senarvi $
 *  $Revision: 1.2 $
 *  $Source: /share/puhe/cvsroot/paragui/include/ychar.h,v $
 *
 *  (www.arabeyes.org - under GPL License)
 *
 ************************************************************************/

#ifndef YCHAR_H
#define YCHAR_H

#ifdef WIN32
#pragma warning(disable : 4290)
#endif

#include <stdexcept>
#include <string>

typedef unsigned int uint32;
typedef unsigned char byte;

class DECLSPEC YChar {
public:
  YChar();
  YChar(const uint32);
  YChar(const int);
  YChar(const char);
  YChar(const char *) throw(std::domain_error);
  YChar(const std::string) throw(std::domain_error);
  
  YChar lower() const;
  YChar upper() const;
  YChar title() const;
  int digitValue() const throw(std::domain_error);
  int hexDigitValue() const throw(std::domain_error);
  
  bool isNull() const;
  bool isAsciiLetter() const;
    
  int bytes() const;
  std::string utf8() const;
  
  static int getNumberOfContinuingOctents(byte)  throw(std::domain_error);
  static YChar fromUtf8(const char *) throw(std::domain_error);
  static YChar fromUtf8(const std::string) throw(std::domain_error);
  
  //operator char();
  operator uint32();
  
  friend bool operator==(const YChar &, const YChar &);
  friend bool operator==(const YChar &, const char &);
  friend bool operator==(const char &, const YChar &);
  friend bool operator==(const YChar &, const int &);
  friend bool operator==(const int &, const YChar &);
  friend std::ostream & operator<<( std::ostream &, YChar);

private:
    uint32 ucs4;
};

bool operator==(const YChar &, const YChar &);
bool operator==(const YChar &, const char &);
bool operator==(const char &, const YChar &);
bool operator==(const YChar &, const int &);
bool operator==(const int &, const YChar &);

std::ostream & operator<<( std::ostream &, YChar);
std::istream & operator>>( std::istream &, YChar &);

#endif /* YCHAR_H */
