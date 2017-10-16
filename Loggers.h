////////////////////////////////////////////////////////////////////////
//
// Loggers.h: header file
//
// Copyright (c) 2015, ShanKoDev Pty (Ltd). All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the owners written consent, and
// providing that this notice and the company name and all copyright
// notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// ShanKoDev accepts no liability for any damage/loss of business that
// this product may cause.
//
// See Kahless_9_user_license_agreement.txt located in framework root
// for the official terms and conditions of this license agreement.
//
// For use with Kahless_9
//
////////////////////////////////////////////////////////////////////////


#ifndef Loggers__h
#define Loggers__h



extern KLib::KLog Log;

void InitLoggers();
void DuoLog(const char * szFmt, ...);


#endif //Loggers__h
