/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.BufferedReader;
import java.io.Closeable;
import java.io.File;
import java.util.Arrays;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;


/**
 *
 * @author Vernkin
 */
public class JMACompareUtil {
    
    private static final String CHAR_ENCODE = "UTF-8"; /** UTF-8 character encoding */

    private static class WordPOS
    {

        /** only include word */
        public String word;
        
        /** include word and origin */
        public String origin;

        /**
         *
         * @param origText include or not include POS, if include, separeted by "/"
         */
        public WordPOS(String origText)
        {
            origin = origText;
            int posIdx = origText.lastIndexOf('/');
            if(posIdx < 0)
                posIdx = origText.length();
            word = origText.substring(0, posIdx);
        }

        public String getPOS()
        {
            int posIdx = origin.lastIndexOf('/');
            if(posIdx < 0)
                return "";
            return origin.substring(posIdx + 1);
        }
    }


    private static String getJMANBestString(String in)
    {
        StringBuffer sb = new StringBuffer(in.length() + 20);
        int idx = in.indexOf(' ');
        sb.append("<score>").append(in.substring(0, idx).trim()).
                append("</score>").append(in.substring(idx+1).trim());
        return sb.toString();
    }

	/**
	 * <strong>The basisFile and jmaFile must be encoded with utf8!</strong><br><br>
	 *
	 * The root node of the xml is jmacomp.<BR>
	 *
	 * Node jmacomp/stat is the statistical information.Under that node, one sample is:
	 * <br>
	 * &lt;stat&gt; <br>
	 * &nbsp;&nbsp;&nbsp;&lt;upTotal&gt;580&lt;/upTotal&gt; <br>
	 * &nbsp;&nbsp;&nbsp;&lt;downTotal&gt;648&lt;/downTotal&gt; <br>
	 * &nbsp;&nbsp;&nbsp;&lt;sameTotal&gt;492&lt;/sameTotal&gt; <br>
	 * &nbsp;&nbsp;&nbsp;&lt;sameError&gt;0&lt;/sameError&gt; <br>
	 * &nbsp;&nbsp;&nbsp;&lt;upDiffError&gt;0&lt;/upDiffError&gt; <br>
	 * &nbsp;&nbsp;&nbsp;&lt;downDiffError&gt;0&lt;/downDiffError&gt; <br>
	 * &lt;/stat&gt; <br>
	 *
	 * <b>upTotal</b> is the number of tokens of the file1. <br>
	 * <b>downTotal</b> is the number of tokens of the file2. <br>
	 * <b>sameTotal</b> is the number of the same tokens that file1 and file2 have. <br>
	 * <b>sameError</b> is the number of the error tokens in the same tokens. <br>
	 * <b>upDiffError</b> is the number of the error token in file1's different tokens (those
	 * belong to file1 and not same tokens). <br>
	 * <b>downDiffError</b> is the number of the error token in file2's different tokens (those
	 * belong to file2 and not same tokens). <br>
	 *
	 * @param file1 the output of the first JMA
	 * @param file2 the output of the second JMA
	 * @return the comparison xml document
	 */
	public static Document generateDiffXml(File basisFile, File jmaFile) throws Exception
	{
		BufferedReader basisBr = null;
		BufferedReader jmaBr = null;

		try{
//			br1 = new BufferedReader(new FileReader(file1));
//			br2 = new BufferedReader(new FileReader(file2));
            // to avoid using default character encoding in reading file, use InputStreamReader instead of FileReader
            FileInputStream fstream1 = new FileInputStream(basisFile);
            basisBr = new BufferedReader(new InputStreamReader(fstream1, CHAR_ENCODE));

            FileInputStream fstream2 = new FileInputStream(jmaFile);
            jmaBr = new BufferedReader(new InputStreamReader(fstream2, CHAR_ENCODE));


			String basisLine = basisBr.readLine();
			String jmaLine = jmaBr.readLine();

			Document diffDoc = DocumentHelper.createDocument();
			Element root = diffDoc.addElement( "jmacomp" );
	        Element differs = root.addElement( "differs" );
	        Element statEle = root.addElement( "stat" );

	        int[] stats = new int[3];
	        Arrays.fill(stats, 0);

			while(basisLine != null && jmaLine != null)
			{
				basisLine = basisLine.trim();
                jmaLine = jmaLine.trim();

                if( (basisLine.length() == 0 || jmaLine.length() == 0)
						&& basisLine.length() != jmaLine.length())
					throw new RuntimeException("One line in file1 and file2 are not empty " +
							"in the same time. line1: " + basisLine + "; line2: " + jmaLine + ".");

				
				if(basisLine.length() > 0){
                    int nBestNum = Integer.parseInt(jmaLine);
                    jmaLine = jmaBr.readLine();

                    StringBuffer nBestStr = new StringBuffer(nBestNum * 256);
                    nBestStr.append("<li>").append(
                            getJMANBestString(jmaLine)).append("</li>");
                    for(int nbi = nBestNum - 1; nbi > 0; --nbi)
                    {
                        nBestStr.append("<li>").append(getJMANBestString(
                                jmaBr.readLine())).append("</li>");
                    }

                    //remove the score of the first result
                    jmaLine = jmaLine.substring(jmaLine.indexOf(' ') + 1).trim();

					Element senEle = differs.addElement("sentence");
                    try{
                        analysisSentence(basisLine, jmaLine, nBestStr.toString(),
                                senEle, stats);
                    }catch(Throwable t){
                        System.err.println(new java.util.Date());
                        System.err.println(t.getMessage());
                        //remove the senEle
                        differs.remove(senEle);
                    }
                }
				basisLine = basisBr.readLine();
				jmaLine = jmaBr.readLine();
			}

			statEle.addElement("id").addText("0");
			statEle.addElement("name").addText("NULL");
			statEle.addElement("upTotal").addText(String.valueOf(stats[0]));
			statEle.addElement("downTotal").addText(String.valueOf(stats[1]));
			statEle.addElement("sameTotal").addText(String.valueOf(stats[2]));
			statEle.addElement("sameError").addText("0");
			statEle.addElement("upDiffError").addText("0");
			statEle.addElement("downDiffError").addText("0");
			return diffDoc;
		}catch(Exception t){
			throw t;
		}finally{
			forceClose(basisBr);
			forceClose(jmaBr);
		}
	}

