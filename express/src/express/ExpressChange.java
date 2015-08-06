package express;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

public class ExpressChange {
	
	private static String infile="C:\\TestData\\express\\exp.txt";
	private static String outfile="C:\\TestData\\express\\newexpress.txt";
	private static final int expLength = 10000;
	
	
	@SuppressWarnings("resource")
	private static String newExpress() {
		BufferedReader br = null;
		BufferedWriter bw = null;
		String exp;
		try {
			//System.out.println("start");
			br = new BufferedReader(new InputStreamReader(new FileInputStream(infile),"UTF-8"));
			bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(outfile),"UTF-8"));
			
			while((exp = br.readLine()) != null) {
				System.out.println("exp= "+exp+"---");
				String javaExp = changeToJavaExp(exp);
				System.out.println("javaexp= "+javaExp+"***");
				String newExp = changeExpressToChar(javaExp);
				System.out.println("newExp="+newExp+"====");
				bw.write(newExp); 
				bw.newLine();
			}
			bw.flush();
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			try {
				if(br != null) {					
					br.close();
				}
				if(bw != null) {
					bw.close();
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return null;	
	}
	
	private static String changeExpressToChar(String exp) {
		int k=0, fromIndex=0;
		int srcPos=0, destPos=0;
		char[] str = new char[expLength];
		char[] e = exp.toCharArray();
		int strLength = exp.length();
		while(fromIndex < exp.length()) {
			k = exp.indexOf("\\x", fromIndex);
			if(k != -1) {
				strLength -= 4;
				int num = k-fromIndex-1;
				System.arraycopy(e, srcPos, str, destPos, num);
				destPos += k-fromIndex-1;
				try{
				String s = conventHexToString(exp.substring(k+2, k+4));
				char[] c = s.toCharArray();
				if(c[0] != ' '){
					System.arraycopy(c, 0, str, destPos, s.length());
					destPos += s.length();
				}
				srcPos = fromIndex = k + 4;
				}
				catch(NumberFormatException e1) {
					System.out.println("NumberFormatException:"+e);
				}
			}
			else {
				System.arraycopy(e, srcPos, str, destPos, exp.length()-fromIndex);
				fromIndex = exp.length();
			}
			
		}
		char[] s1 = new char[strLength];
		int i;
		for(i=0; i<strLength; i++){
			s1[i]=str[i];
		}
		String s = new String(s1);
		s.trim();
		return s;
	}
	
	private static String conventHexToString(String str) {
		
		String s=null;
		switch(str){
			case "00": s=null;break;
			case "01": s="s";break;
			case "02": s="s";break;
			case "03": s="e";break;
			case "04": s="o";break;
			case "05": s="q";break;
			case "06": s="k";break;
			case "07": s="l";break;
			case "08": s="b";break;
			case "09": s="t";break;
			case "0a": s="f";break;
			case "0b": s="v";break;
			case "0c": s="f";break;
			case "0d": s="r";break;
			case "0e": s="o";break;
			case "0f": s="i";break;
			case "10": s="e";break;
			case "11": s="1";break;
			case "12": s="d";break;
			case "13": s="d";break;
			case "14": s="d";break;
			case "15": s="n";break;
			case "16": s="s";break;
			case "17": s="b";break;
			case "18": s="c";break;
			case "19": s="m";break;
			case "1a": s="u";break;
			case "1b": s="s";break;
			case "1c": s="s";break;
			case "1d": s="g";break;
			case "1e": s="r";break;
			case "1f": s="u";break;
			case "20": s=" ";break;
			case "21": s="!";break;
			case "22": s="\"";break;
			case "23": s="#";break;
			case "24": s="$";break;
			case "25": s="%";break;
			case "26": s="&";break;
			case "27": s="'";break;
			case "28": s="(";break;
			case "29": s=")";break;
			case "2a": s="*";break;
			case "2b": s="+";break;
			case "2c": s=",";break;
			case "2d": s="-";break;
			case "2e": s=".";break;
			case "2f": s="/";break;
			case "30": s="0";break;
			case "31": s="1";break;
			case "32": s="2";break;
			case "33": s="3";break;
			case "34": s="4";break;
			case "35": s="5";break;
			case "36": s="6";break;
			case "37": s="7";break;
			case "38": s="8";break;
			case "39": s="9";break;
			case "3a": s=":";break;
			case "3b": s=";";break;
			case "3c": s="<";break;
			case "3d": s="=";break;
			case "3e": s=">";break;
			case "3f": s="?";break;
			case "40": s="@";break;
			case "41": s="A";break;
			case "42": s="B";break;
			case "43": s="C";break;
			case "44": s="D";break;
			case "45": s="E";break;
			case "46": s="F";break;
			case "47": s="G";break;
			case "48": s="H";break;
			case "49": s="I";break;
			case "4a": s="J";break;
			case "4b": s="K";break;
			case "4c": s="L";break;
			case "4d": s="M";break;
			case "4e": s="N";break;
			case "4f": s="O";break;
			case "50": s="P";break;
			case "51": s="Q";break;
			case "52": s="R";break;
			case "53": s="S";break;
			case "54": s="T";break;
			case "55": s="U";break;
			case "56": s="V";break;
			case "57": s="W";break;
			case "58": s="X";break;
			case "59": s="Y";break;
			case "5a": s="Z";break;
			case "5b": s="[";break;
			case "5c": s="\\";break;
			case "5d": s="]";break;
			case "5e": s="^";break;
			case "5f": s="_";break;
			case "60": s="'";break;
			case "61": s="a";break;
			case "62": s="b";break;
			case "63": s="c";break;
			case "64": s="d";break;
			case "65": s="e";break;
			case "66": s="f";break;
			case "67": s="g";break;
			case "68": s="h";break;
			case "69": s="i";break;
			case "6a": s="j";break;
			case "6b": s="k";break;
			case "6c": s="l";break;
			case "6d": s="m";break;
			case "6e": s="n";break;
			case "6f": s="o";break;
			case "70": s="p";break;
			case "71": s="q";break;
			case "72": s="r";break;
			case "73": s="s";break;
			case "74": s="t";break;
			case "75": s="u";break;
			case "76": s="v";break;
			case "77": s="w";break;
			case "78": s="x";break;
			case "79": s="y";break;
			case "7a": s="z";break;
			case "7b": s="{";break;
			case "7c": s="|";break;
			case "7d": s="}";break;
			case "7e": s="~";break;
			case "7f": s="d";break;
			default: break;
		}
		return s;
		
	}
	
	private static String changeToJavaExp(String exp) {
		int k=0, fromIndex=0;
		int srcPos=0, destPos=0;
		char[] str = new char[expLength];
		char[] e = exp.toCharArray();
		int strLength = exp.length();
		while(fromIndex < exp.length()) {
			k = exp.indexOf("\\", fromIndex);
			if(k != -1) {
				strLength++;
				System.arraycopy(e, srcPos, str, destPos, k-fromIndex);
				destPos += k - fromIndex;
				char[] c ={'\\','\\'};
				System.arraycopy(c, 0, str, destPos, 2);
				destPos += 2;
				srcPos = fromIndex = k + 1;
			}
			else {
				System.arraycopy(e, srcPos, str, destPos, exp.length()-fromIndex);
				fromIndex = exp.length();
			}
		}
		char[] s1 = new char[strLength];
		int i;
		for(i=0; i<strLength; i++){
			s1[i]=str[i];
		}
		String s = new String(s1);
		
		return s;
	}
	
	public static void main(String[] args) {
		newExpress();
//		String s = changeExpressToChar("^\\x01\\x01\\x05\\x0a");
//		String s1 =changeToJavaExp("^\\x01\\x01\\x05\\x0a");
//		System.out.println("^\\x03[]Z].?.?\\x05$");
//	    System.out.println(s);
//	    System.out.println(s1);
	}
}
