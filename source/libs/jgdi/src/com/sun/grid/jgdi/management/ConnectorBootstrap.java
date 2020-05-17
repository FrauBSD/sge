/*
 * Copyright 2003-2005 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */
package com.sun.grid.jgdi.management;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.rmi.AlreadyBoundException;
import java.rmi.NoSuchObjectException;
import java.rmi.NotBoundException;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.registry.Registry;
import java.rmi.server.RMIClientSocketFactory;
import java.rmi.server.RMIServerSocketFactory;
import java.rmi.server.UnicastRemoteObject;
import java.security.Principal;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.StringTokenizer;
import java.lang.management.ManagementFactory;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.management.MBeanServer;
import javax.management.remote.JMXAuthenticator;
import javax.management.remote.JMXConnectorServer;
import javax.management.remote.JMXConnectorServerFactory;
import javax.management.remote.JMXServiceURL;
import javax.management.remote.rmi.RMIConnectorServer;
import javax.rmi.ssl.SslRMIClientSocketFactory;
import javax.security.auth.Subject;

import sun.rmi.server.UnicastServerRef;
import sun.rmi.server.UnicastServerRef2;
import sun.management.Agent;
import sun.management.AgentConfigurationError;
import static sun.management.AgentConfigurationError.*;
import sun.management.FileSystem;
// import sun.rmi.registry.RegistryImpl;

import com.sun.jmx.remote.internal.RMIExporter;
import com.sun.jmx.remote.security.JMXPluggableAuthenticator;
import java.rmi.registry.LocateRegistry;

/**
 * This class initializes and starts a customized RMIConnectorServer for JSR 163 
 * JMX Monitoring.
 **/
public final class ConnectorBootstrap {

    private final static Logger log = Logger.getLogger(ConnectorBootstrap.class.getName());
    static private Registry registry;

    /**
     * Default values for JMX configuration properties.
     **/
    public static interface DefaultValues {

        public static final String PORT = "0";
        public static final String CONFIG_FILE_NAME = "management.properties";
        public static final String USE_SSL = "true";
        public static final String USE_REGISTRY_SSL = "false";
        public static final String USE_AUTHENTICATION = "true";
        public static final String PASSWORD_FILE_NAME = "jmxremote.password";
        public static final String ACCESS_FILE_NAME = "jmxremote.access";
        public static final String SSL_NEED_CLIENT_AUTH = "false";
    }

    /**
     * Names of JMX configuration properties.
     **/
    public static interface PropertyNames {

        public static final String PORT = "com.sun.grid.jgdi.management.jmxremote.port";
        public static final String CONFIG_FILE_NAME = "com.sun.grid.jgdi.management.config.file";
        public static final String USE_SSL = "com.sun.grid.jgdi.management.jmxremote.ssl";
        public static final String USE_REGISTRY_SSL = "com.sun.grid.jgdi.management.jmxremote.registry.ssl";
        public static final String USE_AUTHENTICATION = "com.sun.grid.jgdi.management.jmxremote.authenticate";
        public static final String PASSWORD_FILE_NAME = "com.sun.grid.jgdi.management.jmxremote.password.file";
        public static final String ACCESS_FILE_NAME = "com.sun.grid.jgdi.management.jmxremote.access.file";
        public static final String LOGIN_CONFIG_NAME = "com.sun.grid.jgdi.management.jmxremote.login.config";
        public static final String SSL_ENABLED_CIPHER_SUITES = "com.sun.grid.jgdi.management.jmxremote.ssl.enabled.cipher.suites";
        public static final String SSL_ENABLED_PROTOCOLS = "com.sun.grid.jgdi.management.jmxremote.ssl.enabled.protocols";
        public static final String SSL_NEED_CLIENT_AUTH = "com.sun.grid.jgdi.management.jmxremote.ssl.need.client.auth";
        public static final String SSL_SERVER_KEYSTORE = "com.sun.grid.jgdi.management.jmxremote.ssl.serverKeystore";
        public static final String SSL_SERVER_KEYSTORE_PASSWORD = "com.sun.grid.jgdi.management.jmxremote.ssl.serverKeystorePassword";
        public static final String SSL_SERVER_KEYSTORE_PASSWORD_FILE = "com.sun.grid.jgdi.management.jmxremote.ssl.serverKeystorePasswordFile";
    }

