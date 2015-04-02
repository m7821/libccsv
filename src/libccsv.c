/************************************************************
  Programmer : Spencer Yeh
  E-Mail     : m782123@gmail.com
*************************************************************/

#include "libccsvInternal.h"

ERROR_STATUS_LIBCCSV  mErrorStatus = ERROR_STATUS_SUCCESS;

char *mErrorMsg[] = {
       "ERROR_STATUS_SUCCESS",
       "ERROR_STATUS_READ_FILE_ERROR",
       "ERROR_STATUS_OUT_OF_MEMORY",
       "ERROR_STATUS_FILE_ALREADY_EXIST",
       "ERROR_STATUS_RANGE_EXCEED"
       };

/*
 @ Name : csvGetErrorMessage ()


 @ Description : Show error message
 
*********************************************************************/
void
csvGetErrorMessage (
  )
{
  printf ("%s!\n", mErrorMsg[mErrorStatus]);
}

/*
 @ Name : csvRetrieveItemIfValid ()


 @ Description : Retrieve a item in the csv


 @ param *CsvFileBuf - Csv file buffer

 @ param *CsvFileIndex - Index of CsvFileBuf


 @ return ItemBuf - A item that in the csv.
 
*********************************************************************/
uint8_t *
csvRetrieveItemIfValid (
  uint8_t            *CsvFileBuf,
  int32_t            *CsvFileIndex
  )
{
  int32_t            Index = 0;
  char               TmpItemBuf[4096] = {0};
  uint8_t            *ItemBuf;

  if (CsvFileBuf[*CsvFileIndex] == ',') {

    //
    // skip empty item
    //
    (*CsvFileIndex)++;
    return NULL;
    
  } else if (CsvFileBuf[*CsvFileIndex] == ASCII_CHAR_NEW_LINE ||
             CsvFileBuf[*CsvFileIndex] == ASCII_CHAR_CARRIAGE_RETURN) {
    //        
    // skip new line
    //
    (*CsvFileIndex)+=2;
    return NULL;
    
  } else if (CsvFileBuf[*CsvFileIndex] == '"') {

    //
    // skip first "
    //
    TmpItemBuf[Index++] = CsvFileBuf[(*CsvFileIndex)++];

    //
    // put item to a buffer
    //
    while (CsvFileBuf[*CsvFileIndex] != '"')
      TmpItemBuf[Index++] = CsvFileBuf[(*CsvFileIndex)++];

    //
    // skip last "
    //
    TmpItemBuf[Index++] = CsvFileBuf[(*CsvFileIndex)++];
  } else {

    //
    // put normal items to buffer
    //
    while (CsvFileBuf[*CsvFileIndex] != ',') {
      if (CsvFileBuf[*CsvFileIndex] == ASCII_CHAR_NEW_LINE ||
          CsvFileBuf[*CsvFileIndex] == ASCII_CHAR_CARRIAGE_RETURN)
        break;
      TmpItemBuf[Index++] = CsvFileBuf[(*CsvFileIndex)++];
    }

  }

  //
  // skip , & new line
  //
  if (CsvFileBuf[*CsvFileIndex] == ',' )
    (*CsvFileIndex)++;
  else if (CsvFileBuf[*CsvFileIndex] == ASCII_CHAR_NEW_LINE ||
           CsvFileBuf[*CsvFileIndex] == ASCII_CHAR_CARRIAGE_RETURN)
    (*CsvFileIndex)+=2;
  
  ItemBuf = calloc (1, strlen (TmpItemBuf) + 1);
  strcpy (ItemBuf, TmpItemBuf);

  return ItemBuf;
}

