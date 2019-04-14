/******************************************\|**| Sprite_Importer.c\******************************************/#pragma mark ��INCLUDES��/******************************************\|**| #includes\******************************************/#pragma mark _system includes_/* - system*******************************************/#pragma mark _third party includes_/* - third party libraries*******************************************/#pragma mark _bp libraries includes_/* - bp libraries*******************************************/#include "Window Dispatch.h"#include "Control Utilities.h"#include "Hewey.h"#include "Hewey_Main.h"#include "Hewey_Layout.h"#include "Hewey_Utilities.h"#include "str_bp.h"#include "Set Font By String.h"#include "BP_Error.h"#include "Assert.h"#include "strlist.h"#include "Hewey_Helpers.h"#include "BP_Error.h"#include "Classic2Carbon_Helpers.h"#pragma mark _this library includes_/* - this project*******************************************/#include "Hera_Global.h"#include "Sprite_Importer.h"#include "Sprite_Selector.h"#include "NatePixTable.h"#include "Hera_Utilities.h"#include "Button_Window.h"#pragma mark ��DEFINITIONS��/******************************************\|**| #defines\******************************************//* - definitions*******************************************/#define	kEditFieldWidth					60#define	kControl_H_Space				8#define	kColNum							1#define	kRowNum							4#define	kMain_Cell_X					0#define	kMain_Cell_Y					1#define	kMain_ID_Offset					100#define	kMain_GridSize_Field_ID			(kMain_ID_Offset + 2)#define	kMain_ColumnNumber_Field_ID		(kMain_ID_Offset + 4)#define	kMain_TotalNumber_Field_ID		(kMain_ID_Offset + 6)#define	kPicture_Cell_X					0#define	kPicture_Cell_Y					0#define	kPicture_ID						300/*#define	kScroll_V_Cell_X				1#define	kScroll_V_Cell_Y				0#define	kScroll_V_ID					400#define	kScroll_H_Cell_X				0#define	kScroll_H_Cell_Y				1#define	kScroll_H_ID					500*/#define	kSeparator_Cell_X				0#define	kSeparator_Cell_Y				2#define	kSeparator_ID					200#define	kButton_Cell_X					0#define	kButton_Cell_Y					3#define	kOK_Button_ID					1#define	kCancel_Button_ID				2#define	kSmallFontSize					gHera->smallFontSize#define	kSmallFontNum					gHera->smallFontNum#define	kLargeFontNum					gHera->bigFontNum;#define kLargeFontSize					gHera->bigFontSize;#define	kUpdate_Grid_Time				60//#define	kScrollBarSize					16#define	kPictureVisibleWidth_Max		600#define	kPictureVisibleHeight_Max		400#define	kStrResID						8038#pragma mark _macros_/* - macros*******************************************/#pragma mark ��TYPEDEFS��/******************************************\|**| typedefs\******************************************/typedef struct Sprite_ImporterWindowType{	heraScenarioFileType				*heraFile;	huiMasterControlType				*master;	long								windowID;	PicHandle							sourcePict;	long								spriteWidth;	long								spriteHeight;	long								spriteColumn;	long								spriteTotal;	Sprite_Importer_Callback_procPtr	callBack;	long								ownerID;	long								lastUpdateTime;	double								pictScale;} Sprite_ImporterWindowType;#pragma mark ��EXTERNAL GLOBALS��/******************************************\|**| external globals\******************************************/extern heraGlobalType	*gHera;#pragma mark ��PRIVATE GLOBALS��/******************************************\|**| private globals\******************************************/#pragma mark ��PRIVATE PROTOTYPES��/******************************************\|**| private function prototypes\******************************************/#pragma mark ��PRIVATE FUNCTIONS��/******************************************\|**| private functions\******************************************/static Boolean HandleEvent( EventRecord *, CWindowPtr, long, long);static Boolean ProcessMenuChoiceCommand( unsigned long menuData,		CWindowPtr whichWindow, long dispatchID);static Boolean ProcessEvent( EventRecord *theEvent,	CWindowPtr whichWindow, long dispatchID);static void SourcePict_Draw( CWindowPtr window, Sprite_ImporterWindowType *d);static void OK_Button_From_WindowID( CWindowPtr whichWindow, long dispatchID);static void OK_Button( Sprite_ImporterWindowType *d);static void Cancel_Button( Sprite_ImporterWindowType *d);static void Revert_Button( Sprite_ImporterWindowType *d);static Handle AddSpritesFromPict( Sprite_ImporterWindowType *d);static unsigned char GetGWorldPixel( GWorldPtr gworld, short x, short y);//static pascal void HScroll_LiveScroll ( ControlHandle theControl,//	SInt16 partCode);//static pascal void VScroll_LiveScroll ( ControlHandle theControl,//	SInt16 partCode);#pragma mark ��PUBLIC FUNCTIONS��/******************************************\|**| public functions\******************************************/static Boolean HandleEvent( EventRecord *theEvent,	CWindowPtr whichWindow, long dispatchID, long command){	switch( command)	{		case kWDCloseWindowCommand:			break;				case kWDProcessEventCommand:			return( ProcessEvent( theEvent, whichWindow, dispatchID));			break;				case kWDSaveWindowCommand:			if (HU_ScenarioFiles_Equal( gHera->currentSave,				HU_HeraFile_Get_From_WindowID( dispatchID)))			{				if ( Button_Window_Modal( "\pSave changes to this sprite?",										"\pBefore saving, the Sprite Importer "										"must be closed. Click Cancel to "										"abort the save.", nil, nil))				{					OK_Button_From_WindowID( whichWindow, dispatchID);				} else				{					gHera->currentSave = nil;				}			}			return false;			break;		case kWD_Close_ForQuit_Command:			if (HU_ScenarioFiles_Equal( gHera->currentSave,				HU_HeraFile_Get_From_WindowID( dispatchID)))			{				if ( Button_Window_Modal( "\pSave changes to this sprite?",										"\pBefore quitting, the Sprite Importer "										"must be closed. Click Cancel to "										"abort the quit.", nil, nil))				{					OK_Button_From_WindowID( whichWindow, dispatchID);				} else				{					gHera->currentSave = nil;					return true;				}			}			return false;			break;		case kWDProcessMenuChoiceCommand:			return( ProcessMenuChoiceCommand( (unsigned long)theEvent,				whichWindow, dispatchID));			break;	}	return false;}static Boolean ProcessMenuChoiceCommand( unsigned long menuData,		CWindowPtr whichWindow, long dispatchID){	Handle							data = GetDispatchWindowDataHandle(																dispatchID);	SignedByte						dataState;	Sprite_ImporterWindowType			*d;	Boolean							result = false;	GrafPtr							oldPort;	short							menuID = ((menuData & 0xffff0000) >> 16),									menuItem = (menuData & 0x0000ffff);										if ( data == nil) return false;	dataState = HGetState( data);	HLock( data);	d = (Sprite_ImporterWindowType *)*data;	GetPort( &oldPort);	SetPortWindowPort( (WindowPtr)whichWindow);	result = Generic_ProcessMenuChoiceCommand( menuData, whichWindow,												dispatchID);	if ( result)	{		goto ProcessMenuChoiceCommand_Return;	}		switch( menuID)	{		case kEditMenuID:			switch( menuItem)			{				case kEdit_Undo_Item:					result = true;					break;				case kEdit_Add_Item:					result = true;					break;									case kEdit_Delete_Item:					result = true;					break;									case kEdit_Delete_All_Item:					result = true;					break;			}							break;		}	ProcessMenuChoiceCommand_Return:	SetPort( oldPort);	HSetState( data, dataState);	return( result);}static Boolean ProcessEvent( EventRecord *theEvent,	CWindowPtr whichWindow, long dispatchID){	Handle							data = GetDispatchWindowDataHandle( dispatchID);	SignedByte						dataState;	Boolean							result = false;	ControlHandle					aControl, oldControl;	short							whichPart, oldValue;	WindowPtr						scratchWindow;	Point							where;	char							whichChar;	GrafPtr							oldPort;	OSStatus						err;	Sprite_ImporterWindowType				*d;		if ( data == nil) return false;	if ( theEvent == nil) return false;		dataState = HGetState( data);	HLock( data);	d = (Sprite_ImporterWindowType *)*data;		if ( theEvent == nil) goto ProcessEvent_Return;		GetPort( &oldPort);	SetPortWindowPort( (WindowPtr)whichWindow);		switch( theEvent->what)	{		case nullEvent:			IdleControls( (WindowPtr)whichWindow);			{				long	thisTime = TickCount();								// if enough time has passed				if ((thisTime < d->lastUpdateTime) || ((thisTime -						d->lastUpdateTime) > kUpdate_Grid_Time))				{					long	spriteColumn =								HUI_Control_GetTextItemLong( d->master, 0,										kMain_Cell_X, kMain_Cell_Y,										kMain_ColumnNumber_Field_ID),							spriteTotal =								HUI_Control_GetTextItemLong( d->master, 0,										kMain_Cell_X, kMain_Cell_Y,										kMain_TotalNumber_Field_ID);					// check to see if grid should be updated					if (( d->spriteColumn != spriteColumn) ||						( d->spriteTotal != spriteTotal))					{						d->spriteColumn = spriteColumn;						d->spriteWidth =							((**(d->sourcePict)).picFrame.right -								(**(d->sourcePict)).picFrame.left) /							spriteColumn;												HUI_Control_SetTextItemLong( d->master, 0,										kMain_Cell_X, kMain_Cell_Y,										kMain_GridSize_Field_ID,										d->spriteWidth, true);						d->spriteHeight = d->spriteWidth;												d->spriteTotal = spriteTotal;												SourcePict_Draw( whichWindow, d);					}										d->lastUpdateTime = thisTime;						}			}						result = true;			break;					case updateEvt:				BeginUpdate ((WindowPtr)whichWindow);/*			SetPort( (WindowPtr)whichWindow);//			CalcVis( (WindowPtr)whichWindow);			UpdateControls( (WindowPtr)whichWindow, whichWindow->visRgn);*/			HU_GenericUpdateEventHandler( whichWindow);			SourcePict_Draw( whichWindow, d);			EndUpdate ((WindowPtr)whichWindow);			result = true;			break;		case activateEvt:		{			Boolean	isActive = ((theEvent->modifiers & activeFlag) != 0);						if ( isActive)			{				if ( d->heraFile->fileRefNum > 0)					UseResFile( d->heraFile->fileRefNum);				else					UseResFile( gHera->default_spriteFile_refNum);			}		}			oldValue = ((theEvent->modifiers & activeFlag) != 0);			if ( oldValue)			{				aControl = nil;				err = GetRootControl( (WindowPtr)whichWindow, &aControl);				if (( err == noErr) && ( aControl != nil))				{					ActivateControl( aControl);					SourcePict_Draw( whichWindow, d);					result = true;				}			}			break;		case mouseDown:			whichPart = FindWindow (theEvent->where, &scratchWindow);			switch( whichPart)			{				case inGrow:/*					{						Rect	growRect;						long	newSize;//						growRect = qd.screenBits.bounds;						C2C_ScreenBits_GetBounds( &growRect);						growRect.top = growRect.left = 120;						newSize = GrowWindow((WindowPtr)whichWindow,							theEvent->where ,&growRect);						if (newSize != 0)						{							SizeWindow( (WindowPtr)whichWindow, newSize & 0xffff,								(((newSize) >> 16) & 0xFFFF), true );							EraseRect(&whichWindow->portRect);							InvalRect( &whichWindow->portRect);						}					}*/					HU_GenericGrowWindow( whichWindow, theEvent->where);					result = true;					break;								case inContent:					where = theEvent->where;					scratchWindow = WD_FrontWindow();					if ( whichWindow != (CWindowPtr)scratchWindow)					{						goto ProcessEvent_Return;					}					SetPortWindowPort( (WindowPtr)whichWindow);					GlobalToLocal( &where);					whichPart = FindControl( where, (WindowPtr)whichWindow,						&aControl);					if ( aControl == nil)						goto ProcessEvent_Return;					oldValue = GetControlValue( aControl);					err = GetKeyboardFocus( (WindowPtr)whichWindow, &oldControl);					if (( err == noErr) && ( oldControl != aControl))					{						SetKeyboardFocus( (WindowPtr)whichWindow, aControl, whichPart);					}					if ( whichPart != 0)					{						huiPlainControlType	*hui = (huiPlainControlType	*)CU_GetControlReference( aControl);//						whichPart = HandleControlClick ( aControl, where,//							theEvent->modifiers, nil);							if ( !ListBox_Appearance_1_0_1_Hack( aControl, where,									&whichPart))						{							whichPart = HUI_Control_Click_Handle( hui, where,								theEvent->modifiers);														}						if ( whichPart != kControlNoPart) switch( hui->generic.id)						{							case kOK_Button_ID:								result = true;								OK_Button( d);								data = nil;								goto ProcessEvent_Return;								break;														case kCancel_Button_ID:								Cancel_Button( d);								data = nil;								result = true;								goto ProcessEvent_Return;								break;															default:								break;						}					} else // not in control					{						where = theEvent->where;						GlobalToLocal( &where);					}			}			break;					case keyDown:		case autoKey:			whichChar = theEvent->message & charCodeMask;			if ((whichChar=='\r' || whichChar==0x3))			{				// ok				long			soon = TickCount()+5;								aControl = HUI_Control_Get( d->master, 0, kButton_Cell_X,					kButton_Cell_Y, kOK_Button_ID);				if ( aControl != nil)				{					ActivateControl( aControl);					while ( TickCount() < soon){ /* do nothing */}					DeactivateControl( aControl);					result = true;					OK_Button( d);					data = nil;					goto ProcessEvent_Return;				}			} else if ((whichChar==0x1b) || ((whichChar=='.') && (theEvent->modifiers & cmdKey)))			{				// cancel				long			soon = TickCount()+5;								aControl = HUI_Control_Get( d->master, 0, kButton_Cell_X,					kButton_Cell_Y, kCancel_Button_ID);				if ( aControl != nil)				{					ActivateControl( aControl);					while ( TickCount() < soon){ /* do nothing */}					DeactivateControl( aControl);										Cancel_Button( d);					data = nil;					result = true;					goto ProcessEvent_Return;				}			} else if ( whichChar == '\t')			{				if ( theEvent->modifiers & shiftKey)					ReverseKeyboardFocus( (WindowPtr)whichWindow);				else					AdvanceKeyboardFocus( (WindowPtr)whichWindow);				result = true;				break;			}						err = GetKeyboardFocus( (WindowPtr)whichWindow, &aControl);			if (( err == noErr) && ( aControl != nil))			{				whichPart = HandleControlKey( aControl,					(theEvent->message & keyCodeMask)>>16,					theEvent->message & charCodeMask, theEvent->modifiers);				{				}			}			break;	}		ProcessEvent_Return:	SetPort( oldPort);	if ( data != nil)		HSetState( data, dataState);	return( result);	}void SourcePict_Draw( CWindowPtr window, Sprite_ImporterWindowType *d){	ControlHandle	aControl = HUI_Control_Get( d->master, 0, kPicture_Cell_X,						kPicture_Cell_Y, kPicture_ID);	RGBColor		c;	RgnHandle		originalClip, newClip;	long			j, i, k, w, h, totalPossible;	double			td;	Rect			r, controlBounds;	PicHandle		xPicture;	#pragma unused( d, window)		if ( aControl == nil) return;	if ( d->sourcePict == nil) return;		CU_GetControlBounds( aControl, &controlBounds);	DrawThemeGenericWell(&controlBounds, kThemeStateActive, false);	originalClip = NewRgn();	if ( originalClip == nil) return;	GetClip( originalClip);	newClip = NewRgn();	if ( newClip == nil) return;		RectRgn( newClip, &controlBounds);	SectRgn( originalClip, newClip, newClip);		SetClip( newClip);		DrawPicture( d->sourcePict, &controlBounds);		if (( d->spriteColumn > 0) && ( d->spriteWidth > 0) &&		( d->spriteHeight > 0) && ( d->spriteTotal > 0))	{				// white lines		c.red = c.green = c.blue = 65535;		RGBForeColor( &c);				for ( i = (**(d->sourcePict)).picFrame.left + d->spriteWidth;			i < (**(d->sourcePict)).picFrame.right; i += d->spriteWidth)		{			td = i - (**(d->sourcePict)).picFrame.left;			td *= d->pictScale;			td += controlBounds.left;			j = td;			MoveTo( j-1, controlBounds.top);			LineTo( j-1, controlBounds.bottom-1);			MoveTo( j+1, controlBounds.top);			LineTo( j+1, controlBounds.bottom-1);		}		for ( i = (**(d->sourcePict)).picFrame.top + d->spriteHeight;			i < (**(d->sourcePict)).picFrame.bottom; i += d->spriteHeight)		{			td = i - (**(d->sourcePict)).picFrame.top;			td *= d->pictScale;			td += controlBounds.top;			j = td;			MoveTo( controlBounds.left, j-1);			LineTo( controlBounds.right-1, j-1);			MoveTo( controlBounds.left, j+1);			LineTo( controlBounds.right-1, j+1);		}				// black lines		c.red = c.green = c.blue = 0;		RGBForeColor( &c);		for ( i = (**(d->sourcePict)).picFrame.left + d->spriteWidth;			i < (**(d->sourcePict)).picFrame.right; i += d->spriteWidth)		{			td = i - (**(d->sourcePict)).picFrame.left;			td *= d->pictScale;			td += controlBounds.left;			j = td;			MoveTo( j, controlBounds.top);			LineTo( j, controlBounds.bottom);		}		for ( i = (**(d->sourcePict)).picFrame.top + d->spriteHeight;			i < (**(d->sourcePict)).picFrame.bottom; i += d->spriteHeight)		{			td = i - (**(d->sourcePict)).picFrame.top;			td *= d->pictScale;			td += controlBounds.top;			j = td;			MoveTo( controlBounds.left, j);			LineTo( controlBounds.right, j);		}				// x out excluded cells				xPicture = GetPicture( 300);		if ( xPicture == nil) goto SourcePict_Draw_error;				k = d->spriteTotal;		totalPossible = (( (**(d->sourcePict)).picFrame.right -			(**(d->sourcePict)).picFrame.left) / d->spriteWidth) *			(( (**(d->sourcePict)).picFrame.bottom -			(**(d->sourcePict)).picFrame.top) / d->spriteHeight);		td = d->spriteWidth;		td *= d->pictScale;		w = td;		td = d->spriteHeight;		td *= d->pictScale;		h = td;		while ( k < totalPossible)		{			i = (k % d->spriteColumn);			j = (k / d->spriteColumn);			i = i * d->spriteWidth;			td = i;			td *= d->pictScale;			td += controlBounds.left;			i = td;			j *= d->spriteHeight;			td = j;			td *= d->pictScale;			td += controlBounds.top;			j = td;			SetRect( &r, i, j, i + w, j + h);			InsetRect( &r, 4, 4);			DrawPicture( xPicture, &r);			k++;		}		ReleaseResource( (Handle)xPicture);	}	SourcePict_Draw_error:	if ( originalClip != nil)	{		SetClip( originalClip);		DisposeRgn( originalClip);	}		if ( newClip != nil) DisposeRgn( newClip);}static void OK_Button_From_WindowID( CWindowPtr whichWindow, long dispatchID){	Handle							data = GetDispatchWindowDataHandle( dispatchID);	SignedByte						dataState;	Sprite_ImporterWindowType		*d;#pragma unused( whichWindow)	if ( data == nil) return;		dataState = HGetState( data);	HLock( data);	d = (Sprite_ImporterWindowType *)*data;	OK_Button( d);	// data has been deleted; window is closed}static void OK_Button( Sprite_ImporterWindowType *d){	gHera->modalMode--;	// get the values of the controls	d->spriteWidth = d->spriteHeight = 		HUI_Control_GetTextItemLong( d->master, 0,		kMain_Cell_X, kMain_Cell_Y, kMain_GridSize_Field_ID);	d->spriteColumn =		HUI_Control_GetTextItemLong( d->master, 0,		kMain_Cell_X, kMain_Cell_Y, kMain_ColumnNumber_Field_ID);	d->spriteTotal =		HUI_Control_GetTextItemLong( d->master, 0,		kMain_Cell_X, kMain_Cell_Y, kMain_TotalNumber_Field_ID);		if ( d->callBack != nil)		d->callBack( d->ownerID, //nil		AddSpritesFromPict( d)		);	HUI_MasterControl_Delete( d->master);	CloseDispatchWindow( d->windowID);}static void Cancel_Button( Sprite_ImporterWindowType *d){	gHera->modalMode--;	HUI_MasterControl_Delete( d->master);	CloseDispatchWindow( d->windowID);}static void Revert_Button( Sprite_ImporterWindowType *d){#pragma unused (d)}static Handle AddSpritesFromPict( Sprite_ImporterWindowType *d){	short			spriteColumn, spriteRow, frameNum, spriteNum, x, y;	Rect			bounds, size;	unsigned char	backgroundColor, thisPixel;	Point			center;	RGBColor		color;	long			indexColor;	GWorldPtr		pictWorld;	Handle			natePixTable = CreateNatePixTable();	CGrafPtr		originalWorld = nil;	GDHandle		originalDevice = nil;		if ( natePixTable == nil)	{		BP_UserError_Literal("Memory Error", "Couldn�t allocate memory for new pix table.");		return nil;	}		if ( d->sourcePict == nil)	{		BP_UserError_Literal("Picture Error", "Didn�t have source picture.");		goto AddSpritesFromPict_bail;	}	bounds.left = 0;	bounds.top = 0;	bounds.right = (**(d->sourcePict)).picFrame.right - (**(d->sourcePict)).picFrame.left;	bounds.bottom = (**(d->sourcePict)).picFrame.bottom - (**(d->sourcePict)).picFrame.top;		pictWorld = GWorld_Create( &bounds);		if ( pictWorld == nil)	{		BP_UserError_Literal("GWorld Error", "Couldn�t create the GWorld.");		goto AddSpritesFromPict_bail;	}		GetGWorld( &originalWorld, &originalDevice);	SetGWorld( pictWorld, nil);	DrawPicture( d->sourcePict, &bounds);		color.red = color.green = color.blue = 0;	RGBForeColor( &color);		frameNum = 0;	spriteRow = 0;	spriteColumn = 0;		while ( frameNum < d->spriteTotal)	{		if ( spriteColumn >= d->spriteColumn)		{			spriteColumn = 0;			spriteRow++;		}				// get the background color		x = spriteColumn * d->spriteWidth;		y = spriteRow * d->spriteHeight;		backgroundColor = GetGWorldPixel( pictWorld, x, y);				// get the leftmost pixel		bounds.left = x = y = 0;		while (( GetGWorldPixel( pictWorld, spriteColumn * d->spriteWidth + x,				spriteRow * d->spriteHeight + y) == backgroundColor) && ( x < d->spriteWidth))		{			y++;			if ( y >= d->spriteHeight)			{				y = 0;				x++;				bounds.left++;			}		}				// if this isn't a totally empty frame, find the edges		if ( x < d->spriteWidth)		{			// get the rightmost pixel			bounds.right = x = d->spriteWidth - 1;			y = 0;			while ( GetGWorldPixel( pictWorld, spriteColumn * d->spriteWidth + x,					spriteRow * d->spriteHeight + y) == backgroundColor)			{				y++;				if ( y >= d->spriteHeight)				{					y = 0;					x--;					bounds.right--;				}			}						// get the topmost pixel			bounds.top = x = y = 0;			while ( GetGWorldPixel( pictWorld, spriteColumn * d->spriteWidth + x,					spriteRow * d->spriteHeight + y) == backgroundColor)			{				x++;				if ( x >= d->spriteWidth)				{					x = 0;					y++;					bounds.top++;				}			}						// get the bottommost pixel			x = 0;			bounds.bottom = y = d->spriteHeight - 1;			while ( GetGWorldPixel( pictWorld, spriteColumn * d->spriteWidth + x,					spriteRow * d->spriteHeight + y) == backgroundColor)			{				x++;				if ( x >= d->spriteWidth)				{					x = 0;					y--;					bounds.bottom--;				}			}						bounds.right++;			bounds.bottom++;		} else // if it IS an empty frame, then make this sprite the size of the frame		{			bounds.left = 0;			bounds.right = d->spriteWidth;			bounds.top = 0;			bounds.bottom = d->spriteHeight;		}		// calc the size		size.left = size.top = 0;		size.right = bounds.right - bounds.left;		size.bottom = bounds.bottom - bounds.top;		size.right = (size.right + 3) / 4;		size.right *= 4;				// calc the center		center.h = (d->spriteWidth / 2) - bounds.left;		center.v = (d->spriteHeight / 2) - bounds.top;				// insert the sprite -- does not include pix data		spriteNum = GetNatePixTablePixNum( natePixTable);		HUnlock( natePixTable);		InsertNatePix( natePixTable, &size, spriteNum);		MoveHHi( natePixTable);		HLock( natePixTable);				// set the center		SetNatePixTableNatePixHRef( natePixTable, (long)spriteNum, center.h);		SetNatePixTableNatePixVRef( natePixTable, (long)spriteNum, center.v);				// clear all the sprite's pixels		size.right = GetNatePixTableNatePixWidth( natePixTable, spriteNum);		size.bottom = GetNatePixTableNatePixHeight( natePixTable, spriteNum);		for ( y = 0; y < size.bottom; y++)		{			for ( x = 0; x < size.right; x++)			{				SetNatePixTableNatePixDataPixel( natePixTable, spriteNum, x, y,												0x00);			}		}				// copy all the acutal pixels in the source pict to the sprite				for ( y = bounds.top; y < bounds.bottom; y++)		{			for ( x = bounds.left; x < bounds.right; x++)			{				thisPixel = GetGWorldPixel( pictWorld, spriteColumn * d->spriteWidth + x,											spriteRow * d->spriteHeight + y);								indexColor = thisPixel;				// if it's really, white, change it to light light gray				Index2Color( indexColor, &color);								if (( thisPixel == 0x00) || ((color.red > 57568) &&					(color.green > 57568) && ( color.blue > 57568)))					thisPixel = 0x02;//GetTranslateIndex( 0x02);//GetTranslateColorShade( GRAY, VERY_LIGHT);				else if ( thisPixel == backgroundColor)					thisPixel = 0x00;//				if ( thisPixel == backgroundColor)//					thisPixel = 0x00;//				else thisPixel = 0x30;				SetNatePixTableNatePixDataPixel( natePixTable, spriteNum, x -						bounds.left, y - bounds.top, thisPixel);//				MoveTo( spriteColumn * d->spriteWidth + x, spriteRow * d->spriteHeight + y);//				LineTo( spriteColumn * d->spriteWidth + x, spriteRow * d->spriteHeight + y);			}		}				spriteColumn++;		frameNum++;	}AddSpritesFromPict_bail://	if ( natePixTable != nil) DisposeHandle( natePixTable);	if ( originalWorld != nil) 	SetGWorld( originalWorld, originalDevice);	if ( pictWorld != nil) DisposeGWorld( pictWorld);	return natePixTable;	}static unsigned char GetGWorldPixel( GWorldPtr gworld, short x, short y){	PixMapHandle	pixMap = nil;	long			rowbytes;	unsigned char	*theByte = nil;		pixMap = GetGWorldPixMap( gworld);	if ( pixMap != nil)	{		rowbytes = (long)(*pixMap)->rowBytes;		rowbytes &= 0x00007fff;		theByte = (unsigned char *)(*pixMap)->baseAddr + (long)y * rowbytes + (long)x;				return( *theByte);	} else return ( 0x00);}/*// not needed -- no scroll bars; scaling preview image insteadstatic pascal void HScroll_LiveScroll ( ControlHandle theControl,	SInt16 partCode){}static pascal void VScroll_LiveScroll ( ControlHandle theControl,	SInt16 partCode){}*/#pragma mark ��PUBLIC FUNCTIONS��/******************************************\|**| public functions\******************************************/OSErr Sprite_Importer_NewWindow( heraScenarioFileType *heraFile,	StringPtr title, FSSpecPtr file,	long ownerID, Sprite_Importer_Callback_procPtr newSprite){	Rect						bounds;	CWindowPtr					newWindow;	ControlHandle				rootControl, aControl;	huiPlainControlParamType	buttonParam, textParam;	huiMasterControlParamType	masterParam;	OSStatus					err;	Handle						data, strList, picData = nil;	Sprite_ImporterWindowType		*d;	long						maxWidth = 0;	long						windowID = -1, fileLength, count;	huiCellParamType			cellParam;	short						refNum;	char						header[512];	OSErr						error;	double						td;	#pragma unused ( err, textParam, aControl)		// before anything, read in the pic	error = FSpOpenDF( file, fsRdPerm, &refNum);	if ( error == noErr)	{		error = GetEOF( refNum, &fileLength);		if ( error == noErr)		{			fileLength -= 512;	// picture header			count = 512;						error = FSRead( refNum, &count, header);						if ( error == noErr)			{				picData = NewHandle( fileLength);				if ( picData != nil)				{					MoveHHi( picData);					HLock( picData);										count = fileLength;					error = FSRead( refNum, &count, *picData);				}			}											}				FSClose( refNum);	}		// create the new window	data = NewHandle( sizeof( Sprite_ImporterWindowType));	if ( data == nil) return memFullErr;		HLock( data);	d = (Sprite_ImporterWindowType *)*data;		d->sourcePict = (PicHandle)picData;	d->ownerID = ownerID;	d->callBack = newSprite;	d->lastUpdateTime = 0;	d->spriteWidth = d->spriteHeight = d->spriteTotal = d->spriteColumn = 0;	d->pictScale = 1.0;	d->heraFile = heraFile;		SetRect( &bounds, 64, 64, 128, 128);	newWindow = NewDispatchWindow( &bounds, title, false,		kWindowMovableModalDialogProc, (WindowPtr)-1, false,		HandleEvent, 0,		&windowID);	SetDispatchWindowDataHandle( windowID, data);	d->windowID = windowID;		SetThemeWindowBackground( (WindowPtr)newWindow,		kThemeActiveDialogBackgroundBrush, true);	SetPortWindowPort( (WindowPtr)newWindow);	CreateRootControl( (WindowPtr)newWindow, &rootControl);	masterParam.control = &buttonParam;	masterParam.control->layer = 0;	masterParam.control->id = 0;	masterParam.control->flags = huiControlFlag_none;	SetRect( &masterParam.control->minMaxBounds, 0, 0, kHUI_Pixel_Max, kHUI_Pixel_Max);	masterParam.window = newWindow;	masterParam.rowNum = kRowNum;	masterParam.colNum = kColNum;	masterParam.layerNum = 1;	SetRect( &masterParam.inSpace, 6, 6, 6, 6);	SetRect( &masterParam.outSpace, 6, 6, 6, 6);	SetRect( &masterParam.controlSpace, 6, 6, 6, 6);	SetRect( &masterParam.minMaxBounds, 0, 0, kHUI_Pixel_Max, kHUI_Pixel_Max);	masterParam.baseLine = kSmallFontSize;	d->master = HUI_MasterControl_New( nil, 0, 0, 0, &masterParam);		mAssert( d->master != nil);	if ( d->master == nil) return memFullErr;	d->master->tab[0][0].h = 112;	d->master->tab[0][0].hAlign = hui_halign_right;	cellParam.master = d->master;	cellParam.baseLine = -1;	cellParam.hAlign = hui_halign_center;	cellParam.vAlign = hui_valign_top;	cellParam.flags = huiControlFlag_none;	SetRect( &cellParam.minMaxBounds, 0, 0, kHUI_Pixel_Max, kHUI_Pixel_Max);	SetRect( &cellParam.outSpace, -1, -1, -1, -1);	SetRect( &cellParam.inSpace, -1, -1, -1, -1);	cellParam.colSpan = 1;	cellParam.rowSpan = 1;	HUI_Cell_Set( 0, kMain_Cell_X, kMain_Cell_Y, &cellParam);	// set up picture cell	SetRect( &cellParam.outSpace, 2, 2, 2, -1);	SetRect( &cellParam.inSpace, 3, 3, 3, -1);	cellParam.colSpan = 1;	cellParam.rowSpan = 1;	HUI_Cell_Set( 0, kPicture_Cell_X, kPicture_Cell_Y, &cellParam);		// set up buttons, general	buttonParam.master = d->master;	buttonParam.visible = true;	buttonParam.enabled = true;	buttonParam.layer = 0;	buttonParam.whichTabSet = -1;	buttonParam.whichTab = -1;	buttonParam.minMaxBounds.left = 0;	buttonParam.minMaxBounds.top = 0;	buttonParam.minMaxBounds.right = kHUI_Pixel_Max;	buttonParam.minMaxBounds.bottom = kHUI_Pixel_Max;	buttonParam.space.left = 4;	buttonParam.space.top = 8;	buttonParam.space.bottom = 4;	buttonParam.fontStyle.flags = kControlUseFontMask | kControlUseSizeMask |		kControlUseFaceMask | kControlUseJustMask;	buttonParam.fontStyle.font = kLargeFontNum;	buttonParam.fontStyle.size = kLargeFontSize;	buttonParam.fontStyle.style = 0;	buttonParam.fontStyle.just = teCenter;	buttonParam.helpStringResID = -1;	buttonParam.enabledOnIndex = -1;	buttonParam.enabledOffIndex = -1;	buttonParam.disabledIndex = -1;	buttonParam.flags = huiControlFlag_none;	// set up text, general	textParam.master = d->master;	textParam.visible = true;	textParam.enabled = true;	textParam.layer = 0;	textParam.whichTabSet = -1;	textParam.whichTab = -1;	textParam.minMaxBounds.left = 0;	textParam.minMaxBounds.top = 0;	textParam.minMaxBounds.right = kHUI_Pixel_Max;	textParam.minMaxBounds.bottom = kHUI_Pixel_Max;	textParam.space.left = kControl_H_Space;//8;	textParam.space.top = 12;	textParam.space.bottom = 4;	textParam.flags = huiControlFlag_none;	textParam.fontStyle.flags = kControlUseFontMask | kControlUseSizeMask |		kControlUseFaceMask;// | kControlUseJustMask;	textParam.fontStyle.font = gHera->smallFontNum;	textParam.fontStyle.size = gHera->smallFontSize;	textParam.fontStyle.style = 0;	textParam.fontStyle.just = -1;	textParam.cellx = kMain_Cell_X;	textParam.celly = kMain_Cell_Y;	textParam.helpStringResID = -1;	textParam.enabledOnIndex = -1;	textParam.enabledOffIndex = -1;	textParam.disabledIndex = -1;		buttonParam.id = kPicture_ID;	buttonParam.flags = huiControlFlag_none;	buttonParam.cellx = kPicture_Cell_X;	buttonParam.celly = kPicture_Cell_Y;	if ( d->sourcePict != nil)	{		buttonParam.minMaxBounds.right = (**(d->sourcePict)).picFrame.right -			(**(d->sourcePict)).picFrame.left;		buttonParam.minMaxBounds.bottom = (**(d->sourcePict)).picFrame.bottom -			(**(d->sourcePict)).picFrame.top;		if ( buttonParam.minMaxBounds.right > kPictureVisibleWidth_Max)		{			d->pictScale = (double)kPictureVisibleWidth_Max;			d->pictScale /= (double)buttonParam.minMaxBounds.right;			buttonParam.minMaxBounds.right = kPictureVisibleWidth_Max;		}				td = buttonParam.minMaxBounds.bottom;		td *= d->pictScale;				if ( td > kPictureVisibleHeight_Max)		{			d->pictScale = (double)kPictureVisibleHeight_Max;			d->pictScale /= (double)buttonParam.minMaxBounds.bottom;			buttonParam.minMaxBounds.bottom = kPictureVisibleHeight_Max;			td =  (**(d->sourcePict)).picFrame.right -				(**(d->sourcePict)).picFrame.left;			td *= d->pictScale;			buttonParam.minMaxBounds.right = td;		} else		{			buttonParam.minMaxBounds.bottom = td;		}		buttonParam.space.left = 0;		buttonParam.space.top = 0;		buttonParam.space.bottom = 0;				HUI_UserPane_New_LiteralP( &buttonParam);	}	buttonParam.minMaxBounds.right = kHUI_Pixel_Max;	buttonParam.minMaxBounds.bottom = kHUI_Pixel_Max;	buttonParam.space.left = 4;	buttonParam.space.top = 8;	buttonParam.space.bottom = 4;	// grid size		textParam.cellx = buttonParam.cellx = kMain_Cell_X;	textParam.celly = buttonParam.celly = kMain_Cell_Y;		textParam.id = kMain_GridSize_Field_ID - 1;	textParam.flags = huiControlFlag_none;	GetIndString( textParam.label, kStrResID, 1);	textParam.minMaxBounds.left = 0;	HUI_StaticText_New_LiteralP( &textParam);		textParam.id = kMain_GridSize_Field_ID;	textParam.flags = huiControlFlag_none;	textParam.label[0] = 0;	textParam.minMaxBounds.left = kEditFieldWidth;	HUI_StaticText_New_LiteralP( &textParam);		// # of columns	textParam.id = kMain_ColumnNumber_Field_ID - 1;	textParam.flags = huiControlFlag_none;	GetIndString( textParam.label, kStrResID, 2);	textParam.minMaxBounds.left = 0;	HUI_StaticText_New_LiteralP( &textParam);		textParam.id = kMain_ColumnNumber_Field_ID;	textParam.flags = huiControlFlag_none;	textParam.label[0] = 0;	textParam.minMaxBounds.left = kEditFieldWidth;	HUI_EditText_New_LiteralP( &textParam);		textParam.id = kMain_TotalNumber_Field_ID - 1;	textParam.flags = huiControlFlag_none;	GetIndString( textParam.label, kStrResID, 3);	textParam.minMaxBounds.left = 0;	HUI_StaticText_New_LiteralP( &textParam);		textParam.id = kMain_TotalNumber_Field_ID;	textParam.flags = huiControlFlag_none;	textParam.label[0] = 0;	textParam.minMaxBounds.left = kEditFieldWidth;	HUI_EditText_New_LiteralP( &textParam);	/*	buttonParam.cellx = kScroll_V_Cell_X;	buttonParam.celly = kScroll_V_Cell_Y;	buttonParam.id = kScroll_V_ID;	buttonParam.special.liveScrollBar.liveScrollCallBack =		NewControlActionProc( VScroll_LiveScroll);	buttonParam.flags = huiControlFlag_useCellV;	buttonParam.minMaxBounds.left = buttonParam.minMaxBounds.right =		kScrollBarSize;		HUI_LiveScrollBar_New( &buttonParam);	buttonParam.minMaxBounds.left = 0;	buttonParam.minMaxBounds.right = kHUI_Pixel_Max;		buttonParam.cellx = kScroll_H_Cell_X;	buttonParam.celly = kScroll_H_Cell_Y;	buttonParam.id = kScroll_H_ID;	buttonParam.special.liveScrollBar.liveScrollCallBack =		NewControlActionProc( HScroll_LiveScroll);	buttonParam.flags = huiControlFlag_useCellH;	buttonParam.minMaxBounds.top = buttonParam.minMaxBounds.bottom =		kScrollBarSize;		HUI_LiveScrollBar_New( &buttonParam);	buttonParam.minMaxBounds.top = 0;	buttonParam.minMaxBounds.bottom = kHUI_Pixel_Max;*/	strList = GetStringList( 8041);	// cancel, create	if ( strList != nil)	{		HUI_Main_Buttons_H_Setup( d->master, 0,			kSeparator_Cell_X, kColNum, kSeparator_Cell_Y, 1,				true, strList);				ReleaseResource( strList);	}		ShowWindow( (WindowPtr)newWindow);	HUI_Master_Layout( d->master, (Point){0, 0}, (Point){0, 0});	HUI_Master_ShowLayer( d->master, 0);	gHera->modalMode++;		HUnlock( data);	return noErr;}