    /**
     * <p>Prevents our RMI server objects from keeping the JVM alive.</p>
     *
     * <p>We use a private interface in Sun's JMX Remote API implementation
     * that allows us to specify how to export RMI objects.  We do so using
     * UnicastServerRef, a class in Sun's RMI implementation.  This is all
     * non-portable, of course, so this is only valid because we are inside
     * Sun's JRE.</p>
     *
     * <p>Objects are exported using {@link
     * UnicastServerRef#exportObject(Remote, Object, boolean)}.  The
     * boolean parameter is called <code>permanent</code> and means
     * both that the object is not eligible for Distributed Garbage
     * Collection, and that its continued existence will not prevent
     * the JVM from exiting.  It is the latter semantics we want (we
     * already have the former because of the way the JMX Remote API
     * works).  Hence the somewhat misleading name of this class.</p>
     */
    private static class PermanentExporter implements RMIExporter {

        public Remote exportObject(Remote obj, int port,
                RMIClientSocketFactory csf, RMIServerSocketFactory ssf)
                throws RemoteException {

            synchronized (this) {
                if (firstExported == null) {
                    firstExported = obj;
                }
            }

            final UnicastServerRef ref;
            if (csf == null && ssf == null) {
                ref = new UnicastServerRef(port);
            } else {
                ref = new UnicastServerRef2(port, csf, ssf);
            }
            return ref.exportObject(obj, null, true);
        }

        // Nothing special to be done for this case
        public boolean unexportObject(Remote obj, boolean force)
                throws NoSuchObjectException {
            boolean ret = UnicastRemoteObject.unexportObject(obj, force);
            firstExported = null;
            return ret;
        }
        Remote firstExported;
    }
    static PermanentExporter exporter = new PermanentExporter();

    /**
     * This JMXAuthenticator wraps the JMXPluggableAuthenticator and verifies
     * that at least one of the principal names contained in the authenticated
     * Subject is present in the access file.
     */
    private static class AccessFileCheckerAuthenticator implements
            JMXAuthenticator {

        public AccessFileCheckerAuthenticator(Map<String, Object> env)
                throws IOException {
            environment = env;
            accessFile = (String) env.get("jmx.remote.x.access.file");
            properties = propertiesFromFile(accessFile);
        }

        public Subject authenticate(Object credentials) {
            final JMXAuthenticator authenticator = new JMXPluggableAuthenticator(
                    environment);
            final Subject subject = authenticator.authenticate(credentials);
            checkAccessFileEntries(subject);
            return subject;
        }

        private void checkAccessFileEntries(Subject subject) {
            if (subject == null) {
                throw new SecurityException(
                        "Access denied! No matching entries found in " + "the access file [" + accessFile + "] as the " + "authenticated Subject is null");
            }
            final Set principals = subject.getPrincipals();
            for (Iterator i = principals.iterator(); i.hasNext();) {
                final Principal p = (Principal) i.next();
                if (properties.containsKey(p.getName())) {
                    return;
                }
            }
            final Set<String> principalsStr = new HashSet<String>();
            for (Iterator i = principals.iterator(); i.hasNext();) {
                final Principal p = (Principal) i.next();
                principalsStr.add(p.getName());
            }
            throw new SecurityException(
                    "Access denied! No entries found in the access file [" + accessFile + "] for any of the authenticated identities " + principalsStr);
        }
        private final Map<String, Object> environment;
        private final Properties properties;
        private final String accessFile;
    }

    private static Properties propertiesFromFile(String fname)
            throws IOException {
        Properties p = new Properties();
        if (fname == null) {
            return p;
        }
        FileInputStream fin = new FileInputStream(fname);
        p.load(fin);
        fin.close();
        return p;
    }

    /**
     * Initializes and starts the JMX Connector Server.
     * If the com.sun.management.jmxremote.port property is not defined,
     * simply return. Otherwise, attempts to load the config file, and
     * then calls {@link #initialize(java.lang.String, java.util.Properties)}.
     *
     **/
    public static synchronized JMXConnectorServer initialize() {

        // Load a new management properties
        final Properties props = Agent.loadManagementProperties();
        if (props == null) {
            return null;
        }

        final String portStr = props.getProperty(PropertyNames.PORT);

        // System.out.println("initializing: {port=" + portStr + ",
        //                     properties="+props+"}");
        return initialize(portStr, props);
    }