/*
 @ Name : csvGetCsvRowCol ()


 @ Description : Read the csv file and retrieve the items in it to the
                 buffer.


 @ param *CsvFileBuf - Csv file buffer

 @ param CsvFileBufLen - Csv buffer length

 @ param *Rows - How many rows the csv have?

 @ param *Cols - How many cols the csv have?
 
*********************************************************************/
void
csvGetCsvRowCol (
  uint8_t            *CsvFileBuf,
  int32_t            CsvFileBufLen,
  int32_t            *Rows,
  int32_t            *Cols
  )
{
  int32_t            Index;

  if (CsvFileBufLen == 0) {
    *Rows = 0;
    *Cols = 0;

    return;
  }

  //
  // Count columns
  //
  Index = 0;
  while (!(CsvFileBuf[Index] == ASCII_CHAR_NEW_LINE ||
           CsvFileBuf[Index] == ASCII_CHAR_CARRIAGE_RETURN)) {

    if ((CsvFileBuf[Index] == ',' && CsvFileBuf[Index + 1] == '"') ||
        CsvFileBuf[Index] == '"') {

      //
      // skip "" string region
      //
      if (CsvFileBuf[Index] == '"')
        Index++;
      else
        Index+=2;
      
      while (CsvFileBuf[Index++] != '"');
      
    } else if (CsvFileBuf[Index] == ',') {

      //
      // skip value region
      //
      if (Index == 0)
        (*Cols)++;
      
      Index++;
    
    } else {

      //
      // skip value region
      //
      while (CsvFileBuf[Index] != ',') {
        if (CsvFileBuf[Index] == ASCII_CHAR_NEW_LINE ||
            CsvFileBuf[Index] == ASCII_CHAR_CARRIAGE_RETURN)
          break;
        
        Index++;
      }

      Index++;
    }

    (*Cols)++;
    
  }

  //
  // count rows
  //
  Index = 0;
  while (Index < CsvFileBufLen) {

    //
    // skip value and string
    //
    while (CsvFileBuf[Index] != ASCII_CHAR_NEW_LINE &&
           CsvFileBuf[Index] != ASCII_CHAR_CARRIAGE_RETURN)
      Index++;

    //
    // skip new line and carriage return
    //
    while (CsvFileBuf[Index] == ASCII_CHAR_NEW_LINE || 
           CsvFileBuf[Index] == ASCII_CHAR_CARRIAGE_RETURN) 
      Index++;

    (*Rows)++;
  }

}

/*
 @ Name : csvPutItemsIntoMemory ()


 @ Description : Read the csv file and retrieve the items in it to the
                 buffer.


 @ param *CsvFileBuf - Csv file buffer

 @ param CsvFileBufLen - Csv buffer length

 @ param *CsvInternal - Csv internal handle


 @ return ItemBuffer - sucess.
 
*********************************************************************/
uint8_t ***
csvPutItemsIntoMemory (
  uint8_t             *CsvFileBuf,
  int32_t             CsvFileBufLen,
  CSV_HANDLE_INTERNAL *CsvInternal
  )
{
  int32_t             CsvFileIndex = 0;
  int32_t             Index;
  int32_t             IndexB;
  uint8_t             ***ItemBuffer;

  if (CsvFileBufLen == 0)
    return calloc (sizeof (void *), 1);

  //
  // allocate memory space of rows
  //
  ItemBuffer = calloc (sizeof (void *), CsvInternal->MaxRow);

  //
  // allocate memory space of columns
  //
  for (Index = 0; Index < CsvInternal->MaxRow; Index++)
    ItemBuffer[Index] = calloc (sizeof (void *), CsvInternal->MaxColumn);

  //
  // put items to the buffer
  //
  for (Index = 0; Index < CsvInternal->MaxRow; Index++)
    for (IndexB = 0; IndexB < CsvInternal->MaxColumn; IndexB++)
      ItemBuffer[Index][IndexB] = csvRetrieveItemIfValid (CsvFileBuf, &CsvFileIndex);

  return ItemBuffer;
}

