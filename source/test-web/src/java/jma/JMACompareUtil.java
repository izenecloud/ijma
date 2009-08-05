/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jma;

import java.io.BufferedReader;
import java.io.Closeable;
import java.io.File;
import java.io.FileReader;
import java.util.Arrays;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

/**
 *
 * @author Vernkin
 */
public class JMACompareUtil {

	/**
	 * <strong>The file1 and file2 must be encoded with utf8!</strong><br><br>
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
	public static Document generateDiffXml(File file1, File file2) throws Exception
	{
		BufferedReader br1 = null;
		BufferedReader br2 = null;

		try{
			br1 = new BufferedReader(new FileReader(file1));
			br2 = new BufferedReader(new FileReader(file2));

			String line1 = br1.readLine();
			String line2 = br2.readLine();

			Document diffDoc = DocumentHelper.createDocument();
			Element root = diffDoc.addElement( "jmacomp" );
	        Element differs = root.addElement( "differs" );
	        Element statEle = root.addElement( "stat" );

	        int[] stats = new int[3];
	        Arrays.fill(stats, 0);

			while(line1 != null && line2 != null)
			{
				line1 = line1.trim();
				line2 = line2.trim();

				if( (line1.length() == 0 || line2.length() == 0)
						&& line1.length() != line2.length())
					throw new RuntimeException("One line in file1 and file2 are not empty " +
							"in the same time. line1: " + line1 + "; line2: " + line2 + ".");
				if(line1.length() > 0){
					Element senEle = differs.addElement("sentence");
                    try{
                        analysisSentence(line1, line2, senEle, stats);
                    }catch(Throwable t){
                        //remove the senEle
                        differs.remove(senEle);
                    }
                }
				line1 = br1.readLine();
				line2 = br2.readLine();
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
			forceClose(br1);
			forceClose(br2);
		}
	}

	private static String removeAllSpace(String in)
	{
		StringBuffer sb = new StringBuffer(in.length());
		for(int i = 0 ; i < in.length(); ++i )
		{
			char c = in.charAt(i);
			if(c != ' ')
				sb.append(c);
		}
		return sb.toString();
	}


	private static void analysisSentence(String line1, String line2, Element senEle, int[] stats)
	{
		String origText = removeAllSpace(line1);
		//validate the segment result
		if(!origText.equals(removeAllSpace(line2)))
			throw new RuntimeException("File1 (" + line1 + ") or file2 (" + line2 + ") contains" +
					" invalid characters (non-whitespace).");
		senEle.addElement("origText").addCDATA(origText);
		senEle.addElement("userInput").addCDATA("");

		StringTokenizer st1 = new StringTokenizer(line1, " ");
		StringTokenizer st2 = new StringTokenizer(line2, " ");

		stats[0] += st1.countTokens();
		stats[1] += st2.countTokens();

		while(st1.hasMoreTokens() && st2.hasMoreTokens())
		{
			String token1 = st1.nextToken();
			String token2 = st2.nextToken();
			if(token1.length() == token2.length())
			{
				++stats[2];
				senEle.addElement("single").addAttribute("error", "0").addCDATA(token1);
				continue;
			}

			//store the differences
			Element dblEle = senEle.addElement("double");
			Element upEle = dblEle.addElement("up");
			Element downEle = dblEle.addElement("down");
			upEle.addElement("small").addAttribute("error", "0").addCDATA(token1);
			downEle.addElement("small").addAttribute("error", "0").addCDATA(token2);

			int len1 = token1.length();
			int len2 = token2.length();
			while( len1 != len2 )
			{
				if(len1 < len2)
				{
					token1 = st1.nextToken();
					len1 += token1.length();
					upEle.addElement("small").addAttribute("error", "0").addCDATA(token1);
				}
				else
				{
					token2 = st2.nextToken();
					len2 += token2.length();
					downEle.addElement("small").addAttribute("error", "0").addCDATA(token2);
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
		File f1 = new File("basis-utf8.txt");
		File f2 = new File("jma-utf8.txt");
		String ret = generateDiffXml(f1, f2).asXML();
		java.io.PrintWriter pw = new java.io.PrintWriter(new java.io.FileWriter("a.xml"));
		pw.print(ret);
		pw.close();
	}
}
