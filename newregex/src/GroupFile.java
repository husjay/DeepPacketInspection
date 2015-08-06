import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class GroupFile {
	private static int[] location = null;
	private static BufferedWriter[] bw = null;
	public static void main(String[] a){
		readGroupResult("C:\\TestData1\\groupResult\\groupResult.txt");
		groupRegex("C:\\TestData1\\regex0.txt");
	}
	private static void readGroupResult(String groupResultFile){//读分组结果写入文件中
		int lineNum = 0;
		BufferedReader fr=null;
		try{
			fr =new BufferedReader( new FileReader(groupResultFile));
			location = new int[Integer.parseInt(fr.readLine())];
			String line;
			while(true){
				line =fr.readLine();
				if(line==null){
					break;
				}
				if(line.equals("")){
					System.out.println(line);
					lineNum++;
					continue;
				}
				System.out.println(line);
				String[] lineArr = line.split(" ");
				for(int i = 0;i<lineArr.length;i++){
					location[Integer.parseInt(lineArr[i])] = lineNum;
					
				}
				lineNum++;
			}
		}catch(IOException e){
			e.printStackTrace();
		}finally{
			try{
				if(fr!=null)
					fr.close();
			}catch(IOException e){
				e.printStackTrace();
			}
		}
	}
	
	private static void groupRegex(String regexFile){//对regex文件分组
		int maxValue = location[0];
		for(int j = 1;j<location.length;j++){
			if(location[j]>maxValue){
				maxValue = location[j];
			}
		}
		bw = new BufferedWriter[maxValue+1];
		BufferedReader bf = null;
		try{
			for(int i = 0;i<maxValue+1;i++){
				String fileName = "C:\\TestData1\\groupResult\\group"+i+".txt";
				bw[i] = new BufferedWriter(new FileWriter(fileName)); 
			}
			bf = new BufferedReader(new FileReader(regexFile));
			String line;
			int lineNum = 0;
			while((line=bf.readLine())!=null){
				if(line.charAt(0)=='#'){
					lineNum--;
				}
				bw[location[lineNum]].write(line);
				bw[location[lineNum]].newLine();
				lineNum++;
			}
		}catch(IOException e){
			e.printStackTrace();
		}finally{
			try{
				if(bf!=null)
					bf.close();
				for(int i = 0;i<maxValue+1;i++){
					if(bw[i]!=null){
						bw[i].close();
					}
				}
			}catch(IOException e){
				e.printStackTrace();
			}
		}
	}
}
