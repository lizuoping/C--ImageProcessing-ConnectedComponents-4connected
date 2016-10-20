#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int **zeroFramedAry;
int *EQAry;
string *Property;
int neighborAry[4];
int numRows, numCols, minVal, maxVal, newMin, newMax=0, newLabel=0, numLabelUsed=0;

void initial(string fileName) {
	string line,str;
	int length, row = 1, col = 1;
	string::size_type pos;
	ifstream myfile(fileName);//open input file
	if (myfile.is_open()) {
		getline(myfile,line);//read the header 
		length = 0;
		pos = line.find(" ", length);
		numRows = stoi(line.substr(length, pos - length));
		length = pos + 1;
		pos = line.find(" ", length);
		numCols = stoi(line.substr(length, pos - length));
		zeroFramedAry = new int*[numRows+2];//dynamically allocate 
		for (int i = 0; i < numRows+2; i++)
			zeroFramedAry[i] = new int[numCols+2];
		for(int i=0; i<numRows+2; i++) {
			zeroFramedAry[i][0] = 0;
			zeroFramedAry[i][numCols+1] = 0;
		}
		for(int j=0; j<numCols+2; j++) {
			zeroFramedAry[0][j] = 0;
			zeroFramedAry[numRows+1][j] = 0;
		}
		while(myfile>>str) {//read the rest data
			zeroFramedAry[row][col++] = stoi(str);
			if(col>numCols) {
				row++;
				col=1;
			}
		}
		EQAry = new int[numRows*numCols/4];
		for(int i=0; i<numRows*numCols/4; i++)
			EQAry[i]=i;
		myfile.close();//close file
	} else {
		cout << "cannot open " << fileName << endl;
		exit(1);
	}
}

void PrettyPrint(ofstream& myfile) {
	for (int i = 1; i < numRows+1; i++) {
		for (int j = 1; j < numCols+1; j++) {
			if (zeroFramedAry[i][j] >0)//pixel_val > 0
				myfile << zeroFramedAry[i][j]<<" ";//output pixel_val
			else
				myfile << "  ";//output ' ' // blank
		}
		myfile << endl;
	}
}

void loadNeighbors(int row, int col) {
	neighborAry[0]=zeroFramedAry[row-1][col];
	neighborAry[1]=zeroFramedAry[row][col-1];
	neighborAry[2]=zeroFramedAry[row][col+1];
	neighborAry[3]=zeroFramedAry[row+1][col];
}

void updateEQAry(int index, int val) {
	EQAry[index] = val;
}

void printEQAry(ofstream& myfile) {
	for(int i=1; i<=newLabel; i++)
		myfile<<i<<" ";
	myfile<<endl;
	for(int i=1; i<=newLabel; i++)
		if(i>9 && EQAry[i]<10)
			myfile<<" "<<EQAry[i]<<" ";
		else
			myfile<<EQAry[i]<<" ";
	myfile<<endl;
}

int findMinNotZero(int row, int col, int pass) {
	if(pass==1) {
		if(neighborAry[0]<neighborAry[1])
			return (neighborAry[0]==0? neighborAry[1]:neighborAry[0]);
		else
			return (neighborAry[1]==0? neighborAry[0]:neighborAry[1]);
	}
	if(pass==2) {
		if(neighborAry[2]==0) {
			if(zeroFramedAry[row][col]<neighborAry[3])
				return zeroFramedAry[row][col];
			else
				return (neighborAry[3]==0? zeroFramedAry[row][col]:neighborAry[3]);
		}
		if(neighborAry[3]==0) {
			if(zeroFramedAry[row][col]<neighborAry[2])
				return zeroFramedAry[row][col];
			else
				return (neighborAry[2]==0? zeroFramedAry[row][col]:neighborAry[2]);
		}
		int temp=zeroFramedAry[row][col];
		temp=(neighborAry[2]<temp? neighborAry[2]:temp);
		temp=(neighborAry[3]<temp? neighborAry[3]:temp);
		return temp;
	}
}

void ConnectCC_Pass1() {
	for(int i=1; i<numRows+1; i++) {
		for(int j=1; j<numCols+1; j++) {
			loadNeighbors(i,j);
			if(zeroFramedAry[i][j]>0) {
				if(neighborAry[0]==0 && neighborAry[1]==0)
					zeroFramedAry[i][j]=++newLabel;
				else
					zeroFramedAry[i][j]=findMinNotZero(i,j,1);
			}
		}
	}
}