	private static String removeSpaceAndPOS(String in, List<WordPOS> wpList)
	{
		StringTokenizer st = new StringTokenizer(in, " ");
        StringBuffer sb = new StringBuffer((int)Math.round(in.length()/1.5));
		while(st.hasMoreTokens())
        {
            WordPOS wp = new WordPOS(st.nextToken());
            wpList.add(wp);
            sb.append(wp.word);
        }
        
		return sb.toString();
	}


	private static void analysisSentence(String basisLine, String jmaLine,
            String jmaNBest, Element senEle, int[] stats)
	{
		ArrayList<WordPOS> basisWP = new ArrayList<WordPOS>();
        String basisOrigText = removeSpaceAndPOS(basisLine, basisWP);
		
        ArrayList<WordPOS> jmaWP = new ArrayList<WordPOS>();
        String jmaOrigText = removeSpaceAndPOS(jmaLine, jmaWP);

        //validate the segment result
		if(!basisOrigText.equals(jmaOrigText))
			throw new RuntimeException("File1 (" + basisLine + ") or file2 (" + jmaLine + ") contains" +
					" invalid characters (non-whitespace).");
		senEle.addElement("origText").addCDATA(basisOrigText);
		senEle.addElement("userInput").addCDATA("");
        senEle.addElement("jmaNBest").addCDATA(jmaNBest);
        senEle.addElement("feedback").addCDATA("");

		stats[0] += basisWP.size();
		stats[1] += jmaWP.size();

        Iterator<WordPOS> basisItr = basisWP.iterator();
        Iterator<WordPOS> jmaItr = jmaWP.iterator();

		while(basisItr.hasNext() && jmaItr.hasNext())
		{
			WordPOS bwp = basisItr.next();
            WordPOS jwp = jmaItr.next();

            if(bwp.word.length() == jwp.word.length())
			{
				++stats[2];
                String text = bwp.origin;
                if(!text.equals(jwp.origin))
                    text += ", " + jwp.getPOS();
                senEle.addElement("single").addAttribute("error", "0").
                        addCDATA(text);
				continue;
			}

			//store the differences
			Element dblEle = senEle.addElement("double");
			Element upEle = dblEle.addElement("up");
			Element downEle = dblEle.addElement("down");
			upEle.addElement("small").addAttribute("error", "0").addCDATA(bwp.origin);
			downEle.addElement("small").addAttribute("error", "0").addCDATA(jwp.origin);

			int len1 = bwp.word.length();
			int len2 = jwp.word.length();
			while( len1 != len2 )
			{
				if(len1 < len2)
				{
					bwp = basisItr.next();
                    len1 += bwp.word.length();
					upEle.addElement("small").addAttribute("error", "0").addCDATA(bwp.origin);
				}
				else
				{
					jwp = jmaItr.next();
                    len2 += jwp.word.length();
					downEle.addElement("small").addAttribute("error", "0").addCDATA(jwp.origin);
				}
			}

		}
	}


	public static void forceClose(Closeable stream)
	{
		if(stream != null)
		{
			try{
				stream.close();
			}catch(Throwable t){}
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception{
		File f1 = new File("/home/vernkin/temp/seg1.txt");
		File f2 = new File("/home/vernkin/temp/seg2.txt");
		String ret = generateDiffXml(f1, f2).asXML();
		java.io.PrintWriter pw = new java.io.PrintWriter(new java.io.FileWriter("/home/vernkin/temp/seg-diff.xml"));
		pw.print(ret);
		pw.close();
	}
}
