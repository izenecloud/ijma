<%-- 
    Document   : admin
    Created on : 2009-8-14, 14:19:44
    Author     : jun
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<%@page import="jma.ComparisonXmlUpdator"%>
<html>
    <head>
        <meta name="author" content="Jun Jiang, Vernkin Chen (www.iZENEsoft.com)" />
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title>JMA Comparison Administration</title>
        <link href="img/jma_test.css" rel="stylesheet" type="text/css"/>
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
		<h1><a href="index.jsp" title="Compare Basis's MA and iJMA!">
			<span class="logoText">JMAs' Comparison</span></a></h1>
	</div>
</div>

        <h3>JMA Comparison Administration</h3>

        <table border="1">
        <tr>
            <th>Operation</th>
            <th>Description</th>
        </tr>
        <tr>
            <td><a href="admin.jsp?opt=1"> Recompute </a></td>
            <td>recompute the statistics in all of the comparison result, and update into result files</td>
        </tr>
        <tr>
            <td><a href="admin.jsp?opt=2"> Download </a></td>
            <td>download the sentences corrected by user</td>
        </tr>
        </table>

<br>
<%
        String optIndexStr = request.getParameter("opt");
        if (optIndexStr != null) {
            int optIndex = Integer.parseInt(optIndexStr);
            switch(optIndex) {
                case 1:
                    ComparisonXmlUpdator.updateAllXmlCounter();
                    out.println("Operation result:<br>the statistics are updated.");
                    break;

                case 2:
                    ComparisonXmlUpdator updater = new ComparisonXmlUpdator();
                    updater.collectAllModifiedSentence();
                    out.println("Operation result:<br>the modified sentences file is downloaded.");
                    break;

                default:
%>
<script language="javascript" type="text/javascript">
        alert("The operation code <%= optIndex %> is not valid.");
</script>
<%
                    break;
            }
       }
%>

	<div class="footer">
		<p><a href="http://www.izenesoft.com" target="_blank">Contact</a> | <a href="mailto:vernkin@gmail.com">Advices</a> <br />
		&copy; Copyright 2009 iZENEsoft (Shanghai) Co., Ltd.</p>
	</div>
    </body>
</html>
