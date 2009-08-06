/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.io.SAXReader;
import org.dom4j.Element;
import org.dom4j.DocumentHelper;

import java.io.FileWriter;
import java.io.IOException;
import java.util.TreeMap;
import java.util.Collection;
import java.util.Iterator;

/**
 *
 * @author jun
 */
public class StatisticManager {
    private static final String WEB_TEST_PATH = "/home/jun/git/jma/bin/web_test/"; /** the web test path containing script and data files */
    public static final String DB_PATH = WEB_TEST_PATH + "db/"; /** the path of data files */
    public static final String SCRIPT_FILE_PATH = WEB_TEST_PATH + "go_jma.sh"; /** the path of script file */
    public static final String DIFF_FILE_NAME = "diff.xml"; /** the name of the difference file */
    
    private final String DB_FILE_NAME = "db.xml"; /** the file name of the database file */
    private static StatisticManager instance_ = new StatisticManager(); /** the only instance */
    private TreeMap map_ = new TreeMap(); /** the map stores each statistic record */

    /**
     * Constructor.
     */
    private StatisticManager() {
    }

    /**
     * Get the instance.
     * @return the only instance.
     */
    public static StatisticManager getInstance() {
        return instance_;
    }

    /**
     * Load data from file.
     */
    public void load() {
        //System.out.println(new java.util.Date());
        //System.out.println("StatisticManager.load()");
        SAXReader reader = new SAXReader();
        Document dbDoc;
        try {
            dbDoc = reader.read(DB_PATH + DB_FILE_NAME);
        } catch(DocumentException e) {
            System.err.println(new java.util.Date());
            System.out.println("StatisticManager.load(): " + DB_PATH + DB_FILE_NAME + " not exists, just create it.");
            // no such file exists, just create it with the root node.
            dbDoc = DocumentHelper.createDocument();
            dbDoc.addElement("Collection");
        }
        //System.out.println(dbDoc.asXML());

        Element dbRoot = dbDoc.getRootElement();

        // read xml content into map_
        // iterate through child elements of root
        for (Iterator i = dbRoot.elementIterator("Record"); i.hasNext();) {
            Element recordElement = (Element) i.next();
            Integer id = new Integer(recordElement.elementText("ID"));
            StatisticRecord statRecord = new StatisticRecord();

            // read statistic values from diff
            Document diffDoc;
            String diffFile = DB_PATH + id + "/" + DIFF_FILE_NAME;
            try {
                diffDoc = reader.read(diffFile);
            } catch (DocumentException e) {
                // no such file exists, just ignore this record
                System.err.println(new java.util.Date());
                System.err.println("StatisticManager.load(): error in reading file: " + diffFile);
                continue;
            }

            statRecord.setFromDocument(diffDoc);
            map_.put(id, statRecord);
        }
        //System.out.println("records number: " + map_.size());
        //System.out.println();
    }

    /**
     * Save data to file.
     */
    synchronized public void save() throws IOException {
        // create document from map_
        Document dbDoc = DocumentHelper.createDocument();
        Element dbRoot = dbDoc.addElement("Collection");

        Collection c = map_.values();
        Iterator iter = c.iterator();
        while(iter.hasNext()) {
            StatisticRecord statRecord = (StatisticRecord) iter.next();
            Element recordElement = dbRoot.addElement("Record");
            recordElement.addElement("ID").addText(Integer.toString(statRecord.getID()));
        }

        // write document to file
        FileWriter fwriter = new FileWriter(DB_PATH + DB_FILE_NAME);
        dbDoc.write(fwriter);
        fwriter.close();
    }

    /**
     * Get record according to its ID value.
     * @param id the ID value
     * @return the record reference.
     */
    public StatisticRecord getRecord(int id) {
        return (StatisticRecord) map_.get(new Integer(id));
    }

    /**
     * Returns true if this ID exists.
     * @param id the ID value
     * @return true for ID exists, false for not exists.
     */
    public boolean hasRecord(int id) {
        return map_.containsKey(new Integer(id));
    }

    /**
     * Get the collection of all records.
     * @return the records collection.
     */
    public Collection getTotalRecords() {
        return map_.values();
    }

    /**
     * Create an empty record with new ID.
     * @return the new record created.
     */
    synchronized public StatisticRecord createNewRecord() {
        int id = map_.isEmpty() ? 0 : ((Integer) map_.lastKey()).intValue();
        ++id;
        StatisticRecord statRecord = new StatisticRecord(id);
        map_.put(new Integer(id), statRecord);
        return statRecord;
    }

    /**
     * Remove a record according to its ID value.
     * @param id the ID value
     */
    synchronized public void removeRecord(int id) {
        map_.remove(new Integer(id));
    }
}
