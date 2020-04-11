// ==============================================================
//                 ORBITER MODULE: DialogTemplate
//                  Part of the ORBITER SDK
//          Copyright (C) 2003-2016 Martin Schweiger
//                   All rights reserved
//
// SQLC.cpp
//
// This module demonstrates how to build an Orbiter plugin which
// inserts a new MFD (multi-functional display) mode. The code
// is not very useful in itself, but it can be used as a starting
// point for your own MFD developments.
// ==============================================================
//	SQL Console
// Pablo Edronkin, 2017 - 2020.
//
// sqlc.cpp
//
// ==============================================================

#define STRICT
#define ORBITER_MODULE
#include "windows.h"
#include "orbitersdk.h"
#include "sqlc.h"

// ==============================================================
// Global variables.

// Identifier for new MFD mode.
int g_MFDmode; 

// ==============================================================
// API interface.
 
DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char *name = "SQL Console"; // MFD mode name.
	MFDMODESPECEX spec;
	spec.name = name;
	//spec.key = OAPI_KEY_T; // MFD mode selection key.
	spec.key = OAPI_KEY_Q;
	spec.context = NULL;
	spec.msgproc = SQLC::MsgProc; // MFD mode callback function.

	// Register the new MFD mode with Orbiter.
	g_MFDmode = oapiRegisterMFDMode (spec);
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded.
	oapiUnregisterMFDMode (g_MFDmode);
}

// ==============================================================
// SQLC Outside MFD class stuff.
// ==============================================================

//Vectors required to process sqlite query results.
std::vector<std::string> sql_results(0);

/**
Looks into a database file and brings results via the sql_results vector.
*/
static int sql_send_resq(void *data, int argc, char **argv, char **azColName)
{
   for(int i=0; i<argc; i++)
   {
      sql_results.push_back(argv[i] ? argv[i] : "na");
   }
   
   return 0;
}

// ==============================================================
// MFD class implementation.

/**
Constructor.
*/
SQLC::SQLC (DWORD w, DWORD h, VESSEL *vessel)
: MFD2 (w, h, vessel)
{
	font = oapiCreateFont (w/20, true, "Arial", FONT_NORMAL, 450);
	
	// Add MFD initialisation here.
	
	// Required for SQLite connections.
	data = "SQL query function called";
	database_to_open="SQLC";
	database_status=false;
	command_status=false;
	default_sobject="No DB selected.";
	default_sitem="test_fact";
	default_svalue="0.00";
	sobject=default_sobject;
	sitem=default_sitem;
	svalue=default_svalue;
	sql_sentence=" ";	
	//sqlite3 *db;
	zErrMsg = 0;			
}

/**
Destructor.
*/
SQLC::~SQLC ()
{
	oapiReleaseFont (font);
	// Add MFD cleanup code here.
	if (database_status == true)
	{
		sqlite3_close(db);
	}	
}

