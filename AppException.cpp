////////////////////////////////////////////////////////////////////////
//
// AppException.cpp: source file
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

#include "stdafx.h"
#include "AppException.h"



const char * AppExceptionString[] = {
                                       "eApp_ShowUsage",
                                       "eApp_InvalidNumArgs",
                                       "eApp_InvalidInteger",
                                       "eApp_InvalidSwitch",
                                       "eApp_ValuesCannotBeEqual",
                                       "eApp_ItemNotFound",
                                       "eApp_FileCreateError",
                                       "eApp_FileNotFound",
                                       "eApp_FileOpenError",
                                       "eApp_FileReadError",
                                       "eApp_FileWriteError",
                                       "eApp_FileSeekError",
                                       "eApp_MemAllocError",
                                       "eApp_ObjCreationError",
                                       "eApp_GeneralError",
                                       "eApp_NothingSelected",
                                       "eApp_IdxRangeError",
                                       "eApp_PathNotFound",
                                       "eApp_ArgError"
                                    };

