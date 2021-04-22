/*  
 *  Version: MPL 1.1
 *  
 *  The contents of this file are subject to the Mozilla Public License Version 
 *  1.1 (the "License"); you may not use this file except in compliance with 
 *  the License. You may obtain a copy of the License at 
 *  http://www.mozilla.org/MPL/
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 *  
 *  The Original Code is the sscanf 2.0 SA:MP plugin.
 *  
 *  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
 *  Portions created by the Initial Developer are Copyright (C) 2010
 *  the Initial Developer. All Rights Reserved.
 *  
 *  Contributor(s):
 *
 *  SA:MP team - plugin framework.
 *  
 *  Special Thanks to:
 *  
 *  SA:MP Team past, present and future
 */

#include <malloc.h>
#include <string.h>

#include "sscanf.h"
#include "specifiers.h"
#include "utils.h"
#include "data.h"
#include "array.h"
#include "enum.h"

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

//----------------------------------------------------------

logprintf_t
	logprintf,
	real_logprintf;

AMX_NATIVE
	SetPlayerName;

//GetServer_t
//	GetServer;

extern void *
	pAMXFunctions;

//extern int
//	g_iServerVersion;

extern unsigned int
	g_iTrueMax,
	g_iInvalid,
	g_iMaxPlayerName;

extern int *
	g_iConnected;

extern int *
	g_iNPC;

extern char *
	g_szPlayerNames;

extern int
	gOptions;

AMX *
	g_aCurAMX;

//extern char
//	** g_pServer;

#define SAVE_VALUE(m)       \
	if (doSave)             \
		amx_GetAddr(amx, params[paramPos++], &cptr), *cptr = m

#define SAVE_VALUE_F(m)     \
	if (doSave) {           \
		float f = (float)m; \
		amx_GetAddr(amx, params[paramPos++], &cptr), *cptr = amx_ftoc(f); }

// Based on amx_StrParam but using 0 length strings.  This can't be inline as
// it uses alloca - it could be written to use malloc instead, but that would
// require memory free code all over the place!
#define STR_PARAM(amx,param,result)                                                          \
	do {                                                                                     \
		cell * amx_cstr_; int amx_length_;                                                   \
		amx_GetAddr((amx), (param), &amx_cstr_);                                             \
		amx_StrLen(amx_cstr_, &amx_length_);                                                 \
		if (amx_length_ > 0) {                                                               \
			if (((result) = (char *)alloca((amx_length_ + 1) * sizeof (*(result)))) != NULL) \
				amx_GetString((result), amx_cstr_, sizeof (*(result)) > 1, amx_length_ + 1); \
			else {                                                                           \
				logprintf("sscanf error: Unable to allocate memory.");                       \
				return SSCANF_FAIL_RETURN; } }                                               \
		else (result) = ""; }                                                                \
	while (false)

// Macros for the regular values.
#define DO(m,n)                  \
	{m b;                        \
	if (Do##n(&string, &b)) {    \
		SAVE_VALUE((cell)b);     \
		break; }                 \
	RestoreOpts(defaultOpts);      \
	return SSCANF_FAIL_RETURN; }

#define DOV(m,n)                 \
	{m b;                        \
	Do##n(&string, &b);          \
	SAVE_VALUE((cell)b); }

#define DOF(m,n)                 \
	{m b;                        \
	if (Do##n(&string, &b)) {    \
		SAVE_VALUE_F(b)          \
		break; }                 \
	RestoreOpts(defaultOpts);      \
	return SSCANF_FAIL_RETURN; }

// Macros for the default values.  None of these have ifs as the return value
// of GetReturnDefault is always true - we don't penalise users for the
// mistakes of the coder - they will get warning messages if they get the
// format wrong, and I don't know of any mistkes which aren't warned about
// (admittedly a silly statement as if I did I would have fixed them).
#define DE(m,n)                  \
	{m b;                        \
	Do##n##D(&format, &b);       \
	SAVE_VALUE((cell)b);         \
	break; }

#define DEF(m,n)                 \
	{m b;                        \
	Do##n##D(&format, &b);       \
	SAVE_VALUE_F(b)              \
	break; }

// Macros for the default values in the middle of a string so you can do:
// 
// sscanf("hello, , 10", "p<,>sI(42)i", str, var0, var1);
// 
// Note that optional parameters in the middle of a string only work with
// explicit (i.e. not whitespace) delimiters.
#define DX(m,n)                  \
	if (IsDelimiter(*string)) {  \
		m b;                     \
		Do##n##D(&format, &b);   \
		SAVE_VALUE((cell)b);     \
		break; }                 \
	SkipDefault(&format);