/**
Sets what to do according to the key pressed. Standard API function; see MFD API for details.
*/
bool SQLC::ConsumeKeyBuffered (DWORD key)
{
	bool DBInput (void *id, char *str, void *data);
	bool SQLInput (void *id, char *str, void *data);
	bool ModeOnInput (void *id, char *str, void *data);
	bool ModeOffInput (void *id, char *str, void *data);
	bool PrgOnInput (void *id, char *str, void *data);
	bool PrgOffInput (void *id, char *str, void *data);	
	bool ItemInput (void *id, char *str, void *data);
	bool ValueInput (void *id, char *str, void *data);

	switch (key) 
	{
		case OAPI_KEY_B:
			// Select database.			
			oapiOpenInputBox ("Database to connect to:", DBInput, 0, 20, (void*)this);		
			break;
		case OAPI_KEY_C:
			// Connect to selected database.
		  	rc = sqlite3_open(database_to_open.c_str(), &db);
			if(!rc)
			{
				/*Clean up*/			
				sql_results.erase(sql_results.begin(),sql_results.end());
				
				database_status=true;
			}	
			break;
		case OAPI_KEY_D:	
			// Disconnect from selected database.
			if(database_status == true)
			{
				/*Clean up*/			
				sql_results.erase(sql_results.begin(),sql_results.end());				
				
				sqlite3_close(db);
				database_status=false;
			}
			break;
		case OAPI_KEY_P:
			// Write SQL command.
			if(database_status == true)
			{
				oapiOpenInputBox ("SQL command:", SQLInput, 0, 100, (void*)this);			
			}
			break;
		case OAPI_KEY_M:
			// Mode on.
			if(database_status == true)
			{
				oapiOpenInputBox ("Mode to set ON:", ModeOnInput, 0, 20, (void*)this);			
			}
			break;
		case OAPI_KEY_N:
			// Mode off.
			if(database_status == true)
			{
				oapiOpenInputBox ("Mode to set OFF:", ModeOffInput, 0, 20, (void*)this);			
			}
			break;		
		case OAPI_KEY_O:
			// Mode on.
			if(database_status == true)
			{
				oapiOpenInputBox ("Prg to set ON:", PrgOnInput, 0, 20, (void*)this);			
			}
			break;
		case OAPI_KEY_L:
			// Mode off.
			if(database_status == true)
			{
				oapiOpenInputBox ("Prg to set OFF:", PrgOffInput, 0, 20, (void*)this);			
			}
			break;						
		case OAPI_KEY_I:
			//Set Item.		
			if(database_status == true)
			{
				oapiOpenInputBox ("Item:", ItemInput, 0, 20, (void*)this);			
			}		
			break;
		case OAPI_KEY_V:
			//Set Value.		
			if(database_status == true)
			{
				oapiOpenInputBox ("Value:", ValueInput, 0, 20, (void*)this);			
			}		
			break;						
		case OAPI_KEY_E:
			// Execute SQL command.
			if ( (database_status == true) && (command_status == true) )
			{
				//Send command.
				
				/*Clean up*/	
				sql_results.erase(sql_results.begin(),sql_results.end());

				/*Send the command. */
				sqlite3_exec(db, sql_sentence.c_str(), sql_send_resq, (void*)data, &zErrMsg);	

				/* Mark the command as sent.*/			
				command_status=false;
			}
			break;
		default:
			break;
	};
	return true;
}

/**
Enumerates keys used by the MFD. Standard API function; see MFD API for details.
*/
bool SQLC::ConsumeButton (int bt, int event)
{
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;
	static const DWORD btkey[NUM_BUTTONS] = { 
		OAPI_KEY_B, 
		OAPI_KEY_C, 
		OAPI_KEY_D, 
		OAPI_KEY_P,
		OAPI_KEY_M,
		OAPI_KEY_N,
		OAPI_KEY_O,
		OAPI_KEY_L, 		 
		OAPI_KEY_I,
		OAPI_KEY_V,
		OAPI_KEY_E};
	if (bt < NUM_BUTTONS) return ConsumeKeyBuffered (btkey[bt]);
	else return false;
}

/**
Return button labels.
*/
char *SQLC::ButtonLabel (int bt)
{
	// The labels for the buttons used by our MFD mode.
	static char *label[NUM_BUTTONS] = {"DB", "CON", "DIS", "SQL", "MON", "MOF", "PON", "POF", "ITM", "VAL", "EXE"};
	return (bt < NUM_BUTTONS ? label[bt] : 0);
}

/**
Return button menus.
*/
int SQLC::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the two buttons.
	static const MFDBUTTONMENU mnu[NUM_BUTTONS] = {
		{"Select DB", 0, 'b'},
		{"Connect to DB", 0, 'c'},
		{"Disconnect DB", 0, 'd'},				
		{"SQL Command", 0, 'p'},
		{"Mode to set on", 0, 'm'},
		{"Mode to set off", 0, 'n'},
		{"Prg to set on", 0, 'o'},
		{"Prg to set off", 0, 'l'},			
		{"Set Item", 0, 'i'},
		{"Set Value", 0, 'v'},			
		{"Execute", 0, 'e'}
	};
	
	if (menu)
	{ 
		*menu = mnu;
	}
	
	// Return the number of buttons used.
	return NUM_BUTTONS; 
}

/**
Repaint the MFD.
*/
bool SQLC::Update (oapi::Sketchpad *skp)
{	
	Title (skp, "SQL Console");

	skp->SetFont (font);
	skp->SetTextAlign (oapi::Sketchpad::CENTER, oapi::Sketchpad::BASELINE);
	skp->SetTextColor (0x00FFFF);
	//skp->Text (W/2, H/2,"Display area", 12);
	//skp->Rectangle (W/4, H/4, (3*W)/4, (3*H)/4);
	
	// Add MFD display routines here.
	// Use the device context (hDC) for Windows GDI paint functions.
	
	skp->Text(1,1,sobject.c_str(),sobject.length());

	return true;
}

