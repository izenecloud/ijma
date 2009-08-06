/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.File;
import java.io.FileWriter;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.dom4j.Element;

/**
 *
 * @author jun
 */
public class JMAParserServlet extends HttpServlet {
    private StatisticManager statManager_ = StatisticManager.getInstance(); /** the reference to the shared StatisticManager */

    /** 
     * Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        // genreate new record
        StatisticRecord statRecord = statManager_.createNewRecord();
        int id = statRecord.getID();

        // save the file user uploaded
        JMAFileUploader uploader = new JMAFileUploader();
        try {
            uploader.parseRequest(request, "rawfile");
        } catch (Exception e) {
            System.err.println(new java.util.Date());
            System.err.println(e.getMessage());
            
            // remove the record if error happened
            statManager_.removeRecord(id);

            // return to index page
            response.sendRedirect("index.jsp?error=outofsize");
        }
        
        try {
            statRecord.setRawName(uploader.getFileName());
            String newName = StatisticManager.DB_PATH + id + ".txt";
            uploader.saveFile(newName);
//            out.println("succeed in uploading file to " + newName + ".<br>");

            // execute script
            String cmd = StatisticManager.SCRIPT_FILE_PATH + " " + id + " " + statRecord.getRawName();
            boolean retVal = JMAProcExec.execute(cmd);
            if (!retVal) {
                throw new JMAException("failed in script execution.");
            }
//            out.println("succeed in script execution: " + cmd + ".<br>");

            // generate comparison result
            File f1 = new File(StatisticManager.DB_PATH + id + "/basis_utf8.txt");
            File f2 = new File(StatisticManager.DB_PATH + id + "/jma_utf8.txt");
            Document diffDoc = JMACompareUtil.generateDiffXml(f1, f2);
            Element statElem = diffDoc.getRootElement().element("stat");

            // set id and name
            statElem.element("id").setText(Integer.toString(id));
            statElem.element("name").setText(statRecord.getRawName());

            // save to comparison file
            PrintWriter pw = new PrintWriter(new FileWriter(StatisticManager.DB_PATH + id + "/" + StatisticManager.DIFF_FILE_NAME));
            pw.print(diffDoc.asXML());
            pw.close();

            // set statistic result
            statRecord.setFromDocument(diffDoc);
//            out.println("succeed to save comparison result to: " + StatisticManager.DIFF_FILE_NAME + ".<br>");

            // save to statistic db file
            statManager_.save();
            
            response.sendRedirect("jmacomp.html?id=" + id);
        } catch (Exception e) {
            System.err.println(new java.util.Date());
            System.err.println(e.getMessage());
            
            // remove the record if error happened
            statManager_.removeRecord(id);
            
            // print the error message
            response.setContentType("text/html;charset=UTF-8");
            PrintWriter out = response.getWriter();
            out.println("<html>");
            out.println("<head>");
            out.println("<title>Servlet JMAParser</title>");
            out.println("</head>");
            out.println("<body>");
            out.println("Error in parsing this file.<br>");
            out.println("Click <a href=\"index.jsp\">here</a> to go back.<br><br>");
            out.println("Below is the error information for developers to debug.<br>");
            out.println("ID value: " + id + "<br>");
            out.println("error message: " +e.getMessage() + "<br>");
            out.println("</body>");
            out.println("</html>");
        }
    }

    // <editor-fold defaultstate="collapsed" desc="HttpServlet 方法。单击左侧的 + 号以编辑代码。">
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

    /** 
     * Returns a short description of the servlet.
     * @return a String containing servlet description
     */
    public String getServletInfo() {
        return "Short description";
    }// </editor-fold>

}
