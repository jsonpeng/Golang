
<SCRIPT LANGUAGE="JavaScript">
function CreateSelect(_FormName,_SName,_SValue,_Ds,_AllOptionStr,ShowType)
{
    if (_FormName=="")
        _FormName = "all"
    var _DsArr = _Ds.split("|")
    var _Ds1,_Ds2,_Ds3
    var _Ds1 = _DsArr[0]
    var _Ds2 = (_DsArr.length>1)?_DsArr[1]:_DsArr[0]
    var _Ds3 = (_DsArr.length>2)?_DsArr[2]:_DsArr[0]

    var _SNameArr = _SName.split("|")
    var _SName1,_SName2,_SName3
    var _SName1 = _SNameArr[0]
    var _SName2 = (_SNameArr.length>1)?_SNameArr[1]:_SNameArr[0]
    var _SName3 = (_SNameArr.length>2)?_SNameArr[2]:_SNameArr[0]

    var _SValueArr = _SValue.split("|")
    var _SValue1,_SValue2,_SValue3
    var _SValue1 = _SValueArr[0]
    var _SValue2 = (_SValueArr.length>1)?_SValueArr[1]:_SValueArr[0]
    var _SValue3 = (_SValueArr.length>2)?_SValueArr[2]:_SValueArr[0]

    if (ShowType==3){
        _AllOptionStr = _AllOptionStr.replace(/\(\(/ig,"\(\("+_Ds3+"\^")
        _AllOptionStr = _AllOptionStr.replace(/\{\{/ig,"\{\{"+_Ds2+"\(\("+_Ds3+"\*\*")
    }
    else if(ShowType==2){
        _AllOptionStr = _AllOptionStr.replace(/\{\{/ig,"\{\{"+_Ds2+"\(\("+_Ds3+"\*\*")
    }
    else if(ShowType==1){
        _AllOptionStr = _AllOptionStr.replace(/\(\(/ig,"\(\("+_Ds3+"\^")
    }
    var AllStr = _Ds1 + "{{"+ _Ds2 + "(("+ _Ds3 +"||"+_AllOptionStr
    var _AR0 = AllStr.split("||");
    document.writeln("<select name=\"" + _SName1 + "\" size=\"1\" onChange=\""+_SName1+"redirect(this.options.selectedIndex)\">");
    for (var i1 = 0;i1 < _AR0.length;i1++)
    {
        var Area1Str = _AR0[i1];
        var _AR10 = Area1Str.split("{{");
        var _AR11 = _AR10[0].split("@");
        var Tstr1 = _AR11[0];
        var Vstr1 = (_AR11.length==2)?_AR11[1]:_AR11[0]
        document.writeln("<option value=\""+Vstr1+"\">"+Tstr1+"<\/option>");
    }
    document.writeln("<\/select>");

    document.writeln("<select name=\"" + _SName2 + "\" size=\"1\" onChange=\""+_SName1+"redirect1(this.options.selectedIndex)\">");
    var _AR111 = _Ds2.split("@");
    var Tstr11 = _AR111[0];
    var Vstr11 = (_AR111.length==2)?_AR111[1]:_AR111[0];
    document.writeln("<option value=\""+Vstr11+"\">"+Tstr11+"<\/option>");
    document.writeln("<\/select>");

    document.writeln("<select name=\"" + _SName3 + "\" size=\"1\">");
    var _AR222 = _Ds3.split("@");
    var Tstr22 = _AR222[0];
    var Vstr22 = (_AR222.length==2)?_AR222[1]:_AR222[0];
    document.writeln("<option value=\""+Vstr22+"\">"+Tstr22+"<\/option>");
    document.writeln("<\/select>");

    document.writeln("<S"+"CRIPT LANGUAGE=\"JavaScript\" defer>");
    document.writeln("<!--");
    document.writeln("var "+_SName1+"NewAllStr = \""+ AllStr +"\"");
    document.writeln("var "+_SName1+"_AR0 = "+_SName1+"NewAllStr.split(\"||\");");
    document.writeln("var "+_SName1+"groups=document."+ _FormName +"." + _SName1 + ".options.length;");
    document.writeln("var "+_SName1+"group=new Array("+_SName1+"groups)");
    document.writeln("for (i=0; i<"+_SName1+"groups; i++){");
    document.writeln("  "+_SName1+"group[i]=new Array();");
    document.writeln("}");
    document.writeln("for (var i1 = 0;i1 < "+_SName1+"_AR0.length;i1++){");
    document.writeln("  var Area1Str = "+_SName1+"_AR0[i1];");
    document.writeln("  var _AR10 = Area1Str.split(\"{{\");");
    document.writeln("  var _AR12 = _AR10[1].split(\"**\");");
    document.writeln("  for (var i2 = 0;i2 < _AR12.length;i2++){");
    document.writeln("      var Area2Str = _AR12[i2];");
    document.writeln("      var _AR20 = Area2Str.split(\"((\");");
    document.writeln("      var _AR211 = _AR20[0].split(\"@\");");
    document.writeln("      var Tstr2 = _AR211[0];");
    document.writeln("      var Vstr2 = (_AR211.length==2)?_AR211[1]:_AR211[0];");
    document.writeln("      "+_SName1+"group[i1][i2]=new Option(Tstr2,Vstr2);");
    document.writeln("  }");
    document.writeln("}");
    document.writeln("var "+_SName1+"temp = document."+ _FormName +"." + _SName2);
    document.writeln("function "+_SName1+"redirect(x){");
    document.writeln("  for (m="+_SName1+"temp.options.length-1;m>0;m--)");
    document.writeln("  "+_SName1+"temp.options[m]=null;");
    document.writeln("  for (i=0;i<"+_SName1+"group[x].length;i++){");
    document.writeln("      "+_SName1+"temp.options[i]=new Option("+_SName1+"group[x][i].text,"+_SName1+"group[x][i].value);");
    document.writeln("  }");
    document.writeln("  "+_SName1+"temp.options[0].selected=true");
    document.writeln("  "+_SName1+"redirect1(0)");
    document.writeln("}");
    document.writeln("var "+_SName1+"Group2s=document."+ _FormName +"." + _SName2 + ".options.length;");
    document.writeln("var "+_SName1+"Group2=new Array("+_SName1+"groups);");
    document.writeln("for (i=0; i<"+_SName1+"groups; i++){");
    document.writeln("  "+_SName1+"Group2[i]=new Array("+_SName1+"group[i].length)");
    document.writeln("  for (j=0; j<"+_SName1+"group[i].length; j++){");
    document.writeln("      "+_SName1+"Group2[i][j]=new Array()");
    document.writeln("  }");
    document.writeln("}");
    document.writeln("for (var i1 = 0;i1 < "+_SName1+"_AR0.length;i1++){");
    document.writeln("  var Area1Str = "+_SName1+"_AR0[i1]");
    document.writeln("  var _AR10 = Area1Str.split(\"{{\");");
    document.writeln("  var _AR12 = _AR10[1].split(\"**\");");
    document.writeln("  for (var i2 = 0;i2 < _AR12.length;i2++){");
    document.writeln("      var Area2Str = _AR12[i2]");
    document.writeln("      var _AR20 = Area2Str.split(\"((\");");
    document.writeln("      _AR212 = _AR20[1].split(\"^\");");
    document.writeln("      for (var i3 = 0;i3 < _AR212.length;i3++){");
    document.writeln("          Area3Str = _AR212[i3]");
    document.writeln("          _AR3 = Area3Str.split(\"@\");");
    document.writeln("          Tstr3 = _AR3[0]");
    document.writeln("          Vstr3 = (_AR3.length==2)?_AR3[1]:_AR3[0]");
    document.writeln("          "+_SName1+"Group2[i1][i2][i3]=new Option(Tstr3,Vstr3);");
    document.writeln("      }");
    document.writeln("  }");
    document.writeln("}");
    document.writeln("var "+_SName1+"temp1=document."+ _FormName +"." + _SName3 + "");
    document.writeln("function "+_SName1+"redirect1(y){");
    document.writeln("  for (m="+_SName1+"temp1.options.length-1;m>0;m--)");
    document.writeln("  "+_SName1+"temp1.options[m]=null");
    document.writeln("  for (i=0;i<"+_SName1+"Group2[document."+ _FormName +"." + _SName1 + ".options.selectedIndex][y].length;i++){");
    document.writeln("      "+_SName1+"temp1.options[i]=new Option("+_SName1+"Group2[document."+ _FormName +"." + _SName1 + ".options.selectedIndex][y][i].text," + _SName1 + "Group2[document."+ _FormName +"." + _SName1 + ".options.selectedIndex][y][i].value)");
    document.writeln("  }");
    document.writeln("  "+_SName1+"temp1.options[0].selected=true");
    document.writeln("}");
    document.writeln("\/\/-->");
    document.writeln("<\/script>");
    document.writeln("<s"+"cript language=\"JavaScript\">");
    document.writeln("<!--");
    document.writeln("function "+_SName1+"SetValue(){");
    document.writeln("  try{");
    document.writeln("      if (\""+_SValue1+"\"!=\"\"){");
    document.writeln("          document."+ _FormName +"."+_SName1+".value=\""+_SValue1+"\"");
    document.writeln("          "+_SName1+"redirect(document."+ _FormName +"."+_SName1+".options.selectedIndex);");
    document.writeln("          if (\""+_SValue2+"\"!=\"\"){");
    document.writeln("              document."+ _FormName +"."+_SName2+".value=\""+_SValue2+"\"");
    document.writeln("              "+_SName1+"redirect1(document."+ _FormName +"."+_SName2+".options.selectedIndex)");
    document.writeln("              if (\""+_SValue3+"\"!=\"\")");
    document.writeln("                  document."+ _FormName +"."+_SName3+".value=\""+_SValue3+"\"");
    document.writeln("          }");
    document.writeln("      }");
    document.writeln("  }");
    document.writeln("  catch(e){");
    document.writeln("  }");
    document.writeln("}");
    document.writeln("window.attachEvent(\"onload\","+_SName1+"SetValue)");
    document.writeln("\/\/-->");
    document.writeln("<\/script>");
}
</script>

<SCRIPT LANGUAGE="JavaScript">
<!--
var AllStr1 = ""
+"{.node1}{{"
    +"SWITCH_ALL((SWITCH_ALL_SET^SWITCH_ALL_ADD^SWITCH_ALL_EDIT^SWITCH_ALL_EDIT1^SWITCH_ALL_EDIT2^SWITCH_ALL_EDIT3"
    +"**SWITCH_ONE((SWITCH_ONE_SET^SWITCH_ONE_ADD^SWITCH_ONE_EDIT^SWITCH_ONE_EDIT1^SWITCH_ONE_EDIT2^SWITCH_ONE_EDIT3"
    +"**SWITCH_TWO((SWITCH_TWO_SET^SWITCH_TWO__ADD^SWITCH_TWO__EDIT^SWITCH_TWO__EDIT1^SWITCH_TWO__EDIT2^SWITCH_TWO__EDIT3"
+"||{.node2}{{"
    +"SWITCH_ALL((SWITCH_ALL_SET^SWITCH_ALL_ADD^SWITCH_ALL_EDIT^SWITCH_ALL_EDIT1^SWITCH_ALL_EDIT2^SWITCH_ALL_EDIT3"
    +"**SWITCH_ONE((SWITCH_ONE_SET^SWITCH_ONE_ADD^SWITCH_ONE_EDIT^SWITCH_ONE_EDIT1^SWITCH_ONE_EDIT2^SWITCH_ONE_EDIT3"
    +"**SWITCH_TWO((SWITCH_TWO_SET^SWITCH_TWO__ADD^SWITCH_TWO__EDIT^SWITCH_TWO__EDIT1^SWITCH_TWO__EDIT2^SWITCH_TWO__EDIT3"
+"||{.node3}{{"
+"SWITCH_ALL((SWITCH_ALL_SET^SWITCH_ALL_ADD^SWITCH_ALL_EDIT^SWITCH_ALL_EDIT1^SWITCH_ALL_EDIT2^SWITCH_ALL_EDIT3"
    +"**SWITCH_ONE((SWITCH_ONE_SET^SWITCH_ONE_ADD^SWITCH_ONE_EDIT^SWITCH_ONE_EDIT1^SWITCH_ONE_EDIT2^SWITCH_ONE_EDIT3"
    +"**SWITCH_TWO((SWITCH_TWO_SET^SWITCH_TWO__ADD^SWITCH_TWO__EDIT^SWITCH_TWO__EDIT1^SWITCH_TWO__EDIT2^SWITCH_TWO__EDIT3"

+"||{.node4}{{"
+"SWITCH_ALL((SWITCH_ALL_SET^SWITCH_ALL_ADD^SWITCH_ALL_EDIT^SWITCH_ALL_EDIT1^SWITCH_ALL_EDIT2^SWITCH_ALL_EDIT3"
    +"**SWITCH_ONE((SWITCH_ONE_SET^SWITCH_ONE_ADD^SWITCH_ONE_EDIT^SWITCH_ONE_EDIT1^SWITCH_ONE_EDIT2^SWITCH_ONE_EDIT3"
    +"**SWITCH_TWO((SWITCH_TWO_SET^SWITCH_TWO__ADD^SWITCH_TWO__EDIT^SWITCH_TWO__EDIT1^SWITCH_TWO__EDIT2^SWITCH_TWO__EDIT3"

+"||{.node4}{{"
+"SWITCH_ALL((SWITCH_ALL_SET^SWITCH_ALL_ADD^SWITCH_ALL_EDIT^SWITCH_ALL_EDIT1^SWITCH_ALL_EDIT2^SWITCH_ALL_EDIT3"
    +"**SWITCH_ONE((SWITCH_ONE_SET^SWITCH_ONE_ADD^SWITCH_ONE_EDIT^SWITCH_ONE_EDIT1^SWITCH_ONE_EDIT2^SWITCH_ONE_EDIT3"
    +"**SWITCH_TWO((SWITCH_TWO_SET^SWITCH_TWO__ADD^SWITCH_TWO__EDIT^SWITCH_TWO__EDIT1^SWITCH_TWO__EDIT2^SWITCH_TWO__EDIT3"

+"||{.node5}{{"
+"SWITCH_ALL((SWITCH_ALL_SET^SWITCH_ALL_ADD^SWITCH_ALL_EDIT^SWITCH_ALL_EDIT1^SWITCH_ALL_EDIT2^SWITCH_ALL_EDIT3"
    +"**SWITCH_ONE((SWITCH_ONE_SET^SWITCH_ONE_ADD^SWITCH_ONE_EDIT^SWITCH_ONE_EDIT1^SWITCH_ONE_EDIT2^SWITCH_ONE_EDIT3"
    +"**SWITCH_TWO((SWITCH_TWO_SET^SWITCH_TWO__ADD^SWITCH_TWO__EDIT^SWITCH_TWO__EDIT1^SWITCH_TWO__EDIT2^SWITCH_TWO__EDIT3"
    

</SCRIPT>