/*
 @ Name : csvFreeCsvBuffer ()


 @ Description : Free all allocated memory


 @ param *CsvInternal - csv internal handle
 
*********************************************************************/
void
csvFreeCsvBuffer (
  CSV_HANDLE_INTERNAL *CsvInternal
  )
{
  int32_t             Index;
  int32_t             IndexB;

  //
  // free all allocated buffer
  //
  for (Index = 0; Index < CsvInternal->MaxRow; Index++) {
    for (IndexB = 0; IndexB < CsvInternal->MaxColumn; IndexB++)
      free (CsvInternal->Buffer[Index][IndexB]);
    free (CsvInternal->Buffer[Index]);
  }

  free (CsvInternal->Buffer);
}

/*
 @ Name : csvDestoryHandle ()


 @ Description : Free all allocated memory


 @ param *CsvHandle - csv handle
 
*********************************************************************/
void
csvDestoryHandle (
  CSV_HANDLE         *CsvHandle
  )
{
  CSV_HANDLE_INTERNAL *CsvInternal;

  CsvInternal = (CSV_HANDLE_INTERNAL *) CsvHandle;

  csvFreeCsvBuffer (CsvInternal);

  free (CsvInternal->FileName);
}

/*
 @ Name : csvInitCsv ()


 @ Description : Collect information the csv file have and save them
                 to the handle


 @ param *FileName - The csv file name.


 @ return NULL - Error occured.

 @ return CsvInternal - sucess.
 
*********************************************************************/
CSV_HANDLE *
csvInitCsv (
  char               *FileName
  )
{
  int32_t             Status = 0;
  int32_t             CsvFileBufLen;
  uint8_t             *CsvFileBuf;
  CSV_HANDLE_INTERNAL *CsvInternal;
  int32_t  Index = 0;

  //
  // read file buffer
  //
  Status = ReadFileAndSizeByName (FileName, &CsvFileBufLen, &CsvFileBuf);
  if (Status) {
    mErrorStatus = ERROR_STATUS_READ_FILE_ERROR;
    return NULL;
  }

  CsvInternal = calloc (1, sizeof (CSV_HANDLE_INTERNAL));
  if (!CsvInternal) {
    mErrorStatus = ERROR_STATUS_OUT_OF_MEMORY;
    return NULL;
  }

  //
  // Take the file name except sub file name
  //
  while (strcasecmp (&FileName[Index++], ".csv"));
  CsvInternal->FileName = calloc (1, Index + 1);
  if (!CsvInternal->FileName) {
    mErrorStatus = ERROR_STATUS_OUT_OF_MEMORY;
    return NULL;
  }

  strncpy (CsvInternal->FileName, FileName, Index - 1);

  csvGetCsvRowCol (
    CsvFileBuf, 
    CsvFileBufLen,
    &CsvInternal->MaxRow,
    &CsvInternal->MaxColumn
    );

  CsvInternal->Buffer = csvPutItemsIntoMemory (
                          CsvFileBuf,
                          CsvFileBufLen,
                          CsvInternal
                          );

  return (CSV_HANDLE *) CsvInternal;
}

/*
 @ Name : csvWriteItemsToFile ()


 @ Description : Write strings in the buffer to file


 @ param NewCsvFile - The csv file the user want to save.

 @ param *CsvInternal - csv handle
 
*********************************************************************/
void
csvWriteItemsToFile (
  FILE                *NewCsvFile,
  CSV_HANDLE_INTERNAL *CsvInternal
  )
{
  int32_t             Index;
  int32_t             IndexB;
  int32_t             MaxColumn;

  MaxColumn = CsvInternal->MaxColumn - 1;

  //
  // write buffer to file
  //
  for (Index = 0; Index < CsvInternal->MaxRow; Index++)
    for (IndexB = 0; IndexB < CsvInternal->MaxColumn; IndexB++)
      fprintf (NewCsvFile, "%s%s", 
        CsvInternal->Buffer[Index][IndexB] == NULL ? "" : (char *) CsvInternal->Buffer[Index][IndexB],
        IndexB == MaxColumn ? "\n" : ","
        );

}