    /**
     * Initializes and starts a JMX Connector Server for remote
     * monitoring and management.
     **/
    public static synchronized JMXConnectorServer initialize(
            String portStr, Properties props) {

        // Get serverPort number
        final int serverPort;
        try {
            serverPort = Integer.parseInt(portStr);
        } catch (NumberFormatException x) {
            throw new AgentConfigurationError(INVALID_JMXREMOTE_PORT,
                    x, portStr);
        }
        if (serverPort < 0) {
            throw new AgentConfigurationError(INVALID_JMXREMOTE_PORT,
                    portStr);
        }

        // Do we use authentication?
        final String useAuthenticationStr = props.getProperty(
                PropertyNames.USE_AUTHENTICATION,
                DefaultValues.USE_AUTHENTICATION);
        final boolean useAuthentication = Boolean.valueOf(
                useAuthenticationStr).booleanValue();

        // Do we use SSL?
        final String useSslStr = props.getProperty(
                PropertyNames.USE_SSL, DefaultValues.USE_SSL);
        final boolean useSsl = Boolean.valueOf(useSslStr).booleanValue();

        // Do we use RMI Registry SSL?
        final String useRegistrySslStr = props.getProperty(
                PropertyNames.USE_REGISTRY_SSL,
                DefaultValues.USE_REGISTRY_SSL);
        final boolean useRegistrySsl = Boolean.valueOf(
                useRegistrySslStr).booleanValue();

        final String enabledCipherSuites = props.getProperty(PropertyNames.SSL_ENABLED_CIPHER_SUITES);
        String enabledCipherSuitesList[] = null;
        if (enabledCipherSuites != null) {
            StringTokenizer st = new StringTokenizer(
                    enabledCipherSuites, ",");
            int tokens = st.countTokens();
            enabledCipherSuitesList = new String[tokens];
            for (int i = 0; i < tokens; i++) {
                enabledCipherSuitesList[i] = st.nextToken();
            }
        }

        final String enabledProtocols = props.getProperty(PropertyNames.SSL_ENABLED_PROTOCOLS);
        String enabledProtocolsList[] = null;
        if (enabledProtocols != null) {
            StringTokenizer st = new StringTokenizer(enabledProtocols,
                    ",");
            int tokens = st.countTokens();
            enabledProtocolsList = new String[tokens];
            for (int i = 0; i < tokens; i++) {
                enabledProtocolsList[i] = st.nextToken();
            }
        }

        final String sslNeedClientAuthStr = props.getProperty(
                PropertyNames.SSL_NEED_CLIENT_AUTH,
                DefaultValues.SSL_NEED_CLIENT_AUTH);
        final boolean sslNeedClientAuth = Boolean.valueOf(
                sslNeedClientAuthStr).booleanValue();
        String serverKeystorePassword = null;
        String loginConfigName = null;
        String passwordFileName = null;
        String keystorePasswordFileName = null;
        String accessFileName = null;

        // Initialize settings when authentication is active
        if (useAuthentication) {

            // Get non-default login configuration
            loginConfigName = props.getProperty(PropertyNames.LOGIN_CONFIG_NAME);

            if (loginConfigName == null) {
                // Get password file
                passwordFileName = props.getProperty(
                        PropertyNames.PASSWORD_FILE_NAME,
                        getDefaultFileName(DefaultValues.PASSWORD_FILE_NAME));
                checkPasswordFile(passwordFileName);
            }

            // Get access file
            accessFileName = props.getProperty(
                    PropertyNames.ACCESS_FILE_NAME,
                    getDefaultFileName(DefaultValues.ACCESS_FILE_NAME));
            checkAccessFile(accessFileName);
        }

        if (useSsl) {

            // Get keystore password file
            keystorePasswordFileName = props.getProperty(PropertyNames.SSL_SERVER_KEYSTORE_PASSWORD_FILE);

            // get the keystore password from the keystore password file
            // (/var/sgeCA/portNNNN/sge_cell/private/keystore.password)
            // workaround for euid problem of libjvm under Linux otherwise jmxremote.password
            // could have been used
            try {
                serverKeystorePassword = propertiesFromFile(keystorePasswordFileName).getProperty(PropertyNames.SSL_SERVER_KEYSTORE_PASSWORD);
            } catch (IOException ex) {
                throw new AgentConfigurationError(AGENT_EXCEPTION, ex, ex.getMessage());
            }

            // setup SSLContext to use different TrustManager and KeyManager
            final String serverKeystore = props.getProperty(PropertyNames.SSL_SERVER_KEYSTORE);

            File serverKeystoreFile = new File(serverKeystore);
            File caTop = JGDIAgent.getCaTop();
            String serverHost = "";
            try {
                serverHost = InetAddress.getLocalHost().getHostName();
            } catch (UnknownHostException ex) {
                throw new AgentConfigurationError(AGENT_EXCEPTION, ex, "Can't resolve local host");
            }
            char[] pw = (serverKeystorePassword != null) ? serverKeystorePassword.toCharArray() : "".toCharArray();
            log.log(Level.FINE, "SSLHelper.init: caTop = {0} serverKeystore = {1} serverKeystorePW = {2}",
                    new Object[]{caTop, serverKeystore, (serverKeystorePassword != null) ? serverKeystorePassword : "-empty pw-"});
            SSLHelper.getInstanceByKey(serverHost, serverPort, caTop).setKeystore(serverKeystoreFile, pw);
        }

        if (log.isLoggable(Level.FINE)) {
            log.log(Level.FINE,
                    "initialize",
                    Agent.getText("jmxremote.ConnectorBootstrap.initialize") + "\n\t" + PropertyNames.PORT + "=" + serverPort + "\n\t" + PropertyNames.USE_SSL + "=" + useSsl + "\n\t" + PropertyNames.USE_REGISTRY_SSL + "=" + useRegistrySsl + "\n\t" + PropertyNames.SSL_ENABLED_CIPHER_SUITES + "=" + enabledCipherSuites + "\n\t" + PropertyNames.SSL_ENABLED_PROTOCOLS + "=" + enabledProtocols + "\n\t" + PropertyNames.SSL_NEED_CLIENT_AUTH + "=" + sslNeedClientAuth + "\n\t" + PropertyNames.USE_AUTHENTICATION + "=" + useAuthentication + (useAuthentication ? (loginConfigName == null ? ("\n\t" + PropertyNames.PASSWORD_FILE_NAME + "=" + passwordFileName)
                    : ("\n\t" + PropertyNames.LOGIN_CONFIG_NAME + "=" + loginConfigName))
                    : "\n\t" + Agent.getText("jmxremote.ConnectorBootstrap.initialize.noAuthentication")) + (useAuthentication ? ("\n\t" + PropertyNames.ACCESS_FILE_NAME + "=" + accessFileName)
                    : "") + "");
        }

        final MBeanServer mbs = ManagementFactory.getPlatformMBeanServer();
        JMXConnectorServer cs = null;
        try {
            cs = exportMBeanServer(mbs, serverPort, useSsl, useRegistrySsl,
                    enabledCipherSuitesList, enabledProtocolsList,
                    sslNeedClientAuth, useAuthentication,
                    loginConfigName, passwordFileName, accessFileName);

            final JMXServiceURL url = cs.getAddress();
            if (log.isLoggable(Level.FINE)) {
                log.log(Level.FINE, "initialize", Agent.getText(
                        "jmxremote.ConnectorBootstrap.initialize.ready",
                        new JMXServiceURL(url.getProtocol(), url.getHost(),
                        url.getPort(), "/jndi/rmi://" + url.getHost() + ":" + serverPort + "/" + "jmxrmi").toString()));
            }
        } catch (Exception e) {
            throw new AgentConfigurationError(AGENT_EXCEPTION, e, e.toString());
        }
        return cs;
    }