#define DXF(m,n)                 \
	if (IsDelimiter(*string)) {  \
		m b;                     \
		Do##n##D(&format, &b);   \
		SAVE_VALUE_F(b)          \
		break; }                 \
	SkipDefault(&format);

bool
	DoK(AMX * amx, char ** defaults, char ** input, cell * cptr, bool optional);

void
	DoOptions(char *, cell);

// native sscanf(const data[], const format[], (Float,_}:...);
static cell AMX_NATIVE_CALL
	n_sscanf(AMX * amx, cell * params)
{
	if (g_iTrueMax == 0)
	{
		logprintf("sscanf error: System not initialised.");
		return SSCANF_FAIL_RETURN;
	}
	// Friendly note, the most complex set of specifier additions is:
	// 
	//  A<i>(10, 11)[5]
	// 
	// In that exact order - type, default, size.  It's very opposite to how
	// it's done in code, where you would do the eqivalent to:
	// 
	//  <i>[5] = {10, 11}
	// 
	// But this method is vastly simpler to parse in this context!  Technically
	// you can, due to legacy support for 'p', do:
	// 
	//  Ai(10, 11)[5]
	// 
	// But you will get an sscanf warning, and I may remove that ability from
	// the start - that will mean a new function, but an easy to write one.
	// In fact the most complex will probably be something like:
	// 
	//  E<ifs[32]s[8]d>(10, 12.3, Hello there, Hi, 42)
	// 
	// Get the number of parameters passed.  We add one as the indexes are out
	// by one (OBOE - Out By One Error) due to params[0] being the parameter
	// count, not an actual parameter.
	const int
		paramCount = ((int)params[0] / 4) + 1;
	// Could add a check for only 3 parameters here - I can't think of a time
	// when you would not want any return values at all, but that doesn't mean
	// they don't exist - you could just want to check but not save the format.
	// Update - that is now a possibility with the '{...}' specifiers.
	if (paramCount < (2 + 1))
	{
		logprintf("sscanf error: Missing required parameters.");
		return SSCANF_FAIL_RETURN;
	}
	//else if (paramCount == (2 + 1))
	//{
		// Only have an input and a specifier - better hope the whole specifier
		// is quite (i.e. enclosed in '{...}').
	//}
	// Set up function wide values.
	// Get and check the main data.
	// Pointer to the current input data.
	char *
		string;
	STR_PARAM(amx, params[1], string);
	// Pointer to the current format specifier.
	char *
		format;
	STR_PARAM(amx, params[2], format);
	// Check for CallRemoteFunction style null strings and correct.
	if (string[0] == '\1' && string[1] == '\0')
	{
		string[0] = '\0';
	}
	// Save the default options so we can have local modifications.
	int
		defaultOpts = gOptions;
	// Current parameter to save data to.
	int
		paramPos = 3;
	cell *
		cptr;
	InitialiseDelimiter();
	// Skip leading space.
	SkipWhitespace(&string);
	bool
		doSave;
	// Code for the rare cases where the WHOLE format is quiet.
	if (*format == '{')
	{
		++format;
		doSave = false;
	}
	else
	{
		doSave = true;
	}
	g_aCurAMX = amx;
	// Now do the main loop as long as there are variables to store the data in
	// and input string remaining to get the data from.
	while (*string && (paramPos < paramCount || !doSave))
	{
		if (!*format)
		{
			// End of the format string - if we're here we've got all the
			// parameters but there is extra string or variables, which may
			// indicate their code needs fixing, for example:
			// sscanf(data, "ii", var0, var1, var3, var4);
			// There is only two format specifiers, but four returns.  This may
			// also be reached if there is too much input data, but that is
			// considered OK as that is likely a user's fault.
			if (paramPos < paramCount)
			{
				logprintf("sscanf warning: Format specifier does not match parameter count.");
			}
			if (!doSave)
			{
				// Started a quiet section but never explicitly ended it.
				logprintf("sscanf warning: Unclosed quiet section.");
			}
			RestoreOpts(defaultOpts);
			return SSCANF_TRUE_RETURN;
		}
		else if (IsWhitespace(*format))
		{
			++format;
		}
		else
		{
			switch (*format++)
			{
				case 'L':
					DX(bool, L)
					// FALLTHROUGH
				case 'l':
					DOV(bool, L)
					break;
				case 'B':
					DX(int, B)
					// FALLTHROUGH
				case 'b':
					DO(int, B)
				case 'N':
					DX(int, N)
					// FALLTHROUGH
				case 'n':
					DO(int, N)
				case 'C':
					DX(char, C)
					// FALLTHROUGH
				case 'c':
					DO(char, C)
				case 'I':
				case 'D':
					DX(int, I)
					// FALLTHROUGH
				case 'i':
				case 'd':
					DO(int, I)
				case 'H':
				case 'X':
					DX(int, H)
					// FALLTHROUGH
				case 'h':
				case 'x':
					DO(int, H)
				case 'O':
					DX(int, O)
					// FALLTHROUGH
				case 'o':
					DO(int, O)
				case 'F':
					DXF(double, F)
					// FALLTHROUGH
				case 'f':
					DOF(double, F)
				case 'G':
					DXF(double, G)
					// FALLTHROUGH
				case 'g':
					DOF(double, G)
				case '{':
					if (doSave)
					{
						doSave = false;
					}
					else
					{
						// Already in a quiet section.
						logprintf("sscanf warning: Can't have nestled quiet sections.");
					}
					continue;
				case '}':
					if (doSave)
					{
						logprintf("sscanf warning: Not in a quiet section.");
					}
					else
					{
						doSave = true;
					}
					continue;
				case 'P':
					{
						ResetDelimiter();
						char *
							t = GetMultiType(&format);
						if (t) AddDelimiters(t);
						else return SSCANF_FAIL_RETURN;
						continue;
					}
					// FALLTHROUGH
				case 'p':
					// 'P' doesn't exist.
					// Theoretically, for compatibility, this should be:
					// p<delimiter>, but that will break backwards
					// compatibility with anyone doing "p<" to use '<' as a
					// delimiter (doesn't matter how rare that may be).  Also,
					// writing deprecation code and both the new and old code
					// is more trouble than it's worth, and it's slow.
					// UPDATE: I wrote the "GetSingleType" code for 'a' and
					// figured out a way to support legacy and new code, while
					// still maintaining support for the legacy "p<" separator,
					// so here it is:
					ResetDelimiter();
					AddDelimiter(GetSingleType(&format));
					continue;
				case 'Z':
					logprintf("sscanf warning: 'Z' doesn't exist - that would be an optional, deprecated optional string!.");
					// FALLTHROUGH
				case 'z':
					logprintf("sscanf warning: 'z' is deprecated, consider using 'S' instead.");
					// FALLTHROUGH
				case 'S':
					if (IsDelimiter(*string))
					{
						char *
							dest;
						int
							length;
						if (DoSD(&format, &dest, &length))
						{
							// Send the string to PAWN.
							if (doSave)
							{
								amx_GetAddr(amx, params[paramPos++], &cptr);
								amx_SetString(cptr, dest, 0, 0, length);
							}
						}
						break;
					}
					// Implicit "else".
					SkipDefaultEx(&format);
					// FALLTHROUGH
				case 's':
					{
						// Get the length.
						int
							length = GetLength(&format, false);
						char *
							dest;
						DoS(&string, &dest, length, IsEnd(*format) || (!doSave && *format == '}' && IsEnd(*(format + 1))));
						// Send the string to PAWN.
						if (doSave)
						{
							amx_GetAddr(amx, params[paramPos++], &cptr);
							amx_SetString(cptr, dest, 0, 0, length);
						}
					}
					break;
				case 'U':
					if (IsDelimiter(*string))
					{
						int
							b;
						DoUD(&format, &b);
						if (*format == '[')
						{
							int
								len = GetLength(&format, true);
							if (gOptions & 1)
							{
								// Incompatible combination.
								logprintf("sscanf error: 'U(name)[len]' is incompatible with OLD_DEFAULT_NAME.");
								return SSCANF_FAIL_RETURN;
							}
							else if (len < 2)
							{
								logprintf("sscanf error: 'U(num)[len]' length under 2.");
								return SSCANF_FAIL_RETURN;
							}
							else if (doSave)
							{
								amx_GetAddr(amx, params[paramPos++], &cptr);
								*cptr++ = b;
								*cptr = g_iInvalid;
							}
						}
						else
						{
							SAVE_VALUE((cell)b);
						}
						break;
					}
					SkipDefault(&format);
					// FALLTHROUGH
				case 'u':
					if (*format == '[')
					{
						int
							len = GetLength(&format, true);
						if (len < 2)
						{
							logprintf("sscanf error: 'u[len]' length under 2.");
							return SSCANF_FAIL_RETURN;
						}
						else
						{
							int
								b = -1,
								od = gOptions;
							if (doSave)
							{
								char *
									tstr;
								// Don't detect multiple results.
								gOptions &= ~4;
								amx_GetAddr(amx, params[paramPos++], &cptr);
								while (--len)
								{
									tstr = string;
									if (!DoU(&tstr, &b, b + 1))
									{
										*cptr++ = b;
										b = g_iInvalid;
										break;
									}
									if (b == g_iInvalid) break;
									*cptr++ = b;
								}
								if (b == g_iInvalid)
								{
									*cptr = g_iInvalid;
								}
								else
								{
									tstr = string;
									DoU(&tstr, &b, b + 1);
									if (b == g_iInvalid) *cptr = g_iInvalid;
									else *cptr = 0x80000000;
								}
								// Restore results detection.
								gOptions = od;
								string = tstr;
							}
							else
							{
								DoU(&string, &b, 0);
							}
						}
					}
					else
					{
						int
							b;
						DoU(&string, &b, 0);
						SAVE_VALUE((cell)b);
					}
					break;
				case 'Q':
					if (IsDelimiter(*string))
					{
						int
							b;
						DoQD(&format, &b);
						if (*format == '[')
						{
							int
								len = GetLength(&format, true);
							if (gOptions & 1)
							{
								// Incompatible combination.
								logprintf("sscanf error: 'Q(name)[len]' is incompatible with OLD_DEFAULT_NAME.");
								return SSCANF_FAIL_RETURN;
							}
							else if (len < 2)
							{
								logprintf("sscanf error: 'Q(num)[len]' length under 2.");
								return SSCANF_FAIL_RETURN;
							}
							else if (doSave)
							{
								amx_GetAddr(amx, params[paramPos++], &cptr);
								*cptr++ = b;
								*cptr = g_iInvalid;
							}
						}
						else
						{
							SAVE_VALUE((cell)b);
						}
						break;
					}
					SkipDefault(&format);
					// FALLTHROUGH
				case 'q':
					if (*format == '[')
					{
						int
							len = GetLength(&format, true);
						if (len < 2)
						{
							logprintf("sscanf error: 'q[len]' length under 2.");
							return SSCANF_FAIL_RETURN;
						}
						else
						{
							int
								b = -1,
								od = gOptions;
							if (doSave)
							{
								char *
									tstr;
								// Don't detect multiple results.
								gOptions &= ~4;
								amx_GetAddr(amx, params[paramPos++], &cptr);
								while (--len)
								{
									tstr = string;
									if (!DoQ(&tstr, &b, b + 1))
									{
										*cptr++ = b;
										b = g_iInvalid;
										break;
									}
									if (b == g_iInvalid) break;
									*cptr++ = b;
								}
								if (b == g_iInvalid)
								{
									*cptr = g_iInvalid;
								}
								else
								{
									tstr = string;
									DoQ(&tstr, &b, b + 1);
									if (b == g_iInvalid) *cptr = g_iInvalid;
									else *cptr = 0x80000000;
								}
								// Restore results detection.
								gOptions = od;
								string = tstr;
							}
							else
							{
								DoQ(&string, &b, 0);
							}
						}
					}
					else
					{
						int
							b;
						DoQ(&string, &b, 0);
						SAVE_VALUE((cell)b);
					}
					break;
				case 'R':
					if (IsDelimiter(*string))
					{
						int
							b;
						DoRD(&format, &b);
						if (*format == '[')
						{
							int
								len = GetLength(&format, true);
							if (gOptions & 1)
							{
								// Incompatible combination.
								logprintf("sscanf error: 'R(name)[len]' is incompatible with OLD_DEFAULT_NAME.");
								return SSCANF_FAIL_RETURN;
							}
							else if (len < 2)
							{
								logprintf("sscanf error: 'R(num)[len]' length under 2.");
								return SSCANF_FAIL_RETURN;
							}
							else if (doSave)
							{
								amx_GetAddr(amx, params[paramPos++], &cptr);
								*cptr++ = b;
								*cptr = g_iInvalid;
							}
						}
						else
						{
							SAVE_VALUE((cell)b);
						}
						break;
					}
					SkipDefault(&format);
					// FALLTHROUGH
				case 'r':
					if (*format == '[')
					{
						int
							len = GetLength(&format, true);
						if (len < 2)
						{
							logprintf("sscanf error: 'r[len]' length under 2.");
							return SSCANF_FAIL_RETURN;
						}
						else
						{
							int
								b = -1,
								od = gOptions;
							if (doSave)
							{
								char *
									tstr;
								// Don't detect multiple results.
								gOptions &= ~4;
								amx_GetAddr(amx, params[paramPos++], &cptr);
								while (--len)
								{
									tstr = string;
									if (!DoR(&tstr, &b, b + 1))
									{
										*cptr++ = b;
										b = g_iInvalid;
										break;
									}
									if (b == g_iInvalid) break;
									*cptr++ = b;
								}
								if (b == g_iInvalid)
								{
									*cptr = g_iInvalid;
								}
								else
								{
									tstr = string;
									DoR(&tstr, &b, b + 1);
									if (b == g_iInvalid) *cptr = g_iInvalid;
									else *cptr = 0x80000000;
								}
								// Restore results detection.
								gOptions = od;
								string = tstr;
							}
							else
							{
								DoR(&string, &b, 0);
							}
						}
					}
					else
					{
						int
							b;
						DoR(&string, &b, 0);
						SAVE_VALUE((cell)b);
					}
					break;
				case 'A':
					// We need the default values here.
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoA(&format, &string, cptr, true))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						if (DoA(&format, &string, NULL, true))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'a':
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoA(&format, &string, cptr, false))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						if (DoA(&format, &string, NULL, false))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'E':
					// We need the default values here.
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoE(&format, &string, cptr, true))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						if (DoE(&format, &string, NULL, true))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'e':
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoE(&format, &string, cptr, false))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						if (DoE(&format, &string, NULL, false))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'K':
					// We need the default values here.
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoK(amx, &format, &string, cptr, true))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						if (DoK(amx, &format, &string, NULL, true))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'k':
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoK(amx, &format, &string, cptr, false))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						if (DoK(amx, &format, &string, NULL, false))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case '\'':
					// Find the end of the literal.
					{
						char
							* str = format,
							* write = format;
						bool
							escape = false;
						while (!IsEnd(*str) && (escape || *str != '\''))
						{
							if (*str == '\\')
							{
								if (escape)
								{
									// "\\" - Go back a step to write this
									// character over the last character (which
									// just happens to be the same character).
									--write;
								}
								escape = !escape;
							}
							else
							{
								if (*str == '\'')
								{
									// Overwrite the escape character with the
									// quote character.  Must have been
									// preceeded by a slash or it wouldn't have
									// got to here in the loop.
									--write;
								}
								escape = false;
							}
							// Copy the string over itself to get rid of excess
							// escape characters.
							// Not sure if it's faster in the average case to
							// always do the copy or check if it's needed.
							// This write is always safe as it makes the string
							// shorter, so we'll never run out of space.  It
							// will also not overwrite the original string.
							*write++ = *str++;
						}
						if (*str == '\'')
						{
							// Correct end.  Make a shorter string to search
							// for.
							*write = '\0';
							// Find the current section of format in string.
							char *
								find = strstr(string, format);
							if (!find)
							{
								// Didn't find the string
								RestoreOpts(defaultOpts);
								return SSCANF_FAIL_RETURN;
							}
							// Found the string.  Update the current string
							// position to the length of the search term
							// further along from the start of the term.  Use
							// "write" here as we want the escaped string
							// length.
							string = find + (write - format);
							// Move to after the end of the search string.  Use
							// "str" here as we want the unescaped string
							// length.
							format = str + 1;
						}
						else
						{
							logprintf("sscanf warning: Unclosed string literal.");
							char *
								find = strstr(string, format);
							if (!find)
							{
								RestoreOpts(defaultOpts);
								return SSCANF_FAIL_RETURN;
							}
							string = find + (write - format);
							format = str;
						}
					}
					break;
				case '?':
					{
						char *
							t = GetMultiType(&format);
						if (t) DoOptions(t, -1);
						else return SSCANF_FAIL_RETURN;
						continue;
					}
				case '%':
					logprintf("sscanf warning: sscanf specifiers do not require '%' before them.");
					continue;
				default:
					logprintf("sscanf warning: Unknown format specifier '%c', skipping.", *(format - 1));
					continue;
			}
			// Loop cleanup - only skip one spacer so that we can detect
			// multiple explicit delimiters in a row, for example:
			// 
			// hi     there
			// 
			// is NOT multiple explicit delimiters in a row (they're
			// whitespace).  This however is:
			// 
			// hi , , , there
			// 
			SkipOneSpacer(&string);
		}
	}
	// Temporary to the end of the code.
	ResetDelimiter();
	AddDelimiter(')');
	// We don't need code here to handle the case where paramPos was reached,
	// but the end of the string wasn't - if that's the case there's no
	// problem as we just ignore excess string data.
	while (paramPos < paramCount || !doSave)
	{
		// Loop through if there's still parameters remaining.
		if (!*format)
		{
			logprintf("sscanf warning: Format specifier does not match parameter count.");
			if (!doSave)
			{
				// Started a quiet section but never explicitly ended it.
				logprintf("sscanf warning: Unclosed quiet section.");
			}
			RestoreOpts(defaultOpts);
			return SSCANF_TRUE_RETURN;
		}
		else if (IsWhitespace(*format))
		{
			++format;
		}
		else
		{
			// Do the main switch again.
			switch (*format++)
			{
				case 'L':
					DE(bool, L)
				case 'B':
					DE(int, B)
				case 'N':
					DE(int, N)
				case 'C':
					DE(char, C)
				case 'I':
				case 'D':
					DE(int, I)
				case 'H':
				case 'X':
					DE(int, H)
				case 'O':
					DE(int, O)
				case 'F':
					DEF(double, F)
				case 'G':
					DEF(double, G)
				case 'U':
					DE(int, U)
				case 'Q':
					DE(int, Q)
				case 'R':
					DE(int, R)
				case 'A':
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoA(&format, NULL, cptr, true))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						// Also pass NULL data so it knows to only collect the
						// default values.
						if (DoA(&format, NULL, NULL, true))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'E':
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoE(&format, NULL, cptr, true))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						// Also pass NULL data so it knows to only collect the
						// default values.
						if (DoE(&format, NULL, NULL, true))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case 'K':
					if (doSave)
					{
						amx_GetAddr(amx, params[paramPos++], &cptr);
						if (DoK(amx, &format, NULL, cptr, true))
						{
							break;
						}
					}
					else
					{
						// Pass a NULL pointer so data isn't saved anywhere.
						// Also pass NULL data so it knows to only collect the
						// default values.
						if (DoK(amx, &format, NULL, NULL, true))
						{
							break;
						}
					}
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case '{':
					if (doSave)
					{
						doSave = false;
					}
					else
					{
						// Already in a quiet section.
						logprintf("sscanf warning: Can't have nestled quiet sections.");
					}
					break;
				case '}':
					if (doSave)
					{
						logprintf("sscanf warning: Not in a quiet section.");
					}
					else
					{
						doSave = true;
					}
					break;
				case 'Z':
					logprintf("sscanf warning: 'Z' doesn't exist - that would be an optional, deprecated optional string!.");
					// FALLTHROUGH
				case 'z':
					logprintf("sscanf warning: 'z' is deprecated, consider using 'S' instead.");
					// FALLTHROUGH
				case 'S':
					{
						char *
							dest;
						int
							length;
						if (DoSD(&format, &dest, &length))
						{
							// Send the string to PAWN.
							if (doSave)
							{
								amx_GetAddr(amx, params[paramPos++], &cptr);
								amx_SetString(cptr, dest, 0, 0, length);
							}
						}
					}
					break;
				case 'P':
					//logprintf("sscanf warning: You can't have an optional delimiter.");
					GetMultiType(&format);
					continue;
					// FALLTHROUGH
				case 'p':
					// Discard delimiter.  This only matters when they have
					// real inputs, not the default ones used here.
					GetSingleType(&format);
					continue;
				case '\'':
					// Implicitly optional if the specifiers after it are
					// optional.
					{
						bool
							escape = false;
						while (!IsEnd(*format) && (escape || *format != '\''))
						{
							if (*format == '\\')
							{
								escape = !escape;
							}
							else
							{
								escape = false;
							}
							++format;
						}
						if (*format == '\'')
						{
							++format;
						}
						else
						{
							logprintf("sscanf warning: Unclosed string literal.");
						}
					}
					break;
					// Large block of specifiers all together.
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
				case 'k':
				case 'l':
				case 'n':
				case 'o':
				case 'q':
				case 'r':
				case 's':
				case 'u':
				case 'x':
					// These are non optional items, but the input string
					// didn't include them, so we fail - this is in fact the
					// most basic definition of a fail (the original)!  We
					// don't need any text warnings here - admittedly we don't
					// know if the format specifier is well formed (there may
					// not be enough return variables for example), but it
					// doesn't matter - the coder should have tested for those
					// things, and the more important thing is that the user
					// didn't enter the correct data.
					RestoreOpts(defaultOpts);
					return SSCANF_FAIL_RETURN;
				case '?':
					GetMultiType(&format);
					continue;
				case '%':
					logprintf("sscanf warning: sscanf specifiers do not require '%' before them.");
					break;
				default:
					logprintf("sscanf warning: Unknown format specifier '%c', skipping.", *(format - 1));
					break;
			}
			// Don't need any cleanup here.
		}
	}
	if (*format)
	{
		do
		{
			if (!IsWhitespace(*format))
			{
				// Only print this warning if the remaining characters are not
				// spaces - spaces are allowed, and sometimes required, on the
				// ends of formats (e.g. to stop the final 's' specifier
				// collecting all remaining characters and only get one word).
				// We could check that the remaining specifier is a valid one,
				// but this is only a guide - they shouldn't even have other
				// characters IN the specifier so it doesn't matter - it will
				// point to a bug, which is the important thing.
				if (doSave)
				{
					if (*format == '}')
					{
						logprintf("sscanf warning: Not in a quiet section.");
					}
					else if (*format != '{')
					{
						// Fix the bad display bug.
						logprintf("sscanf warning: Format specifier does not match parameter count.");
					}
					// Only display it once.
					break;
				}
				else
				{
					if (*format == '}')
					{
						doSave = true;
					}
					else
					{
						logprintf("sscanf warning: Format specifier does not match parameter count.");
						break;
					}
				}
			}
			++format;
		}
		while (*format);
	}
	if (!doSave)
	{
		// Started a quiet section but never explicitly ended it.
		logprintf("sscanf warning: Unclosed quiet section.");
	}
	// No more parameters and no more format specifiers which could be read
	// from - this is a valid return!
	RestoreOpts(defaultOpts);
	return SSCANF_TRUE_RETURN;
}

