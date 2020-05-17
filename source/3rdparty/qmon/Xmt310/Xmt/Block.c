/* 
 * Motif Tools Library, Version 3.1
 * $Id: Block.c,v 1.1.1.1 2001/07/18 11:06:01 root Exp $
 * 
 * Written by David Flanagan.
 * Copyright (c) 1992-2001 by David Flanagan.
 * All Rights Reserved.  See the file COPYRIGHT for details.
 * This is open source software.  See the file LICENSE for details.
 * There is no warranty for this software.  See NO_WARRANTY for details.
 *
 * $Log: Block.c,v $
 * Revision 1.1.1.1  2001/07/18 11:06:01  root
 * Initial checkin.
 *
 * Revision 1.1  2001/06/12 15:00:21  andre
 * AA-2001-06-12-0: replaced Xmt212 by Xmt310
 *                  (http://sourceforge.net/projects/motiftools) with
 *                  our xmt212 patches applied
 *
 *
 */

#include <Xmt/Xmt.h>

#if NeedFunctionPrototypes
void XmtBlock(Widget w, Boolean *block)
#else
void XmtBlock(w, block)
Widget w;
Boolean *block;
#endif
{
    XtAppContext app = XtWidgetToApplicationContext(w);
    
    while (*block) {
	XtAppProcessEvent(app, XtIMAll);
	/* argv puts an XSync here.  Why? */
    }

    XSync(XtDisplayOfObject(w), 0);
    XmUpdateDisplay(w);
}
