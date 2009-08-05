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
            out.println("<td rowspan=\"2\" align=\"center\"> <a target=\"_blank\" href=\"jmacomp.html?id="+ id +"\">" + statRecord.getRawName() + "</a> </td>");
        }else {
            out.println("<td rowspan=\"2\" colspan=\"2\" align=\"center\"> SUM </td>");
        }
       
        out.println("<td align=\"center\">Basis</td>");
        out.println("<td align=\"center\">" + statRecord.getUpTotal() + "</td>");
        out.println("<td align=\"center\">" + (int)statRecord.getUpSamePercent() + "%</td>");
        out.println("<td align=\"center\">" + (int)statRecord.getUpAccuracyPercent() + "%</td>");
        out.println("<td align=\"center\">" + (int)statRecord.getUpDiffAccuracyPercent() + "%</td>");
        out.println("<td rowspan=\"2\" align=\"center\">" + (int)statRecord.getSameAccuracyPercent() + "%</td>");
        out.println("</tr>");

        out.println("<tr>");
        out.println("<td align=\"center\" style=\"background:silver;\">iJMA</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + statRecord.getDownTotal() + "</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + (int)statRecord.getDownSamePercent() + "%</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + (int)statRecord.getDownAccuracyPercent() + "%</td>");
        out.println("<td align=\"center\" style=\"background:silver;\">" + (int)statRecord.getDownDiffAccuracyPercent() + "%</td>");
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
            alert("No file is selected, please upload a Japanese text file in UTF-8 encoding.");
            return false;
        }else {
            return true;
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
</div>

<h3>Below is the statistics overview for each comparison result.</h3>

<p>Each ID means a unique index for each comparison.</p>
<p>The sum value of the total statistics is shown in the last row.</p>

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

	<div class="footer">
		<p><a href="http://www.izenesoft.com" target="_blank">Contact</a> | <a href="mailto:vernkin@gmail.com">Advices</a> <br />
		&copy; Copyright 2009 iZENEsoft (Shanghai) Co., Ltd.</p>
	</div>
    </body>
</html>