void ConnectCC_Pass2() {
	for(int i=numRows; i>0; i--) {
		for(int j=numCols; j>0; j--) {
			loadNeighbors(i,j);
			if(zeroFramedAry[i][j]>0 && (neighborAry[2]!=0 || neighborAry[3]!=0))
				if(zeroFramedAry[i][j]!=neighborAry[2] || zeroFramedAry[i][j]!=neighborAry[3]) {
					int minLabel = findMinNotZero(i,j,2);
					updateEQAry(zeroFramedAry[i][j], minLabel);
					zeroFramedAry[i][j] = minLabel;
				}
		}
	}
}

void manageEQAry() {
	for(int i=1; i<=newLabel; i++) 
		if(EQAry[i]==i)
			EQAry[i]=++numLabelUsed;
		else
			EQAry[i]=EQAry[EQAry[i]];
}

void ConnectCC_Pass3() {
	for(int i=1; i<numRows+1; i++) {
		for(int j=1; j<numCols+1; j++) {
			if(zeroFramedAry[i][j]>0)
				zeroFramedAry[i][j]=EQAry[zeroFramedAry[i][j]];
		}
	}
}

void computeProperty() {
	Property = new string[numLabelUsed+1];
	for(int m=1; m<=numLabelUsed; m++) {
		Property[m]="Label: "+to_string(m)+".    ";
		int sumPixel = 0;
		for (int i = 1; i < numRows+1; i++)
			for (int j = 1; j < numCols+1; j++)
				if (zeroFramedAry[i][j]==m)
					sumPixel++;
		Property[m]+="#pixels: "+to_string(sumPixel)+".    ";
		Property[m]+="The bounding box:";
		int minR=numRows, minC=numCols, maxR=0, maxC=0;
		for (int i = 1; i < numRows+1; i++)
			for (int j = 1; j < numCols+1; j++)
				if (zeroFramedAry[i][j]==m) {
					minR=(i<minR? i:minR);
					minC=(j<minC? j:minC);
					maxR=(i>maxR? i:maxR);
					maxC=(j>maxC? j:maxC);
				}
		Property[m]+="("+to_string(minR)+", "+to_string(minC)+", "+to_string(maxR)+", "+to_string(maxC)+")"+".  ";
	}
}

void connectedCC4(char **argv) {
	initial(argv[1]);
	ofstream myfile(argv[2]);//open output file
	if (myfile.is_open()) {
		ConnectCC_Pass1();
		myfile<<"The result of Pass1:"<<endl;
		PrettyPrint(myfile);
		myfile<<"The EQ Array of Pass1:"<<endl;
		printEQAry(myfile);
		myfile<<endl<<"--------------------------------------------------------------------"<<endl;
		ConnectCC_Pass2();
		myfile<<"The result of Pass2:"<<endl;
		PrettyPrint(myfile);
		myfile<<"The EQ Array of Pass2:"<<endl;
		printEQAry(myfile);
		myfile<<endl<<"--------------------------------------------------------------------"<<endl;
		manageEQAry();
		myfile<<"The EQ Array of manageEQAry:"<<endl;
		printEQAry(myfile);
		myfile<<endl<<"--------------------------------------------------------------------"<<endl;
		ConnectCC_Pass3();
		myfile<<"The result of Pass3:"<<endl;
		PrettyPrint(myfile);
		myfile<<"The EQ Array of Pass3:"<<endl;
		printEQAry(myfile);
		myfile.close();//close file
	} else {
		cout << "cannot write " << argv[2] << endl;
		exit(1);
	}
	myfile.open(argv[3]);//open output file
	if (myfile.is_open()) {
		if (numLabelUsed>0) newMin = 1;
		else newMin=0;
		newMax = numLabelUsed;
		myfile<<numRows<<" "<<numCols<<" "<<newMin<<" "<<newMax<<endl;
		PrettyPrint(myfile);
		myfile.close();//close file
	} else {
		cout << "cannot write " << argv[2] << endl;
		exit(1);
	}
	myfile.open(argv[4]);//open output file
	if (myfile.is_open()) {
		myfile<<"The connected component property:"<<endl;
		computeProperty();
		for(int i=1; i<=numLabelUsed; i++)
			myfile<<Property[i]<<endl;
		myfile.close();//close file
	} else {
		cout << "cannot write " << argv[2] << endl;
		exit(1);
	}
}

int main(int argc, char** argv) {
	connectedCC4(argv);
	cout << "All work done!" << endl;
	return 0;
}