//#if SSCANF_QUIET
void
	qlog(char * str, ...)
{
	// Do nothing
}
//#endif

//----------------------------------------------------------
// The Support() function indicates what possibilities this
// plugin has. The SUPPORTS_VERSION flag is required to check
// for compatibility with the server. 

PLUGIN_EXPORT unsigned int PLUGIN_CALL
	Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

//----------------------------------------------------------
// The Load() function gets passed on exported functions from
// the SA-MP Server, like the AMX Functions and logprintf().
// Should return true if loading the plugin has succeeded.

PLUGIN_EXPORT bool PLUGIN_CALL
	Load(void ** ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	real_logprintf = logprintf;
	//GetServer = (GetServer_t)ppData[0xE1];
	
	//logprintf("0x%08X\n", (int)logprintf);
	logprintf("\n");
	logprintf(" ===============================\n");
	logprintf("      sscanf plugin loaded.     \n");
	logprintf("         Version:  2.8.1        \n");
	logprintf("   (c) 2012 Alex \"Y_Less\" Cole  \n");
	logprintf(" ===============================\n");
	
	// Determine server version.  Ideally we would use a feature check, such as
	// checking wether ConnectNPC exists, doing away with the need for any
	// memory addresses, unfortunately if the native isn't used in the current
	// amx, then amx_FindNative won't find it, despite the fact that it does
	// exist.  I need to find a more reliable and more portable way of checking
	// the current server version.
	/*if (logprintf == LOGPRINTF_0221 || logprintf == LOGPRINTF_0222 || logprintf == LOGPRINTF_0223 || logprintf == LOGPRINTF_0224)
	{
		g_iServerVersion = SERVER_VERSION_0200;
		g_iTrueMax = MAX_PLAYERS_0200;
		g_iInvalid = INVALID_PLAYER_ID_0200;
	}
	else if (logprintf == LOGPRINTF_0300)
	{
		g_iServerVersion = SERVER_VERSION_0300;
		g_iTrueMax = MAX_PLAYERS_0300;
		g_iInvalid = INVALID_PLAYER_ID_0300;
	}
	else *//*if (logprintf == LOGPRINTF_0340)
	{
		g_iServerVersion = SERVER_VERSION_0340;
		g_iTrueMax = MAX_PLAYERS_0300;
		g_iInvalid = INVALID_PLAYER_ID_0300;
	}
	else if (logprintf == LOGPRINTF_0342)
	{
		g_iServerVersion = SERVER_VERSION_0342;
		g_iTrueMax = MAX_PLAYERS_0300;
		g_iInvalid = INVALID_PLAYER_ID_0300;
	}
	else
	{
		logprintf("sscanf error: The current build ONLY supports 0.3d");
	}*/ 
	#if SSCANF_QUIET
		logprintf = qlog;
	#endif
	return true;
}

