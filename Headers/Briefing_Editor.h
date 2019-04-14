/******************************************\|**| Briefing_Editor.h\******************************************/#ifndef kBriefing_Editor_h#define kBriefing_Editor_h#pragma mark ��INCLUDES��/******************************************\|**| #includes\******************************************/#pragma mark _system includes_/* - system*******************************************/#pragma mark _third party includes_/* - third party libraries*******************************************/#pragma mark _bp libraries includes_/* - bp libraries*******************************************/#pragma mark _this library includes_/* - this project*******************************************/#include "Hera_Global.h"#pragma mark ��DEFINITIONS��/******************************************\|**| #defines\******************************************//* - definitions*******************************************/#pragma mark _macros_/* - macros*******************************************/#pragma mark ��TYPEDEFS��/******************************************\|**| typedefs\******************************************/typedef void(*Briefing_Editor_Callback_procPtr)(long, long, long);#pragma mark ��PUBLIC PROTOTYPES��/******************************************\|**| public function prototypes\******************************************/OSErr Briefing_Editor_NewWindow( heraScenarioFileType *heraFile, 	StringPtr title, short resRefNum,	long briefPointFirst, long briefPointNum, Handle discreteData,	Handle scenarioData, long scenarioIndex, long initialID,	long callerID,	long discreteDataID,	Briefing_Editor_Callback_procPtr callBack);#endif kBriefing_Editor_h