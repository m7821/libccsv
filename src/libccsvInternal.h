/************************************************************
  Programmer : Spencer Yeh
  E-Mail     : m782123@gmail.com
*************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libccsv.h>
#include <ctype.h>
#include <libcfileopt.h>

#ifndef _LIBCCSV_INTERNAL_H_
#define _LIBCCSV_INTERNAL_H_

#define ASCII_CHAR_NEW_LINE                  0xa
#define ASCII_CHAR_CARRIAGE_RETURN           0xd

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct _CSV_HANDLE_INTERNAL {
  char               *FileName;
  uint32_t           MaxColumn;
  uint32_t           MaxRow;
  uint8_t            ***Buffer;
} CSV_HANDLE_INTERNAL;

#endif