//----------------------------------------------------------
// The Unload() function is called when the server shuts down,
// meaning this plugin gets shut down with it.

PLUGIN_EXPORT void PLUGIN_CALL
	Unload()
{
	logprintf("\n");
	logprintf(" ===============================\n");
	logprintf("     sscanf plugin unloaded.    \n");
	logprintf(" ===============================\n");
}

static cell AMX_NATIVE_CALL
	n_SSCANF_Init(AMX * amx, cell * params)
{
	if (params[0] != 3 * sizeof (cell))
	{
		logprintf("sscanf error: SSCANF_Init has incorrect parameters.");
		g_iTrueMax = 0;
		return 0;
	}
	if (g_iTrueMax != 0)
	{
		// Already initialised.
		return 1;
	}
	g_iTrueMax = (int)params[1];
	g_iInvalid = (int)params[2];
	g_iMaxPlayerName = (int)params[3];
	g_szPlayerNames = new char [g_iTrueMax * g_iMaxPlayerName];
	g_iConnected = new int [g_iTrueMax];
	// FINALLY fixed this bug - I didn't know "new []" didn't initialise...
	memset(g_iConnected, 0, sizeof (int) * g_iTrueMax);
	g_iNPC = new int [g_iTrueMax];
	return 1;
}

static void
	DoName(AMX * amx, cell playerid, cell name)
{
	cell *
		str;
	int
		len;
	amx_GetAddr(amx, name, &str);
	amx_StrLen(str, &len);
	if ((unsigned int)len >= g_iMaxPlayerName)
	{
		len = (int)g_iMaxPlayerName - 1;
	}
	amx_GetString(g_szPlayerNames + (g_iMaxPlayerName * playerid), str, 0, len + 1);
}