    /*
     * Creates and starts a RMI Connector Server for "local" monitoring 
     * and management. 
     */
    public static JMXConnectorServer startLocalConnectorServer() {
        // Ensure cryptographically strong random number generater used
        // to choose the object number - see java.rmi.server.ObjID
        System.setProperty("java.rmi.server.randomIDs", "true");

        // This RMI server should not keep the VM alive
        Map<String, Object> env = new HashMap<String, Object>();
        env.put(RMIExporter.EXPORTER_ATTRIBUTE,
                exporter);

        // The local connector server need only be available via the
        // loopback connection. 
        String localhost = "localhost";
        InetAddress lh = null;
        try {
            lh = InetAddress.getByName(localhost);
            localhost = lh.getHostAddress();
        } catch (UnknownHostException x) {
        }

        // localhost unknown or (somehow) didn't resolve to
        // a loopback address.
        if (lh == null || !lh.isLoopbackAddress()) {
            localhost = "127.0.0.1";
        }

        MBeanServer mbs = ManagementFactory.getPlatformMBeanServer();
        try {
            JMXServiceURL url = new JMXServiceURL("rmi", localhost, 0);
            JMXConnectorServer server = JMXConnectorServerFactory.newJMXConnectorServer(url, env, mbs);
            server.start();
            return server;
        } catch (Exception e) {
            throw new AgentConfigurationError(AGENT_EXCEPTION, e, e.toString());
        }
    }

