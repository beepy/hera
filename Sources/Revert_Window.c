/******************************************\|**| Revert_Window.c\******************************************/#pragma mark ��INCLUDES��/******************************************\|**| #includes\******************************************/#pragma mark _system includes_/* - system*******************************************/#pragma mark _third party includes_/* - third party libraries*******************************************/#pragma mark _bp libraries includes_/* - bp libraries*******************************************/#include "Window Dispatch.h"#include "Control Utilities.h"#include "Hewey.h"#include "Hewey_Main.h"#include "Hewey_Layout.h"#include "Hewey_Utilities.h"#include "str_bp.h"#include "Set Font By String.h"#include "BP_Error.h"#include "Assert.h"#include "strlist.h"#include "Hewey_Helpers.h"#include "Hera_Global.h"#pragma mark _this library includes_/* - this project*******************************************/#include "Revert_Window.h"#pragma mark ��DEFINITIONS��/******************************************\|**| #defines\******************************************//* - definitions*******************************************/#define	kColNum							1#define	kRowNum							3#define	kMain_Cell_X					0#define	kMain_Cell_Y					0#define	kMain_ID_Offset					100#define	kSeparator_Cell_X				0#define	kSeparator_Cell_Y				1#define	kSeparator_ID					200#define	kButton_Cell_X					0#define	kButton_Cell_Y					2#define	kCancel_Button_ID				1#define	kRevert_This_Button_ID			2#define	kRevert_All_Button_ID			3#define	kSmallFontSize					gHera->smallFontSize#define	kSmallFontNum					gHera->smallFontNum#define	kLargeFontNum					gHera->bigFontNum;#define kLargeFontSize					gHera->bigFontSize;#define	kStrResID						8000#pragma mark _macros_/* - macros*******************************************/#pragma mark ��TYPEDEFS��/******************************************\|**| typedefs\******************************************/typedef struct Revert_WindowWindowType{	huiMasterControlType			*master;	revertWindow_Callback_procPtr	callBack;	long							ownerWindowID;} Revert_WindowWindowType;#pragma mark ��EXTERNAL GLOBALS��/******************************************\|**| external globals\******************************************/extern heraGlobalType	*gHera;#pragma mark ��PRIVATE GLOBALS��/******************************************\|**| private globals\******************************************/#pragma mark ��PRIVATE PROTOTYPES��/******************************************\|**| private function prototypes\******************************************/#pragma mark ��PRIVATE FUNCTIONS��/******************************************\|**| private functions\******************************************/static Boolean HandleEvent( EventRecord *, CWindowPtr, long, long);static Boolean ProcessEvent( EventRecord *theEvent,	CWindowPtr whichWindow, long dispatchID);#pragma mark ��PUBLIC FUNCTIONS��/******************************************\|**| public functions\******************************************/static Boolean HandleEvent( EventRecord *theEvent,	CWindowPtr whichWindow, long dispatchID, long command){	switch( command)	{		case kWDCloseWindowCommand:			break;				case kWDProcessEventCommand:			return( ProcessEvent( theEvent, whichWindow, dispatchID));			break;				case kWDSaveWindowCommand:			return false;			break;	}	return false;}static Boolean ProcessEvent( EventRecord *theEvent,	CWindowPtr whichWindow, long dispatchID){	Handle							data = GetDispatchWindowDataHandle( dispatchID);	SignedByte						dataState;	Boolean							result = false;	ControlHandle					aControl, oldControl;	short							whichPart, oldValue;	WindowPtr						scratchWindow;	Point							where;	char							whichChar;	GrafPtr							oldPort;	OSStatus						err;	Revert_WindowWindowType				*d;		if ( data == nil) return false;	if ( theEvent == nil) return false;		dataState = HGetState( data);	HLock( data);	d = (Revert_WindowWindowType *)*data;		if ( theEvent == nil) goto ProcessEvent_Return;		GetPort( &oldPort);	SetPort( (WindowPtr)whichWindow);		switch( theEvent->what)	{		case nullEvent:			IdleControls( (WindowPtr)whichWindow);						result = true;			break;					case updateEvt:				BeginUpdate ((WindowPtr)whichWindow);			SetPort( (WindowPtr)whichWindow);			CalcVis( (WindowPtr)whichWindow);			UpdateControls( (WindowPtr)whichWindow, whichWindow->visRgn);			EndUpdate ((WindowPtr)whichWindow);			result = true;			break;		case mouseDown:			whichPart = FindWindow (theEvent->where, &scratchWindow);			switch( whichPart)			{				case inGrow:					{						Rect	growRect;						long	newSize;						growRect = qd.screenBits.bounds;						growRect.top = growRect.left = 120;						newSize = GrowWindow((WindowPtr)whichWindow,							theEvent->where ,&growRect);						if (newSize != 0)						{							SizeWindow( (WindowPtr)whichWindow, newSize & 0xffff,								(((newSize) >> 16) & 0xFFFF), true );							EraseRect(&whichWindow->portRect);							InvalRect( &whichWindow->portRect);						}					}					result = true;					break;								case inContent:					where = theEvent->where;					scratchWindow = FrontWindow();					if ( whichWindow != (CWindowPtr)scratchWindow)					{						goto ProcessEvent_Return;					}					SetPort( (WindowPtr)whichWindow);					GlobalToLocal( &where);					whichPart = FindControl( where, (WindowPtr)whichWindow,						&aControl);					oldValue = GetControlValue( aControl);					err = GetKeyboardFocus( (WindowPtr)whichWindow, &oldControl);					if (( err == noErr) && ( oldControl != aControl))					{						SetKeyboardFocus( (WindowPtr)whichWindow, aControl, whichPart);					}					if ( whichPart != 0)					{						huiPlainControlType	*hui = (huiPlainControlType	*)(**aControl).contrlRfCon;						whichPart = HandleControlClick ( aControl, where,							theEvent->modifiers, nil);							switch( hui->generic.id)						{														case kCancel_Button_ID:								HUI_MasterControl_Delete( d->master);								CloseDispatchWindow( dispatchID);								data = nil;								result = true;								gHera->modalMode = false;								goto ProcessEvent_Return;								break;														case kRevert_This_Button_ID:							case kRevert_All_Button_ID:								if ( d->callBack != nil)									d->callBack( hui->generic.id, 										d->ownerWindowID);								HUI_MasterControl_Delete( d->master);								CloseDispatchWindow( dispatchID);								data = nil;								result = true;								gHera->modalMode = false;								goto ProcessEvent_Return;								break;							default:								break;						}					} else // not in control					{						where = theEvent->where;						GlobalToLocal( &where);					}			}			break;					case keyDown:		case autoKey:			whichChar = theEvent->message & charCodeMask;			if ((whichChar=='\r' || whichChar==0x3) ||				(whichChar==0x1b) ||				((whichChar=='.') && (theEvent->modifiers & cmdKey)))			{				// ok ( really cancel)				long			soon = TickCount()+5;								aControl = HUI_Control_Get( d->master, 0, kButton_Cell_X,					kButton_Cell_Y, kCancel_Button_ID);				if ( aControl != nil)				{					HiliteControl( aControl, 1);					while ( TickCount() < soon){ /* do nothing */}					HiliteControl( aControl, 0);					result = true;					HUI_MasterControl_Delete( d->master);					CloseDispatchWindow( dispatchID);					data = nil;					gHera->modalMode = false;					goto ProcessEvent_Return;				}			} else if ( whichChar == '\t')			{				if ( theEvent->modifiers & shiftKey)					ReverseKeyboardFocus( (WindowPtr)whichWindow);				else					AdvanceKeyboardFocus( (WindowPtr)whichWindow);				result = true;				break;			}						err = GetKeyboardFocus( (WindowPtr)whichWindow, &aControl);			if (( err == noErr) && ( aControl != nil))			{				whichPart = HandleControlKey( aControl,					(theEvent->message & keyCodeMask)>>16,					theEvent->message & charCodeMask, theEvent->modifiers);				{				}			}			break;	}		ProcessEvent_Return:	SetPort( oldPort);	if ( data != nil)		HSetState( data, dataState);	return( result);	}#pragma mark ��PUBLIC FUNCTIONS��/******************************************\|**| public functions\******************************************/OSErr Revert_Window_NewWindow( StringPtr title, short buttonStrResID,	short textStrResID, revertWindow_Callback_procPtr callBack,		long ownerWindowID ){	Rect						bounds;	CWindowPtr					newWindow;	ControlHandle				rootControl, aControl;	huiPlainControlParamType	buttonParam, textParam;	huiMasterControlParamType	masterParam;	OSStatus					err;	Handle						data;	Revert_WindowWindowType		*d;	long						maxWidth = 0;	long						windowID = -1;	huiCellParamType			cellParam;	#pragma unused ( err, textParam, aControl)		if ( buttonStrResID <= 0) buttonStrResID = 8006;	if ( textStrResID <= 0) textStrResID = 8008;		// create the new window	data = NewHandle( sizeof( Revert_WindowWindowType));	if ( data == nil) return memFullErr;		HLock( data);	d = (Revert_WindowWindowType *)*data;	d->callBack = callBack;	d->ownerWindowID = ownerWindowID;		SetRect( &bounds, 64, 64, 128, 128);	newWindow = NewDispatchWindow( &bounds, title, false,		kWindowMovableModalDialogProc, (WindowPtr)-1, false,		HandleEvent, 0,		&windowID);	SetDispatchWindowDataHandle( windowID, data);		SetThemeWindowBackground( (WindowPtr)newWindow,		kThemeActiveDialogBackgroundBrush, true);	SetPort( (WindowPtr)newWindow);	CreateRootControl( (WindowPtr)newWindow, &rootControl);	masterParam.control = &buttonParam;	masterParam.control->layer = 0;	masterParam.control->id = 0;	masterParam.control->flags = huiControlFlag_none;	SetRect( &masterParam.control->minMaxBounds, 0, 0, kHUI_Pixel_Max, kHUI_Pixel_Max);	masterParam.window = newWindow;	masterParam.rowNum = kRowNum;	masterParam.colNum = kColNum;	masterParam.layerNum = 1;	SetRect( &masterParam.inSpace, 6, 6, 6, 6);	SetRect( &masterParam.outSpace, 6, 6, 6, 6);	SetRect( &masterParam.controlSpace, 6, 6, 6, 6);	SetRect( &masterParam.minMaxBounds, 0, 0, kHUI_Pixel_Max, kHUI_Pixel_Max);	masterParam.baseLine = kSmallFontSize;	d->master = HUI_MasterControl_New( nil, 0, 0, 0, &masterParam);		mAssert( d->master != nil);	if ( d->master == nil) return memFullErr;	d->master->tab[0][0].h = 112;	d->master->tab[0][0].hAlign = hui_halign_right;	cellParam.master = d->master;	cellParam.baseLine = -1;	cellParam.hAlign = hui_halign_center;	cellParam.vAlign = hui_valign_top;	cellParam.flags = huiControlFlag_none;	SetRect( &cellParam.minMaxBounds, 0, 0, kHUI_Pixel_Max, kHUI_Pixel_Max);	SetRect( &cellParam.outSpace, -1, -1, -1, -1);	SetRect( &cellParam.inSpace, -1, -1, -1, -1);	cellParam.colSpan = 1;	cellParam.rowSpan = 1;	HUI_Cell_Set( 0, kMain_Cell_X, kMain_Cell_Y, &cellParam);	// set up buttons, general	buttonParam.master = d->master;	buttonParam.visible = true;	buttonParam.enabled = true;	buttonParam.layer = 0;	buttonParam.whichTabSet = -1;	buttonParam.whichTab = -1;	buttonParam.minMaxBounds.left = 0;	buttonParam.minMaxBounds.top = 0;	buttonParam.minMaxBounds.right = kHUI_Pixel_Max;	buttonParam.minMaxBounds.bottom = kHUI_Pixel_Max;	buttonParam.space.left = 4;	buttonParam.space.top = 8;	buttonParam.space.bottom = 4;	buttonParam.fontStyle.flags = kControlUseFontMask | kControlUseSizeMask |		kControlUseFaceMask | kControlUseJustMask;	buttonParam.fontStyle.font = kLargeFontNum;	buttonParam.fontStyle.size = kLargeFontSize;	buttonParam.fontStyle.style = 0;	buttonParam.fontStyle.just = teCenter;	buttonParam.helpStringResID = -1;	buttonParam.enabledOnIndex = -1;	buttonParam.enabledOffIndex = -1;	buttonParam.disabledIndex = -1;	buttonParam.flags = huiControlFlag_none;		HUI_Main_Buttons_H_Setup( d->master, 0,		kSeparator_Cell_X, kColNum, kSeparator_Cell_Y, 1,			false, GetStringList( buttonStrResID));		buttonParam.cellx = kMain_Cell_X;	buttonParam.celly = kMain_Cell_Y;	buttonParam.id = 100;	GetIndString( buttonParam.label, textStrResID, 1);	HUI_StaticText_New_LiteralP( &buttonParam);			ShowWindow( (WindowPtr)newWindow);	HUI_Master_Layout( d->master, (Point){0, 0}, (Point){0, 0});	HUI_Master_ShowLayer( d->master, 0);		gHera->modalMode = true;		HUnlock( data);	return noErr;}