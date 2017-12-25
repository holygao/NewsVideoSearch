//************************************************************************
//    Implement default error handling procedure.
//
//
//    Author  : Jialong HE
//    Date    : March 14, 1999
//************************************************************************
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "SV_Error.h"

static char SV_LibID[] = "Copyright (c) by Jialong He";
//==================================================
// This is the default Error Handler Function
//==================================================
void SV_DefaultHandler(int ErrorCode, char* ErrorMsg, char* FName, int LNum) {

   fprintf (stderr, "%s (%d) [%s:%d]\n", ErrorMsg, ErrorCode, FName, LNum);
   exit(ErrorCode);

}

//=============================================================
// Point to Default Error Handler Function
//
//  int (*Func)(float) : Func is a pointer to function varible.
//  
//  The pointed function takes a float varible and return an int
//
//============================================================
void (*FunPoint)(int ErrorCode, char* ErrorMsg, char* FName, int LName) = SV_DefaultHandler;


//==================================================
// User selected error handler
//==================================================
void SV_SetErrorHandler (void (*UserHandler)(int ErrorCode, char* ErrorMsg, char* FName, int LName) )  {

  FunPoint = UserHandler;

}

