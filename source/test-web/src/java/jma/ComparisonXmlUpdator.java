/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

/**
 *
 * @author vernkin
 */
public class ComparisonXmlUpdator {
    public static void updateComparisonXml(Document doc)
    {
        Element root = doc.getRootElement();
        Element differs = root.element("differs");
        Element stat = root.element("stat");
        if(stat == null)
            stat = root.addElement("stat");

        int upTotal = 0;
        int downTotal = 0;
        int sameTotal = 0;
        int sameError = 0;
        int upDiffError = 0;
        int downDiffError = 0;

        Iterator<Element> senItr = (Iterator<Element>)differs.elementIterator();
        while(senItr.hasNext())
        {
            Iterator<Element>  blocks = (Iterator<Element>)senItr.next().elementIterator();
            // ignore the origText, suggestion, N-best and feedback
            for(int i=0;i<4;++i)
                blocks.next();
            while(blocks.hasNext())
            {
                Element block = blocks.next();
                if(block.getName().equals("single"))
                {
                    ++sameTotal;
                    if(block.attribute("error").getValue().equals("1"))
                        ++sameError;
                }
                else if(block.getName().equals("double"))
                {
                    Element up = block.element("up");
                    Element down = block.element("down");

                    Iterator<Element> smallItr = (Iterator<Element>)up.elementIterator();
                    while(smallItr.hasNext())
                    {
                        ++upTotal;
                        if(smallItr.next().attribute("error").getValue().equals("1"))
                            ++upDiffError;
                    }

                    smallItr = (Iterator<Element>)down.elementIterator();
                    while(smallItr.hasNext())
                    {
                        ++downTotal;
                        if(smallItr.next().attribute("error").getValue().equals("1"))
                            ++downDiffError;
                    }
                }
            }
        }

        addXmlTextNode(stat, "upTotal", String.valueOf(upTotal));
        addXmlTextNode(stat, "downTotal", String.valueOf(downTotal));
        addXmlTextNode(stat, "sameTotal", String.valueOf(sameTotal));
        addXmlTextNode(stat, "sameError", String.valueOf(sameError));
        addXmlTextNode(stat, "upDiffError", String.valueOf(upDiffError));
        addXmlTextNode(stat, "downDiffError", String.valueOf(downDiffError));
    }

    private static void addXmlTextNode(Element fatherEle, String childName, String val)
    {
        Element ele = fatherEle.element(childName);
        if(ele != null)
        {
            ele.setText(val);
        }
        else
        {
            fatherEle.addElement(childName).addText(val);
        }
    }


    /**
     * Update All xml Counter
     */
    public static synchronized void updateAllXmlCounter()
    {
        File root = new File(StatisticManager.DB_PATH);
        File[] children = root.listFiles();
        int id = 0;
        for(File child : children)
        {
            try{
                id = Integer.parseInt(child.getName());
            }catch(NumberFormatException e){
                //only care about the directories with number as name
                continue;
            }
            updateXmlCounter(child, id);
        }
    }

    private static void updateXmlCounter(File father, int index)
    {
        File diffFile = new File(father, StatisticManager.DIFF_FILE_NAME);
        if(!diffFile.exists())
            return;
        try{
            Document d = FreqUtil.createDocument(diffFile);
            updateComparisonXml(d);
            FreqUtil.writeStringToFile(diffFile, d.asXML());

            // update the global statistic manager
            StatisticManager statManager = StatisticManager.getInstance();
            if (statManager.hasRecord(index) == false) {
                System.err.println(new java.util.Date());
                System.err.println("ComparisonXmlUpdator request to update record id " + index + ", which does not exist in StatisticManager.");
                return;
            }
            statManager.getRecord(index).setFromDocument(d);
        }catch(Exception ex){
            System.err.println("Error in update File " + diffFile +
                    " : " + ex.getMessage());
            ex.printStackTrace();;
        }
    }

    /**
     * Whether user do something on this sentence
     * @param sentEle
     * @return
     */
    private static boolean isModifiedSentence(Element sentEle)
    {
        Iterator<Element>  blocks = (Iterator<Element>)sentEle.elementIterator();
        blocks.next(); //origText
        if(blocks.next().getTextTrim().length() > 0) // suggestion
            return true;
        blocks.next(); //n-best
        if(blocks.next().getTextTrim().length() > 0) // feedback
            return true;
        
        while(blocks.hasNext())
        {
            Element block = blocks.next();
            if(block.getName().equals("single"))
            {
                if(block.attribute("error").getValue().equals("1"))
                    return true;
            }
            else if(block.getName().equals("double"))
            {
                Element up = block.element("up");
                Element down = block.element("down");

                Iterator<Element> smallItr = (Iterator<Element>)up.elementIterator();
                while(smallItr.hasNext())
                {
                    if(smallItr.next().attribute("error").getValue().equals("1"))
                        return true;
                }

                smallItr = (Iterator<Element>)down.elementIterator();
                while(smallItr.hasNext())
                {
                    if(smallItr.next().attribute("error").getValue().equals("1"))
                        return true;
                }
            }
        }

        return false;
    }


    private static void collectModifedSentencesForDoc(Document doc, Element destEle)
    {
        Element root = doc.getRootElement();
        Element differs = root.element("differs");

        Iterator<Element> senItr = (Iterator<Element>)differs.elementIterator();
        while(senItr.hasNext())
        {
            Element sen = senItr.next();
            if(isModifiedSentence(sen))
            {
                destEle.add((Element)sen.clone());
            }
        }
    }


    public synchronized Document collectAllModifiedSentence()
    {
        File root = new File(StatisticManager.DB_PATH);
        File[] children = root.listFiles();
        List<File> diffFiles = new ArrayList<File>(children.length);
        for(File child : children)
        {
            try{
                Integer.parseInt(child.getName());
            }catch(NumberFormatException e){
                //only care about the directories with number as name
                continue;
            }
            File diffFile = new File(child, StatisticManager.DIFF_FILE_NAME);
            if(diffFile.exists())
                diffFiles.add(diffFile);
        }

        return collectModifedSentences(diffFiles);
    }


    public static Document collectModifedSentences(List<File> files)
    {
        Document doc = DocumentHelper.createDocument();
        Element root = doc.addElement( "jmacomp" );
	    Element differs = root.addElement( "differs" );
        Element stat = root.addElement( "stat" );
        stat.addElement("id").addText("-1");
        stat.addElement("name").addCDATA("Modified Sentence");

        for(File file : files)
        {
            if(!file.exists())
                continue;
            try{
                Document d = FreqUtil.createDocument(file);
                collectModifedSentencesForDoc(d, differs);
            }catch(Exception ex){
                System.err.println("Error in collect File " + file +
                        " : " + ex.getMessage());
                ex.printStackTrace();;
            }
        }

        return doc;
    }

    public static void main(String[] args) throws Exception
    {
        File f = new File("/home/vernkin/temp/seg-diff.xml");
        /*
         Document d = FreqUtil.createDocument(f);
        updateComparisonXml(d);
        System.out.println(d.asXML());
         */

        List<File> files = new ArrayList<File>();
        files.add(f);
        Document dd = collectModifedSentences(files);
        System.out.println(dd.asXML());
    }

}
