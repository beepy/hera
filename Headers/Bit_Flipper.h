/******************************************\|**| Bit_Flipper.h\******************************************/#ifndef kBit_Flipper_h#define kBit_Flipper_h#pragma mark ��INCLUDES��/******************************************\|**| #includes\******************************************/#pragma mark _system includes_/* - system*******************************************/#pragma mark _third party includes_/* - third party libraries*******************************************/#pragma mark _bp libraries includes_/* - bp libraries*******************************************/#pragma mark _this library includes_/* - this project*******************************************/#pragma mark ��DEFINITIONS��/******************************************\|**| #defines\******************************************//* - definitions*******************************************/#pragma mark _macros_/* - macros*******************************************/#pragma mark ��TYPEDEFS��/******************************************\|**| typedefs\******************************************/typedef void(*bitFlipper_BitSetProcPtrType)(long, unsigned long);#pragma mark ��PUBLIC PROTOTYPES��/******************************************\|**| public function prototypes\******************************************/OSErr BitFlipper_NewWindow( long *windowID, StringPtr title,	Handle bitNameStrList, unsigned long bits, long ownerID,	bitFlipper_BitSetProcPtrType setBitProc);#endif kBit_Flipper_h