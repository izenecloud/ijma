/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import org.apache.commons.fileupload.*;
import org.apache.commons.fileupload.disk.*;
import org.apache.commons.fileupload.servlet.*;

import javax.servlet.http.HttpServletRequest;
import java.io.File;
import java.util.*;

/**
 * Parse the form field in HTTP request, and save the uploaded file.
 * @author jun
 */
public class JMAFileUploader {
    private FileItem fileItem_; /** the file item to save */

    private String fileName_; /** the name of the file to save */

    private static final int MAX_SIZE = 2097152; /** the maximum size (2M bytes) of file to upload */

    /**
     * Parse the form field in HTTP request and save the file item.
     * @param request the HTTP request
     * @param fieldName the field name in the form request, which field is the file to upload
     * @throws org.apache.commons.fileupload.FileUploadException
     * @throws use.JMAException
     */
    public void parseRequest(HttpServletRequest request, String fieldName) throws FileUploadException, JMAException {
        boolean isMultipart = ServletFileUpload.isMultipartContent(request);
        if (!isMultipart) {
            throw new JMAException("Error: the request contains no multipart content for file uploading.");
        }

        FileItemFactory factory = new DiskFileItemFactory();

        ServletFileUpload upload = new ServletFileUpload(factory);
        upload.setSizeMax(MAX_SIZE);

        List fileItems;
        try {
            fileItems = upload.parseRequest(request);
        } catch (FileUploadException e) {
            throw e;
        }

        // Process the uploaded items
        Iterator iter = fileItems.iterator();
        boolean isFound = false;
        while (iter.hasNext()) {
            FileItem item = (FileItem) iter.next();

            if (!item.isFormField() && item.getFieldName().equals(fieldName)) {
                fileName_ = item.getName();
                fileItem_ = item;
                isFound = true;
                break;
            }
        }

        if(!isFound) {
            throw new JMAException("Error: no field name is " + fieldName + ".");
        }

        if(fileName_.length() == 0) {
            throw new JMAException("Error: no file is selected to upload.");
        }
    }

    /**
     * Get the original filename in the client's filesystem, as provided by
     * the browser (or other client software). In most cases, this will be the
     * base file name, without path information. However, some clients, such as
     * the Opera browser, do include path information.
     * @return the original filename.
     */
    public String getFileName() {
        return fileName_;
    }

    /**
     * Save the file with new name.
     * @param newFile the new name of the file to be saved.
     * @throws java.lang.Exception
     */
    public void saveFile(String newFile) throws Exception {
        fileItem_.write(new File(newFile));
    }
}