    private static void checkPasswordFile(String passwordFileName) {
        if (passwordFileName == null || passwordFileName.length() == 0) {
            throw new AgentConfigurationError(PASSWORD_FILE_NOT_SET);
        }
        File file = new File(passwordFileName);
        if (!file.exists()) {
            throw new AgentConfigurationError(PASSWORD_FILE_NOT_FOUND,
                    passwordFileName);
        }

        if (!file.canRead()) {
            throw new AgentConfigurationError(
                    PASSWORD_FILE_NOT_READABLE, passwordFileName);
        }

        FileSystem fs = FileSystem.open();
        try {
            if (fs.supportsFileSecurity(file)) {
                if (!fs.isAccessUserOnly(file)) {
                    final String msg = Agent.getText(
                            "jmxremote.ConnectorBootstrap.initialize.password.readonly",
                            passwordFileName);
                    log.log(Level.FINE, "initialize", msg);
                    throw new AgentConfigurationError(
                            PASSWORD_FILE_ACCESS_NOT_RESTRICTED,
                            passwordFileName);
                }
            }
        } catch (IOException e) {
            throw new AgentConfigurationError(
                    PASSWORD_FILE_READ_FAILED, e, passwordFileName);
        }
    }

    private static void checkAccessFile(String accessFileName) {
        if (accessFileName == null || accessFileName.length() == 0) {
            throw new AgentConfigurationError(ACCESS_FILE_NOT_SET);
        }
        File file = new File(accessFileName);
        if (!file.exists()) {
            throw new AgentConfigurationError(ACCESS_FILE_NOT_FOUND,
                    accessFileName);
        }

        if (!file.canRead()) {
            throw new AgentConfigurationError(ACCESS_FILE_NOT_READABLE,
                    accessFileName);
        }
    }

    /**
     * Compute the full path name for a default file.
     * @param basename basename (with extension) of the default file.
     * @return ${JRE}/lib/management/${basename}
     **/
    private static String getDefaultFileName(String basename) {
        final String fileSeparator = File.separator;
        return System.getProperty("java.home") + fileSeparator + "lib" + fileSeparator + "management" + fileSeparator + basename;
    }