/*
 @ Name : csvSaveCsvFile ()


 @ Description : Save current buffer to csv file


 @ param *CsvHandle - csv handle

 @ param UseNewFileName - Create a new file to save or use current file.


 @ return Others - Error occured.

 @ return ERROR_STATUS_SUCCESS - sucess.
 
*********************************************************************/
ERROR_STATUS_LIBCCSV
csvSaveCsvFile (
  CSV_HANDLE      *CsvHandle,
  _Bool           UseNewFileName
  )
{
  CSV_HANDLE_INTERNAL *CsvInternal;
  FILE                *NewCsvFile;
  char                *NewFileName;

  CsvInternal = (CSV_HANDLE_INTERNAL *) CsvHandle;
  
  if (UseNewFileName) {
    NewFileName = calloc (1, strlen (CsvInternal->FileName) + strlen ("_edit.csv") + 1);
    sprintf (NewFileName, "%s_edit.csv", CsvInternal->FileName);
  
    NewCsvFile = fopen (NewFileName, "w+");
  } else {
    NewFileName = calloc (1, strlen (CsvInternal->FileName) + strlen (".csv") + 1);
    sprintf (NewFileName, "%s.csv", CsvInternal->FileName);
    
    NewCsvFile = fopen (NewFileName, "r+");
  }

  if (!NewCsvFile)
    return ERROR_STATUS_READ_FILE_ERROR;

  csvWriteItemsToFile (NewCsvFile, CsvInternal);

  free (NewFileName);

  fclose (NewCsvFile);
  
  return ERROR_STATUS_SUCCESS;
}

/*
 @ Name : csvExtendCurrentCsvBuffer ()


 @ Description : If the user want to write a string out of current range,
                 this function will try to extend the buffer size. Make 
                 the user's string can write to the position successfully.


 @ param *CsvInternal - csv internal handle

 @ param NewRow - The row the user wanna access.

 @ param NewCol - The column the user wanna access.


 @ return Others - Error occured.

 @ return ERROR_STATUS_SUCCESS - sucess.
 
*********************************************************************/
ERROR_STATUS_LIBCCSV
csvExtendCurrentCsvBuffer (
  CSV_HANDLE_INTERNAL *CsvInternal,
  int32_t             NewRow,
  int32_t             NewCol
  )
{
  int32_t             Index = 0;
  int32_t             IndexB = 0;
  int32_t             CurRow = 0;
  int32_t             CurCol = 0;
  uint8_t             ***CsvBuffer;
  _Bool               IsNewRow;
  _Bool               IsNewCol;

  CurRow    = CsvInternal->MaxRow;
  CurCol    = CsvInternal->MaxColumn;
  IsNewRow  = NewRow > CurRow;
  IsNewCol  = NewCol > CurCol;

  //
  // input rows & columns do not exeed the current value
  //
  if (!IsNewRow && !IsNewCol)
    return ERROR_STATUS_SUCCESS;

  CsvBuffer = CsvInternal->Buffer;

  if (!IsNewRow)
    NewRow = CurRow;

  if (!IsNewCol)
    NewCol = CurCol;

  //
  // reallocate the buffer that do not exist (rows)
  //
  CsvBuffer = realloc (CsvBuffer, sizeof (void *) * NewRow);
  if (!CsvBuffer) {
    mErrorStatus = ERROR_STATUS_OUT_OF_MEMORY;
    return mErrorStatus;
  }
  memset (&CsvBuffer[CurRow], 0, sizeof (void *) * (NewRow - CurRow));

  //
  // reallocate the buffer that do not exist (columns)
  //
  for (Index = 0; Index < NewRow; Index++) {
    if (CsvBuffer[Index] != NULL) {
      CsvBuffer[Index] = realloc (CsvBuffer[Index], sizeof (void *) * NewCol);
      if (!CsvBuffer[Index]) {
        mErrorStatus = ERROR_STATUS_OUT_OF_MEMORY;
        return mErrorStatus;
      }
      memset (&CsvBuffer[Index][CurCol], 0, sizeof (void *) * (NewCol - CurCol));
    } else {
      CsvBuffer[Index] = calloc (sizeof (void *), NewCol);
    }
  }

  //
  // Weiting new informations to handle
  //
  CsvInternal->Buffer    = CsvBuffer;
  CsvInternal->MaxRow    = NewRow;
  CsvInternal->MaxColumn = NewCol;

  return ERROR_STATUS_SUCCESS;
}

