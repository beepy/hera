/******************************************\|**| IconSuiteFromAlias.h\******************************************/#ifndef kIconSuiteFromAlias_h#define kIconSuiteFromAlias_h#pragma mark ��INCLUDES��/******************************************\|**| #includes\******************************************/#pragma mark _system includes_/* - system*******************************************/#pragma mark _third party includes_/* - third party libraries*******************************************/#pragma mark _bp libraries includes_/* - bp libraries*******************************************/#pragma mark _this library includes_/* - this project*******************************************/#pragma mark ��DEFINITIONS��/******************************************\|**| #defines\******************************************//* - definitions*******************************************/#pragma mark _macros_/* - macros*******************************************/#pragma mark ��TYPEDEFS��/******************************************\|**| typedefs\******************************************/#pragma mark ��PUBLIC PROTOTYPES��/******************************************\|**| public function prototypes\******************************************/void DrawAliasSmallIcon( AliasHandle alias, short left, short right);void DrawFSpecSmallIcon( FSSpec *fspec, short left, short top);Handle GetIconSuiteFromAlias( AliasHandle alias);OSErr GetIconSuiteFromFSSpec(FSSpecPtr hfsObj, Handle *iconSuite);#endif kIconSuiteFromAlias_h