static cell AMX_NATIVE_CALL
	n_SSCANF_Join(AMX * amx, cell * params)
{
	if (params[0] != 3 * sizeof (cell))
	{
		logprintf("sscanf error: SSCANF_Join has incorrect parameters.");
		return 0;
	}
	cell
		playerid = params[1];
	++g_iConnected[playerid];
	DoName(amx, playerid, params[2]);
	g_iNPC[playerid] = params[3];
	return 1;
}

static cell AMX_NATIVE_CALL
	n_SSCANF_Leave(AMX * amx, cell * params)
{
	if (params[0] != 1 * sizeof (cell))
	{
		logprintf("sscanf error: SSCANF_Leave has incorrect parameters.");
		return 0;
	}
	// To be correct for multiple scripts with loads and unloads (unloadfs).
	--g_iConnected[params[1]];
	return 1;
}

static cell AMX_NATIVE_CALL
	n_SSCANF_Option(AMX * amx, cell * params)
{
	if (params[0] != 2 * sizeof (cell))
	{
		logprintf("sscanf error: SSCANF_Option has incorrect parameters.");
		return 0;
	}
	char *
		string;
	STR_PARAM(amx, params[1], string);
	DoOptions(string, params[2]);
	return 1;
}

static cell AMX_NATIVE_CALL
	n_SSCANF_SetPlayerName(AMX * amx, cell * params)
{
	// Hook ALL AMXs, even if they don't use sscanf, by working at the plugin
	// level.  This allows us to intercept name changes.
	//cell
	//	result;
	//amx_Callback(amx, 0, &result, params);
	if (params[0] != 2 * sizeof (cell))
	{
		logprintf("sscanf error: SSCANF_SetPlayerName has incorrect parameters.");
		return 0;
	}
	DoName(amx, params[1], params[2]);
	return SetPlayerName(amx, params);
}