/*
 @ Name : csvCommonWrite ()


 @ Description : Write the user input string to the csv buffer


 @ param *CsvInternal - csv internal handle

 @ param *Str - The string the user wanna write.

 @ param Row - The row the user wanna access.

 @ param Col - The column the user wanna access.


 @ return Others - Error occured.

 @ return ERROR_STATUS_SUCCESS - sucess.
 
*********************************************************************/
ERROR_STATUS_LIBCCSV
csvCommonWrite (
  CSV_HANDLE_INTERNAL *CsvInternal,
  char                *Str,
  int32_t             Row,
  int32_t             Col
  )
{
  uint8_t             StrEmptySpace = 0;
  _Bool               NeedDoubleQuotation = FALSE;

  if (csvExtendCurrentCsvBuffer (CsvInternal, Row + 1, Col + 1))
    return mErrorStatus;

  //
  // check if the str has ","
  //
  if (strchr (Str, ','))
    NeedDoubleQuotation = TRUE;

  if (CsvInternal->Buffer[Row][Col] == NULL)
    CsvInternal->Buffer[Row][Col] = calloc (1, strlen (Str) + 1);

  if (NeedDoubleQuotation)
    StrEmptySpace = 3;
  else
    StrEmptySpace = 1;

  //
  // allocte new space if input string is longer than old one
  //
  if (strlen (CsvInternal->Buffer[Row][Col]) < strlen (Str)) {
    CsvInternal->Buffer[Row][Col] = realloc (CsvInternal->Buffer[Row][Col], strlen (Str) + StrEmptySpace);
    if (!CsvInternal->Buffer[Row][Col]) {
      mErrorStatus = ERROR_STATUS_OUT_OF_MEMORY;
      return mErrorStatus;
    }
    memset (CsvInternal->Buffer[Row][Col], 0, strlen (Str) + StrEmptySpace);
  }

  //
  // Write the user's string to buffer
  //
  sprintf (CsvInternal->Buffer[Row][Col], "%s%s%s",
    NeedDoubleQuotation ? "\"" : "",
    Str,
    NeedDoubleQuotation ? "\"" : ""
    );

  return ERROR_STATUS_SUCCESS;
}

/*
 @ Name : csvWriteStr ()


 @ Description : Write string to the csv file


 @ param *CsvHandle - csv handle

 @ param *Str - The string the user wanna write.

 @ param Row - The row the user wanna access.

 @ param Col - The column the user wanna access.


 @ return value - sucess.
 
*********************************************************************/
ERROR_STATUS_LIBCCSV
csvWriteStr (
  CSV_HANDLE      *CsvHandle,
  char            *Str,
  int32_t         Row,
  int32_t         Col
  )
{
  CSV_HANDLE_INTERNAL *CsvInternal;

  CsvInternal = (CSV_HANDLE_INTERNAL *) CsvHandle;

  csvCommonWrite (CsvInternal, Str, Row, Col);

  return ERROR_STATUS_SUCCESS;
}

