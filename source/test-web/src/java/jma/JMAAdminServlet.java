/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.IOException;
import java.io.PrintWriter;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.dom4j.Document;

/**
 *
 * @author vernkin
 */
public class JMAAdminServlet  extends HttpServlet {

    /**
     * Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    /**
     * Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setCharacterEncoding("utf-8");
        String action = request.getParameter("a");
        if(action == null)
        {
            toClient(response.getWriter(), "Invalid Request");
            return;
        }

        if(action.equals("stat"))
        {
            Document doc = ComparisonXmlUpdator.collectAllModifiedSentence();
            response.setContentType("application/octet-stream");
            response.setHeader("Content-Disposition", "attachment; filename=\"error_stat.xml\"");
            toClient(response.getWriter(), doc.asXML());
            return;
        }

        toClient(response.getWriter(), "Unknown Request Action " + action);
    }

    private void toClient(PrintWriter pw, String msg) {
        pw.print(msg);
        pw.flush();
        pw.close();
    }
}
