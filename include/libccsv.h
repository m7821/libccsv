/************************************************************
  Programmer : Spencer Yeh
  E-Mail     : m782123@gmail.com
*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifndef _LIB_C_CSV_H_
#define _LIB_C_CSV_H_

typedef void CSV_HANDLE;

typedef enum _ERROR_STATUS_LIBCCSV {
  ERROR_STATUS_SUCCESS,
  ERROR_STATUS_READ_FILE_ERROR,
  ERROR_STATUS_OUT_OF_MEMORY,
  ERROR_STATUS_FILE_ALREADY_EXIST,
  ERROR_STATUS_RANGE_EXCEED
} ERROR_STATUS_LIBCCSV;

ERROR_STATUS_LIBCCSV
csvWriteStr (
  CSV_HANDLE      *CsvHandle,
  char            *Str,
  int32_t         Row,
  int32_t         Col
  );

ERROR_STATUS_LIBCCSV
csvWriteVal (
  CSV_HANDLE      *CsvHandle,
  int32_t         Val,
  int32_t         Row,
  int32_t         Col
  );

const char *
csvReadStr (
  CSV_HANDLE      *CsvHandle,
  int32_t         Row,
  int32_t         Col
  );

int32_t
csvReadVal (
  CSV_HANDLE      *CsvHandle,
  int32_t         Row,
  int32_t         Col
  );

ERROR_STATUS_LIBCCSV
csvSaveCsvFile (
  CSV_HANDLE      *CsvHandle,
  _Bool           UseNewFileName
  );

CSV_HANDLE *
csvInitCsv (
  char               *FileName
  );

void
csvDestoryHandle (
  CSV_HANDLE         *CsvHandle
  );

CSV_HANDLE *
csvCreateCsvFile (
  char                *FileName
  );


#endif