/*
 @ Name : csvWriteVal ()


 @ Description : Write value to the csv file


 @ param *CsvHandle - csv handle

 @ param Val - The value the user wanna write.

 @ param Row - The row the user wanna access.

 @ param Col - The column the user wanna access.


 @ return value - sucess.
 
*********************************************************************/
ERROR_STATUS_LIBCCSV
csvWriteVal (
  CSV_HANDLE      *CsvHandle,
  int32_t         Val,
  int32_t         Row,
  int32_t         Col
  )
{
  CSV_HANDLE_INTERNAL *CsvInternal;
  char                ValStr[0xFF] = {0};

  CsvInternal = (CSV_HANDLE_INTERNAL *) CsvHandle;

  sprintf (ValStr, "%d", Val);

  csvCommonWrite (CsvInternal, ValStr, Row, Col);

  return ERROR_STATUS_SUCCESS;
}

/*
 @ Name : csvRangeCheck ()


 @ Description : Check if the user exeeds the range of current csv.


 @ param *CsvInternal - csv internal handle

 @ param Row - The row the user wanna access.

 @ param Col - The column the user wanna access.


 @ return ERROR_STATUS_RANGE_EXCEED - Out of range.

 @ return ERROR_STATUS_SUCCESS - sucess.
 
*********************************************************************/
ERROR_STATUS_LIBCCSV
csvRangeCheck (
  CSV_HANDLE_INTERNAL *CsvInternal,
  int32_t         Row,
  int32_t         Col
  )
{
  if (Row > CsvInternal->MaxRow || Col > CsvInternal->MaxColumn)
    mErrorStatus = ERROR_STATUS_RANGE_EXCEED;
  else
    mErrorStatus = ERROR_STATUS_SUCCESS;

  return mErrorStatus;
}

/*
 @ Name : csvReadStr ()


 @ Description : Read the string in the csv file


 @ param *CsvHandle - csv handle

 @ param Row - The row the user wanna access.

 @ param Col - The column the user wanna access.


 @ return NULL - Out of range.

 @ return string - sucess.
 
*********************************************************************/
const char *
csvReadStr (
  CSV_HANDLE      *CsvHandle,
  int32_t         Row,
  int32_t         Col
  )
{
  CSV_HANDLE_INTERNAL *CsvInternal;
  
  CsvInternal = (CSV_HANDLE_INTERNAL *) CsvHandle;

  if (csvRangeCheck (CsvInternal, Row, Col))
    return NULL;
  else
    return (const char *) CsvInternal->Buffer[Row][Col];
}

/*
 @ Name : csvReadVal ()


 @ Description : Read the value in the csv file


 @ param *CsvHandle - csv handle

 @ param Row - The row the user wanna access.

 @ param Col - The column the user wanna access.


 @ return 0 - Out of range.

 @ return value - sucess.
 
*********************************************************************/
int32_t
csvReadVal (
  CSV_HANDLE      *CsvHandle,
  int32_t         Row,
  int32_t         Col
  )
{
  CSV_HANDLE_INTERNAL *CsvInternal;
  
  CsvInternal = (CSV_HANDLE_INTERNAL *) CsvHandle;

  if (csvRangeCheck (CsvInternal, Row, Col))
    return 0;
  else
    return atoi (CsvInternal->Buffer[Row][Col]);
    
}

/*
 @ Name : csvCreateCsvFile ()


 @ Description : The function is help to create a new csv file and get
                 the handle back.


 @ param *FileName - A file name that the user want to create


 @ return NULL - The file has already exist.

 @ return CsvHandle - If there is no error.
 
*********************************************************************/
CSV_HANDLE *
csvCreateCsvFile (
  char                *FileName
  )
{
  CSV_HANDLE          *CsvHandle;
  CSV_HANDLE_INTERNAL *CsvInternal;
  uint16_t            FileNameLen;
  FILE                *NewFile = NULL;
  int32_t             FileSize = 0;
  uint8_t             *FileBuf;

  //
  // Try to open the file and check if it exists
  //
  NewFile = fopen (FileName, "r");
  if (NewFile) {
    mErrorStatus = ERROR_STATUS_FILE_ALREADY_EXIST;
    fclose (NewFile);
    return NULL;
  }
  
  NewFile = fopen (FileName, "w+b");
  fclose (NewFile);

  return csvInitCsv (FileName);
}
