/******************************************\|**| Old_File.h\******************************************/#ifndef kOld_File_h#define kOld_File_h#pragma mark ��INCLUDES��/******************************************\|**| #includes\******************************************/#pragma mark _system includes_/* - system*******************************************/#include "StandardFile.h"#pragma mark _third party includes_/* - third party libraries*******************************************/#pragma mark _bp libraries includes_/* - bp libraries*******************************************/#pragma mark _this library includes_/* - this project*******************************************/#pragma mark ��DEFINITIONS��/******************************************\|**| #defines\******************************************//* - definitions*******************************************/#pragma mark _macros_/* - macros*******************************************/#pragma mark ��TYPEDEFS��/******************************************\|**| typedefs\******************************************/typedef struct{	TEHandle		editStrucHdl;	PicHandle		pictureHdl;	SInt16			fileRefNum;	FSSpec			fileFSSpec;	Boolean			windowTouched;}	docStructure, *docStructurePointer, **docStructureHandle;typedef StandardFileReply *standardFileReplyPtr;#pragma mark ��PUBLIC PROTOTYPES��/******************************************\|**| public function prototypes\******************************************/pascal Boolean  filterFunctionOpenDialog(CInfoPBPtr pbPtr,void *dataPtr);pascal SInt16  hookFunctionOpenDialog(SInt16 item,DialogPtr theDialog,	void *dataPtr);OSErr  doDirectorySelectionDialog( StandardFileReply *);pascal Boolean  filterFunctionDirSelect(CInfoPBPtr pbPtr,void *dataPtr);pascal SInt16  hookFunctionDirSelect(SInt16 item,DialogPtr theDialog,	void *dataPtr);#endif kOld_File_h