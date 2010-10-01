/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.IOException;

/**
 *
 * @author jun
 */
public class JMAProcExec {

public static boolean execute(String cmd)
{
    try {
        exec(cmd);
    } catch (Exception e) {
        System.err.println(new java.util.Date());
        System.err.println(e.getMessage());
        return false;
    }

    return true;
}

private static void exec(String cmd) throws Exception
{
    Process proc = null;
    try {
        proc = Runtime.getRuntime().exec(cmd);
    } catch (IOException e) {
        throw e;
    }

    // wait for the execution end
    int exitVal = 0;
    try {
        exitVal = proc.waitFor();
    } catch (InterruptedException e) {
        throw e;
    }

    // check return value
    //int exitVal = proc.exitValue();
    if (exitVal != 0) {
        throw new JMAException("exit value " + exitVal + "in executing " + cmd);
    }
}
}
