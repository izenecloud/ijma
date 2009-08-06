<%-- 
    Document   : index
    Created on : 2009-8-3, 19:34:55
    Author     : jun
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
   <%@page import="jma.StatisticManager"%>
   <%@page import="jma.StatisticRecord"%>
   <%@page import="java.io.IOException"%>
   <%@page import="java.io.PrintWriter"%>
   <%@page import="java.util.Collection"%>
   <%@page import="java.util.Iterator"%>
   <%!
    /** the reference to the shared StatisticManager */
    private StatisticManager statManager_ = StatisticManager.getInstance();
    /** the id value of sum record */
    private static final int SUM_RECORD_ID = -1;

    public void init() throws ServletException {
        statManager_.load();
    }

    private void printStatisticRecord(JspWriter out, StatisticRecord statRecord) throws IOException {
        out.println("<tr>");

        int id = statRecord.getID();
        if(id != SUM_RECORD_ID) {
            out.println("<td rowspan=\"2\" align=\"center\">" + id + "</td>");
            out.println("<td rowspan=\"2\" align=\"center\"> <a href=\"jmacomp.html?id="+ id +"\">" + statRecord.getRawName() + "</a> </td>");
        }else {
            out.println("<td rowspan=\"2\" colspan=\"2\" align=\"center\"> Sum </td>");
        }

        java.text.DecimalFormat percentFormat = new java.text.DecimalFormat("00.00");

        out.println("<td align=\"center\">Basis</td>");
        out.println("<td align=\"center\">" + statRecord.getUpTotal() + "</td>");
        out.println("<td align=\"center\">" + percentFormat.format(statRecord.getUpSamePercent()) + "%</td>");
        out.println("<td align=\"center\" style=\"color:purple\">" + percentFormat.format(statRecord.getUpAccuracyPercent()) + "%</td>");
        out.println("<td align=\"center\">" + percentFormat.format(statRecord.getUpDiffAccuracyPercent()) + "%</td>");
        out.println("<td rowspan=\"2\" align=\"center\">" + percentFormat.format(statRecord.getSameAccuracyPercent()) + "%</td>");
        out.println("<td rowspan=\"2\" align=\"center\">" + percentFormat.format(statRecord.getFScore()) + "%</td>");
        if(id != SUM_RECORD_ID) {
            out.println("<td rowspan=\"2\" align=\"center\"> <a href=\"StatisticRemoveServlet?id="+ id + "\" onclick=\"javascript:return confirm(\'Once the statistic record is deleted, it cannot be recovered.\\nAre you sure to delete it?\')\">delete</a></td>");
        }
        out.println("</tr>");

        out.println("<tr>");
        out.println("<td align=\"center\" style=\"background:silver;\">iJMA</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + statRecord.getDownTotal() + "</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + percentFormat.format(statRecord.getDownSamePercent()) + "%</td>");
        out.println("<td align=\"center\" style=\"color:purple;background:silver;\">" + percentFormat.format(statRecord.getDownAccuracyPercent()) + "%</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + percentFormat.format(statRecord.getDownDiffAccuracyPercent()) + "%</td>");
        out.println("</tr>");
    }
   %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<meta name="author" content="Jun Jiang, Vernkin Chen (www.iZENEsoft.com)" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>JMAs' Comparison</title>
<link href="img/jma_test.css" rel="stylesheet" type="text/css"/>
<script language="JavaScript">
    function isValid(form) {
        if(form.rawfile.value == "") {
            alert("No file is selected,\nplease upload a Japanese text file in UTF-8 encoding.");
            return false;
        }else {
            return true;
        }
    }
function isDelete(idVal) {
    var answer = confirm("Once the record is deleted, it cannot be recovered,\nare you sure for this operation?")
    if (answer){
        alert("Bye bye!")
        window.location = "RemoveRecordServlet?id="+ idVal;
    }
}
</script>
</head>

<body>
<div id="header">
	<div class="header_right">
		<div class="top_info">
			<div class="top_info_right">
				<p>A <b>testing platform</b> supported by <a href="mailto:jun.zju@gmail.com">Jun</a> and <a href="mailto:vernkin@gmail.com">Vernkin</a>, August, 2009</p>
			</div>
		</div>

		<div class="slogan">
			<p>Based on <span>iJMA</span> and <span>Basis's MA</span></p>
		</div>
	</div>

	<div class="logo">
		<h1><a href="#" title="Compare Basis's MA and iJMA!">
			<span class="logoText">JMAs' Comparison</span></a></h1>
	</div>
</div>

<div class="instructionDiv">
<h3>To start evaluation, please upload a Japanese text file in UTF-8 encoding.</h3>
        <form method="POST" action="JMAParserServlet" enctype="multipart/form-data" onSubmit="return isValid(this);">
            <input type="file" name="rawfile">
            <input type="submit" value="Start">
        </form>
<br><br>Example file <a href="utf8_example.txt">utf8_example.txt</a> just for test.
</div>

<h3>Statistics Overview</h3>

        <table border="1">
        <tr>
            <th>ID</th>
            <th>File Name</th>
            <th>JMA</th>
            <th>Tokens Number</th>
            <th>Same Percentage</th>
            <th>Precision</th>
            <th>Precision on Different Tokens</th>
            <th>Precision on Same Tokens</th>
            <th>Comparison F-score</th>
            <th>Operation</th>
        </tr>

        <%
        // statistics sum
        int upTotalSum, downTotalSum, sameTotalSum;
        int upDiffErrorSum, downDiffErrorSum, sameErrorSum;
        upTotalSum = downTotalSum = sameTotalSum = 0;
        upDiffErrorSum = downDiffErrorSum = sameErrorSum = 0;

        Collection c = statManager_.getTotalRecords();
        Iterator iter = c.iterator();
        StatisticRecord statRecord = null;

        //System.out.println(new java.util.Date());
        //System.out.println("in index.jsp, records number: " + c.size());

        while(iter.hasNext()) {
            // get statistic from record
            statRecord = (StatisticRecord) iter.next();

            // sum the value
            upTotalSum += statRecord.getUpTotal();
            downTotalSum += statRecord.getDownTotal();
            sameTotalSum += statRecord.getSameTotal();

            upDiffErrorSum += statRecord.getUpDiffError();
            downDiffErrorSum += statRecord.getDownDiffError();
            sameErrorSum += statRecord.getSameError();

            printStatisticRecord(out, statRecord);
        }

        // create a sum record just to print the sum value
        statRecord = new StatisticRecord(SUM_RECORD_ID);
        statRecord.setTotalValue(upTotalSum, downTotalSum, sameTotalSum);
        statRecord.setErrorValue(upDiffErrorSum, downDiffErrorSum, sameErrorSum);
        printStatisticRecord(out, statRecord);
        %>
        </table>

<br>
<p>The meaning of each column in the above table:</p>
<ul>
    <li> <strong>ID</strong> means a unique index for each comparison result.
    <li> <strong>File Name</strong> is the uploaded file name. <b>By clicking on the file name, you could manual check again on that file.</b>
    <li> <strong>JMA</strong> is the name of Japanese morphological analyzer. There are two JMAs in the comparison. One is <a href="http://www.basistech.com/" target="_blank">Basis</a>, the other is our iJMA (means <a href="http://www.izenesoft.com" target="_blank">iZENEsoft</a> JMA).
    <li> <strong>Token Number</strong> is the total number of tokens in each JMA's output.
    <li> <strong>Same Percentage</strong> means how many tokens in one JMA's output are the same with that of the other JMA.
    <li> <strong style="color:purple">Precision</strong> means how many tokens are correct in the manual check. Please note that if no manual check is ever done, all of the tokens are assumed to be correct on the output of both JMAs.
    <li> <strong>Precision on Different Tokens</strong> means in the manual check, how many tokens are correct on each JMAs' different tokens.
    <li> <strong>Precision on Same Tokens</strong> means in the manual check, how many tokens are correct on each JMAs' same tokens.
    <li> <strong>Comparison F-score</strong> is the harmonic mean of the <strong>Same Percentage</strong> value of two JMAs. This value reveals how similar are those tokens of two JMAs.<br>
         To be more precisely, <i>Comparison F-score</i> = 2 * <i>Same Percentage on Basis</i> * <i>Same Percentage on iJMA</i> / (<i>Same Percentage on Basis</i> + <i>Same Percentage on iJMA</i>).
    <li> <strong>Sum</strong> in the last two rows is the sum of above results for each JMA.
</ul>

	<div class="footer">
		<p><a href="http://www.izenesoft.com" target="_blank">Contact</a> | <a href="mailto:vernkin@gmail.com">Advices</a> <br />
		&copy; Copyright 2009 iZENEsoft (Shanghai) Co., Ltd.</p>
	</div>
    </body>

<%
        String error = request.getParameter("error");
        // request the xml with specific id
        if (error != null && error.equals("outofsize")) {
%>
<script language="javascript" type="text/javascript">
        alert("The file uploaded exceeds maximum size.\nplease upload a file within 5Mega bytes.");
</script>
<%
        }
%>
</html>
