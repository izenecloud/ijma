/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;

/**
 *
 * @author vernkin
 */
public class FreqUtil {

    /**
     * Use UTF-8 Encoding
     * @param src
     * @return
     * @throws java.lang.Exception
     */
    public static String readFileAsString(File src) throws Exception{
        InputStream is = null;
        try {
            byte[] buf = new byte[(int) src.length()];
            is = new FileInputStream(src);
            is.read(buf);

            //System.out.println(new String(buf));
            return new String(buf, "UTF-8");
        } catch (Exception e) {
            System.err.println(new java.util.Date());
            System.err.println("Error in read file :" + e.getMessage());
            throw e;
        } finally {
            forceClose(is);
        }
    }

    /**
     * Use UTF-8 Encoding
     * @param dest
     * @param str
     * @return
     * @throws java.lang.Exception
     */
    public static void writeStringToFile(File dest, String str) throws Exception
    {
        FileOutputStream fstream = null;
        try{
            fstream = new FileOutputStream(dest);
            OutputStreamWriter streamWriter = new OutputStreamWriter(fstream, "UTF-8");
            streamWriter.write(str);
            streamWriter.close();
        }catch(Exception e){
            throw e;
        }finally{
            forceClose(fstream);
        }
    }

    public static Document createDocument(String xmlText) throws Exception
    {

        return DocumentHelper.parseText(xmlText);
    }

    public static Document createDocument(File xmlFile) throws Exception
    {
        return createDocument(readFileAsString(xmlFile));
    }

    public static void forceClose(Closeable stream)
    {
        if(stream == null)
            return;
        try{
            stream.close();
        }catch(Throwable t){};
    }

}
