import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

public class DFA {
	int N;//DFA状态数
	int M;//DFA字母表个数
	int startState;//起始状态，状态按照0,1,...N-1命名
	ArrayList<Integer> endState = new ArrayList<Integer>();//结束状态
	char[] alphaArray;//字母表中字母
	private int[] depth;//对应N个状态的深度
	int[][] transactionInfo;//转移边信息	
	
	public DFA(String dfaInputUrl){
		initDfa(dfaInputUrl);
		calculateDepth();
	}
	/*
	public static void main(String[] args){
		DFA dfa = new DFA("D:\\TestData\\dfa.txt");
		for(int i = 0;i<dfa.depth.length;i++){
			System.out.println(dfa.depth[i]);
		}
	}*/
	private void initDfa(String dfaInputUrl){
		BufferedReader fr=null;
		try{
			fr =new BufferedReader( new FileReader(dfaInputUrl));
			N = Integer.parseInt(fr.readLine());
			startState = Integer.parseInt(fr.readLine());
			String[] endStateArray =fr.readLine().split(" "); 
			for(int i = 0;i<endStateArray.length;i++){
				endState.add(Integer.parseInt(endStateArray[i]));
			}
			String[] alphaStringArray =fr.readLine().split(" ");
			M = Integer.parseInt(alphaStringArray[0]);
			alphaArray = new char[M];
			for(int i = 1;i<alphaStringArray.length;i++){
				alphaArray[i-1] = alphaStringArray[i].charAt(0);
			}
			transactionInfo = new int[N][M];
			for(int i = 0;i<N;i++){
				String[] t = fr.readLine().split(" ");
				for(int j = 0;j<M;j++){
					transactionInfo[i][j] = Integer.parseInt(t[j]);
				}
			}
			depth = new int[N];
			for(int i = 0;i<N;i++){
				depth[i] = -1;
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
	
	private void calculateDepth(){//计算状态深度
		int markNum = 0;//已经计算出深度的状态数
		int currentDepth = 0;//当前深度
		Queue<Integer> queue = new LinkedList<Integer>();//通过队列来计算
		queue.offer(startState);
		queue.offer(-1);//使用-1来隔离深度不同的状态
		while(!(queue.isEmpty())){
			int currentState = queue.poll();
			if(currentState==-1){
				queue.offer(-1);
				currentDepth++;
				continue;
			}
			if(depth[currentState]==-1){
				depth[currentState]=currentDepth;
				markNum++;
				for(int i=0;i<M;i++){
					queue.offer(transactionInfo[currentState][i]);
				}
			}
			if(markNum==N)
				break;
		}
	}

	public int getN() {
		return N;
	}

	public int getM() {
		return M;
	}

	public int[] getDepth() {
		return depth;
	}

	public int[][] getTransactionInfo() {
		return transactionInfo;
	}
	public int getStartState() {
		return startState;
	}
	public ArrayList<Integer> getEndState() {
		return endState;
	}
	public char[] getAlphaArray() {
		return alphaArray;
	}	
}