/**
MFD message parser.
*/
int SQLC::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) 
	{
		case OAPI_MSG_MFD_OPENED:
			// Our new MFD mode has been selected, so we create the MFD and return a pointer to it.
			return (int)(new SQLC (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	
	return 0;
}

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters the name of the selected database.
*/
bool SQLC::DBSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{	
		result=true;
		if (user_input == "DGIIIAI2")
		{
			database_to_open="C:\\sources\\orbiter1\\Databases\\DGIIIAI\\DGIIIAI.db";
		}
		else if (user_input == "SQLC")
		{
			database_to_open="C:\\sources\\orbiter1\\Databases\\SQLC\\SQLC.db";
		}
		sobject=user_input;			
	}
	else
	{
		result=false;
		database_to_open="SQLC";
		sobject=default_sobject;
	}
	return result;
};

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters the name of the selected database.
*/
bool SQLC::SQLSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{
		result=true;
		command_status=true;
		sql_sentence=user_input;
	}
	else
	{
		result=false;
		command_status=false;
		sql_sentence=" ";
	}
	return result;
};

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters a mode to set on.
*/
bool SQLC::ModeOnSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{
		result=true;
		command_status=true;
		sql_sentence="UPDATE sde_facts SET Value=1 WHERE Item='mode_"+user_input+"'";
	}
	else
	{
		result=false;
		command_status=false;
		sql_sentence=" ";
	}
	return result;
};

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters a mode to set off.
*/
bool SQLC::ModeOffSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{
		result=true;
		command_status=true;
		sql_sentence="UPDATE sde_facts SET Value=0 WHERE Item='mode_"+user_input+"'";
	}
	else
	{
		result=false;
		command_status=false;
		sql_sentence=" ";
	}
	return result;
};

/////////////////////////////////////////////////////////

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters a program to set on.
*/
bool SQLC::PrgOnSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{
		result=true;
		command_status=true;
		sql_sentence="UPDATE sde_facts SET Value="+user_input+" WHERE Item='submode_prg_load'";
	}
	else
	{
		result=false;
		command_status=false;
		sql_sentence=" ";
	}
	return result;
};

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters a program to set off.
*/
bool SQLC::PrgOffSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{
		result=true;
		command_status=true;
		sql_sentence="UPDATE sde_facts SET Value="+user_input+" WHERE Item='submode_prg_end'";
	}
	else
	{
		result=false;
		command_status=false;
		sql_sentence=" ";
	}
	return result;
};

/////////////////////////////////////////////////////////

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters the Item field of an sde file.
*/
bool SQLC::ItemSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if (user_input.length()>0)
	{
		result=true;
		//command_status=true;
		command_status=false;
		sitem=user_input;
		sql_sentence="UPDATE sde_facts SET Value="+svalue+" WHERE Item='"+sitem+"'";
	}
	else
	{
		result=false;
		command_status=false;
		sitem=default_sitem;
		sql_sentence=" ";
	}
	return result;
};

/**
Required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details on this particular
aspect of this function); enters the Value field of an sde file.
*/
bool SQLC::ValueSetInput (char *rstr)
{
	bool result;

	std::string user_input=rstr;
	if ( (user_input.length()>0) && (svalue.length()>0) )
	{
		result=true;
		command_status=true;
		svalue=user_input;
		sql_sentence="UPDATE sde_facts SET Value="+svalue+" WHERE Item='"+sitem+"'";
	}
	else
	{
		result=false;
		command_status=false;
		svalue=default_svalue;
		sql_sentence=" ";
	}
	return result;
};

/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Selects database.
*/
bool DBInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->DBSetInput (str);
};

/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Enters a SQL instruction.
*/
bool SQLInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->SQLSetInput (str);
};

/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Accepts a mode name to set on.
*/
bool ModeOnInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->ModeOnSetInput (str);
};

/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Accepts a mode name to set off.
*/
bool ModeOffInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->ModeOffSetInput (str);
};

///////////////////////////////////////////////////////////////
/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Accepts a prg name to set on.
*/
bool PrgOnInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->PrgOnSetInput (str);
};

/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Accepts a prg name to set off.
*/
bool PrgOffInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->PrgOffSetInput (str);
};
///////////////////////////////////////////////////////////////
/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Input field name.
*/
bool ItemInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->ItemSetInput (str);
};

/**
Callback function required for user data input using oapiOpenInputBox (see the Orbiter SDK API for details 
on this particular aspect of this function). Input field name.
*/
bool ValueInput (void *id, char *str, void *data)
{
	return ((SQLC*)data)->ValueSetInput (str);
};