//----------------------------------------------------------
// The AmxLoad() function gets called when a new gamemode or
// filterscript gets loaded with the server. In here we register
// the native functions we like to add to the scripts.

AMX_NATIVE_INFO
	sscanfNatives[] =
		{
			{"sscanf", n_sscanf},
			{"SSCANF_Init", n_SSCANF_Init},
			{"SSCANF_Join", n_SSCANF_Join},
			{"SSCANF_Leave", n_SSCANF_Leave},
			{"SSCANF_Option", n_SSCANF_Option},
			{0,        0}
		};

// From "amx.c", part of the PAWN language runtime:
// http://code.google.com/p/pawnscript/source/browse/trunk/amx/amx.c

#define USENAMETABLE(hdr) \
	((hdr)->defsize==sizeof(AMX_FUNCSTUBNT))

#define NUMENTRIES(hdr,field,nextfield) \
	(unsigned)(((hdr)->nextfield - (hdr)->field) / (hdr)->defsize)

#define GETENTRY(hdr,table,index) \
	(AMX_FUNCSTUB *)((unsigned char*)(hdr) + (unsigned)(hdr)->table + (unsigned)index*(hdr)->defsize)

#define GETENTRYNAME(hdr,entry) \
	(USENAMETABLE(hdr) ? \
		(char *)((unsigned char*)(hdr) + (unsigned)((AMX_FUNCSTUBNT*)(entry))->nameofs) : \
		((AMX_FUNCSTUB*)(entry))->name)

PLUGIN_EXPORT int PLUGIN_CALL
	AmxLoad(AMX * amx) 
{
	int
		num,
		idx;
	// Operate on the raw AMX file, don't use the amx_ functions to avoid issues
	// with the fact that we've not actually finished initialisation yet.  Based
	// VERY heavilly on code from "amx.c" in the PAWN runtime library.
	AMX_HEADER *
		hdr = (AMX_HEADER *)amx->base;
	AMX_FUNCSTUB *
		func;
	num = NUMENTRIES(hdr, natives, libraries);
	for (idx = 0; idx != num; ++idx)
	{
		func = GETENTRY(hdr, natives, idx);
		if (!strcmp("SetPlayerName", GETENTRYNAME(hdr, func)))
		{
			// Intercept the call!
			SetPlayerName = (AMX_NATIVE)func->address;
			func->address = (ucell)n_SSCANF_SetPlayerName;
			break;
		}
	}
	return amx_Register(amx, sscanfNatives, -1);
}

//----------------------------------------------------------
// When a gamemode is over or a filterscript gets unloaded, this
// function gets called. No special actions needed in here.

PLUGIN_EXPORT int PLUGIN_CALL
	AmxUnload(AMX * amx) 
{
	return AMX_ERR_NONE;
}
