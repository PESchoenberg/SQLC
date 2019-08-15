// ==============================================================
//                 ORBITER MODULE: DialogTemplate
//                  Part of the ORBITER SDK
//            Copyright (C) 2003 Martin Schweiger
//                   All rights reserved
//
// SQLC.h
//
// This module demonstrates how to build an Orbiter plugin which
// inserts a new MFD (multi-functional display) mode. The code
// is not very useful in itself, but it can be used as a starting
// point for your own MFD developments.
// ==============================================================
//	SQL Console
// Pablo Edronkin, 2017
//
// sqlc.h
//
// ==============================================================

#ifndef __SQLC_H
#define __SQLC_H

#include "stdafx.h"

class SQLC: public MFD2 
{
	public:

		SQLC (DWORD w, DWORD h, VESSEL *vessel);

		~SQLC ();

		bool ConsumeKeyBuffered (DWORD key);

		bool ConsumeButton (int bt, int event);

		char *ButtonLabel (int bt);

		int ButtonMenu (const MFDBUTTONMENU **menu) const;

		bool Update (oapi::Sketchpad *skp);

		static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

		bool DBSetInput (char *rstr);
		
		bool SQLSetInput (char *rstr);

		bool ModeOnSetInput (char *rstr);
		
		bool ModeOffSetInput (char *rstr);
		
		bool PrgOnSetInput (char *rstr);
		
		bool PrgOffSetInput (char *rstr);		
		
		bool ItemSetInput (char *rstr);
		
		bool ValueSetInput (char *rstr);

		bool DBInput (void *id, char *str, void *data);

		bool SQLInput (void *id, char *str, void *data);
		
		bool ModeOnInput (void *id, char *str, void *data);
		
		bool ModeOffInput (void *id, char *str, void *data);
		
		bool PrgOnInput (void *id, char *str, void *data);
		
		bool PrgOffInput (void *id, char *str, void *data);		
		
		bool ItemInput (void *id, char *str, void *data);
		
		bool ValueInput (void *id, char *str, void *data);

		//SQLC
		bool database_status;
		bool command_status;
		std::string sobject;
		std::string sitem;
		std::string svalue;
		std::string default_sobject;
		std::string default_sitem;
		std::string default_svalue;
		std::string database_to_open;
		std::string sql_sentence;
		sqlite3 *db;
		char *zErrMsg;
		int rc;
		//int num_bt;
	   const char* data;			
	
	protected:
		oapi::Font *font;
};

#endif // !__SQLC_H