    private static JMXConnectorServer exportMBeanServer(
            MBeanServer mbs, int serverPort, boolean useSsl,
            boolean useRegistrySsl, String[] enabledCipherSuites,
            String[] enabledProtocols, boolean sslNeedClientAuth,
            boolean useAuthentication, String loginConfigName,
            String passwordFileName, String accessFileName)
            throws IOException, MalformedURLException {

        /* Make sure we use non-guessable RMI object IDs.  Otherwise
         * attackers could hijack open connections by guessing their
         * IDs.  */
        System.setProperty("java.rmi.server.randomIDs", "true");

        JMXServiceURL url = new JMXServiceURL("rmi", null, 0);

        Map<String, Object> env = new HashMap<String, Object>();

        env.put(RMIExporter.EXPORTER_ATTRIBUTE, exporter);

        if (useAuthentication) {
            if (loginConfigName != null) {
                env.put("jmx.remote.x.login.config", loginConfigName);
            }
            if (passwordFileName != null) {
                env.put("jmx.remote.x.password.file", passwordFileName);
            }

            env.put("jmx.remote.x.access.file", accessFileName);

            if (env.get("jmx.remote.x.password.file") != null || env.get("jmx.remote.x.login.config") != null) {
                env.put(JMXConnectorServer.AUTHENTICATOR,
                        new AccessFileCheckerAuthenticator(env));
            }
        }

        RMIClientSocketFactory csf = null;
        RMIServerSocketFactory ssf = null;

        if (useSsl || useRegistrySsl) {
            csf = new JGDISslRMIClientSocketFactory(url.getHost(), serverPort, JGDIAgent.getCaTop());
            ssf = new JGDISslRMIServerSocketFactory(url.getHost(), serverPort, JGDIAgent.getCaTop(), enabledCipherSuites,
                    enabledProtocols, sslNeedClientAuth);
        }

        if (useSsl) {
            env.put(
                    RMIConnectorServer.RMI_CLIENT_SOCKET_FACTORY_ATTRIBUTE,
                    csf);
            env.put(
                    RMIConnectorServer.RMI_SERVER_SOCKET_FACTORY_ATTRIBUTE,
                    ssf);
        }

        JMXConnectorServer connServer = null;
        try {
            connServer = JMXConnectorServerFactory.newJMXConnectorServer(url, env, mbs);
            connServer.start();
        } catch (IOException e) {
            if (connServer == null) {
                throw new AgentConfigurationError(
                        CONNECTOR_SERVER_IO_ERROR, e, url.toString());
            } else {
                throw new AgentConfigurationError(
                        CONNECTOR_SERVER_IO_ERROR, e, connServer.getAddress().toString());
            }
        }

        if (useRegistrySsl) {
            if (registry == null) {
                try {
                    registry = LocateRegistry.createRegistry(serverPort, csf, ssf);
                    registry.bind("jmxrmi", exporter.firstExported);
                } catch (RemoteException ex) {
                   throw new AgentConfigurationError(
                           CONNECTOR_SERVER_IO_ERROR, ex, ex.getMessage());
                } catch (AlreadyBoundException ex) {
                   throw new AgentConfigurationError(
                           CONNECTOR_SERVER_IO_ERROR, ex, ex.getMessage());
                }
            } else {
                registry.rebind("jmxrmi", exporter.firstExported);
            }
        } else {
            if (registry == null) {
                try {
                    registry = LocateRegistry.createRegistry(serverPort);
                    registry.bind("jmxrmi", exporter.firstExported);
                } catch (RemoteException ex) {
                   throw new AgentConfigurationError(
                           CONNECTOR_SERVER_IO_ERROR, ex, ex.getMessage());
                } catch (AlreadyBoundException ex) {
                   throw new AgentConfigurationError(
                           CONNECTOR_SERVER_IO_ERROR, ex, ex.getMessage());
                }
            } else {
                registry.rebind("jmxrmi", exporter.firstExported);
            }
        }

        /* Our exporter remembers the first object it was asked to
        export, which will be an RMIServerImpl appropriate for
        publication in our special registry.  We could
        alternatively have constructed the RMIServerImpl explicitly
        and then constructed an RMIConnectorServer passing it as a
        parameter, but that's quite a bit more verbose and pulls in
        lots of knowledge of the RMI connector.  */

        return connServer;
    }

    public static void resetRegistry() {
        try {
            exporter.unexportObject(exporter.firstExported, true);
            ConnectorBootstrap.registry.unbind("jmxrmi");
        } catch (RemoteException ex) {
            throw new AgentConfigurationError(
                           CONNECTOR_SERVER_IO_ERROR, ex, ex.getMessage());
        } catch (NotBoundException ex) {
            throw new AgentConfigurationError(
                           CONNECTOR_SERVER_IO_ERROR, ex, ex.getMessage());
        }
    }

    /**
     * This class cannot be instantiated.
     **/
    private ConnectorBootstrap() {
    }
}
