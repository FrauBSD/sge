/*___INFO__MARK_BEGIN__*/
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  the Sun Industry Standards Source License Version 1.2
 *
 *  Sun Microsystems Inc., March, 2001
 *
 *
 *  Sun Industry Standards Source License Version 1.2
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.2 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://gridengine.sunsource.net/Gridengine_SISSL_license.html
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2001 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 ************************************************************************/
/*___INFO__MARK_END__*/
package com.sun.grid.installer.gui;

import com.izforge.izpack.installer.InstallerFrame;
import java.awt.Rectangle;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

/**
 * Frame to show log file content
 */
public class LogFrame extends JFrame {
    private JTextArea textArea = null;
    private InstallerFrame parent = null;
    private JScrollPane scrollPane = null;

    /**
     * Constructor
     * @param title The title of the frame
     * @param image
     */
    public LogFrame(InstallerFrame parent, String title) {
        super(title);

        this.parent = parent;

        setIconImage(parent.icons.getImageIcon("frame.log").getImage());

        setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        setSize(500, 500);

        buildGUI();
    }

    /**
     * Build the window
     */
    private void buildGUI() {
        textArea = new JTextArea();
        textArea.setEditable(false);

        scrollPane = new JScrollPane(textArea);

        getContentPane().add(scrollPane);
    }

    /**
     * Sets the content of the text area.
     * @param logFilePath Path to the log file whose content has to be shown
     */
    public void setLogContent(String log) {
        textArea.setText(log);

        // scroll to the most top-left corner
        textArea.setSelectionStart(0);
        textArea.setSelectionEnd(0);
    }
}
