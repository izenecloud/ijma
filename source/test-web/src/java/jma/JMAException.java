/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

/**
 *
 * @author jun
 */
public class JMAException extends Exception {

    public JMAException() {
        this(null);
    }

    public JMAException(final String msg) {
        super(msg);
    }
}
