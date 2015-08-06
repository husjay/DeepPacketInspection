import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.LinkedList;


public class Divide {
	private  int K;//分组大小
	private  int N;//正则表达式个数
	private  int[][] M = null;//冲突矩阵
	private  float[] DR = null;//每个正则表达式对应dfa的冲突率
	private  int[] DRFileId = null;//对应DR数组，存放正则表达式标识
	private  LinkedList[] Group = null;//存放分组情况
	
	private  void divide(int k){//分组算法
		K= k;
		initMArr("C:\\TestData1\\m1file.txt");
		initDrArr("C:\\TestData1\\m2file.txt");
		drSort();
		initGroupArr();
		groupWithDr();
		optimizeGroupArr();
		OutPutGroupResult("C:\\TestData1\\groupResult\\groupResult.txt");
	}
	
	private void OutPutGroupResult(String fileName){//将分组结果写入文件中
		BufferedWriter bw = null;
		try{
			bw =new BufferedWriter( new FileWriter(fileName));
			bw.write(N+"");
			bw.newLine();
			for(int i = 0;i<K;i++){
				StringBuilder sb = new StringBuilder();
				for(int j = 0;j<Group[i].size();j++){
					sb.append(Group[i].get(j)+" ");
				}
				bw.write(sb.toString().trim());
				bw.newLine();
			}
		}catch(IOException e){
			e.printStackTrace();
		}finally{
			try{
				if(bw!=null)
					bw.close();
			}catch(IOException e){
				e.printStackTrace();
			}
		}
	}
	private void drSort(){//1.对dr数组进行升序排序
		drIncrQuickSort(DR,DRFileId,0,N-1);
	}
	private void drIncrQuickSort(float[] sortArr,int[] assArr,int p,int r){
		if(sortArr == null || p<0 || r>=sortArr.length)
			throw new RuntimeException("partition函数数组为空或者下标越界");
		int q = drQuickSortPartition(sortArr,assArr,p,r);
		if(p<q-1)
			drIncrQuickSort(sortArr,assArr,p,q-1);
		if(r>q+1)
			drIncrQuickSort(sortArr,assArr,q+1,r);
	}
	private int drQuickSortPartition(float[] sortArr,int[] assArr,int p,int r){
		if(sortArr == null || p<0 || r>=sortArr.length)
			throw new RuntimeException("partition函数数组为空或者下标越界");
		float standard = sortArr[r];//让最后一个元素为枢轴元素
		int i = p;//用来遍历
		int j = p;//指向的位置前都为比枢轴元素小的元素
		while(i<r)
		{
			if(sortArr[i]<standard)
			{
				CommonFunction.swap(sortArr, i, j);
				CommonFunction.swap(assArr, i, j);
				j++;
			}
			i++;
		}
		CommonFunction.swap(sortArr, i, j);
		CommonFunction.swap(assArr, i, j);
		return j;
	}
	
	private void initDrArr(String fileName){
		DR = new float[N];
		DRFileId = new int[N];
		BufferedReader fr=null;
		try{
			fr =new BufferedReader( new FileReader(fileName));
			String t = fr.readLine();
			for(int i = 0;i<N;i++){
				String[] drArr = t.split(" ");
				DR[i] = Float.parseFloat(drArr[i]);
				DRFileId[i] = i;
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
	private  void initMArr(String fileName){
		BufferedReader fr=null;
		try{
			fr =new BufferedReader( new FileReader(fileName));
			N = Integer.parseInt(fr.readLine());
			M = new int[N][N];
			for(int i = 0;i<N;i++){
				String t = fr.readLine();
				String[] tArr = t.split(" ");
				for(int j = 0;j<N;j++){
					M[i][j] = Integer.parseInt(tArr[j]);
				}
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
	
	private void initGroupArr(){
		Group = new LinkedList[K];
		for(int i =0;i<K;i++){
			Group[i] = new LinkedList();
		}
	}
	
	private void groupWithDr(){//按dr进行分组
		boolean reverse =false;
		int j = 0;
		for(int i = 0;i<N;i++){
			if (!reverse){
				Group[j++].add(DRFileId[i]);
				if(j==K-1){
					reverse=true;
				}
			}
			else{
				Group[j--].add(DRFileId[i]);
				if(j==0){
					reverse=false;
				}
			}
		}
	}
	
	private void optimizeGroupArr(){//开始优化分组数组
		boolean nochange = true;
		int[] f = new int[K];//某一正则表达式与各个组的冲突值
		while(true){
			int i = 0;
			nochange = true;
			for(i = 0;i<N;i++){
				for(int m = 0;m<K;m++){
					f[m] = 0;
					for(int j = 0;j<Group[m].size();j++){
						f[m]+=M[i][(Integer)Group[m].get(j)];
					}
					System.out.print(f[m]+" ");
				}
				int minF = 0;
				for(int j = 0;j<K;j++){
					if(f[j]<f[minF]){
						minF = j;
					}
				}
				System.out.println("\n"+minF);
				if(Group[minF].contains(i))
					continue;
				else{
					a: for(int t = 0;t<K;t++){
						if(Group[t].contains(i)){
							for(int k = 0;k<Group[t].size();k++){
								if(Group[t].get(k).equals(i)){
									Group[t].remove(k);
									break a;
								}
							}
						}
					}
					Group[minF].add(i);
					nochange = false;
					for(int x = 0;x<K;x++){
						System.out.print(Group[x]+"  ");
					}
					System.out.println();
				}
			}
			if(nochange==true&&i==N)
				break;
		}
	}
	public static void main(String argus[]){
		new Divide().divide(6); 
	}

}
class CommonFunction
{
	public static void swap(int[] arr,int i,int j)
	{
		int temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
	public static void swap(float[] arr,int i,int j)
	{
		float temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
	public static void printArr(int[] arr)
	{
		for(int i = 0;i<arr.length;i++)
		{
			System.out.print(arr[i]+"\t");
		}
	}

}