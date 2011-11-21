#include "helpers.h"

void printHelp() {

	cout << "Usage:" << endl;
}

int writeBES2File(const char* filename, bes a) {

	int i, j, k;
	int strLen;

	char *besFile;
	ofstream besfile;

	strLen = strlen(filename);
	besFile = (char*) malloc(sizeof(char) * (strLen + 12));
	strcpy(besFile, filename);
	strcat(besFile, ".PaRBES.bes");

	printf("Writing equation system to file %-40.35s", besFile);
	besfile.open(besFile);
	if( ! besfile.is_open() ) printf("[Error: could not open %s!]\n", besfile);

    for (i=0; i< a.blockCount; i++) {
        
        // print block information
        besfile << "block ";
        if (a.blocks[i].sign) besfile << "nu ";
        else besfile << "mu ";
        besfile << "B" << a.blocks[i].blockidentifier << " ";
        if (a.blocks[i].unique) besfile << "unique ";
        besfile << "mode " << a.blocks[i].mode << " is" << endl;
        //besfile << "\t (" << a.blocks[i].eqnCount << " equations)" << endl;
        
        // print equations
        for(j = 0; j < a.blocks[i].eqnCount; j++) {
            besfile << "   X" << a.blocks[i].eqns[j].lhsId << " =";
            for(k = 0; k < a.blocks[i].eqns[j].varCount; k++)
            {
                switch (a.blocks[i].eqns[j].rhs[k].type)
                {
                    case T: besfile << " true";
                        break;
                        
                    case F: besfile << " false";
                        break;
                        
                    case local: besfile << " X" << a.blocks[i].eqns[j].rhs[k].localRef;
                        break;
                        
                    case global: besfile << " X" << a.blocks[i].eqns[j].rhs[k].localRef << '_' << a.blocks[i].eqns[j].rhs[k].globalRef;
                        break;
                        
                    case conjunct: besfile << " and";
                        break;
                        
                    case disjunct: besfile << " or";
                        break;
                }
            }
            besfile << "\n";
        }
        besfile << "end block" << "\n" << "\n";
    }

	besfile.close(); // end write data to file
	printf("[done]\n");

	return 0;
}

int writeSolution2File(const char* filename, bes a) {
	
	int i, j;
	int strLen;
	int totalNumVars;

	char *solFile;
	ofstream solfile;

	totalNumVars = 0;

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 5));
	strcpy(solFile, filename);
	strcat(solFile, ".sol");

	printf("Writing solution file for %-40.35s \t", filename);
	solfile.open(solFile);
	if (!solfile.is_open()) printf("[Error: could not open %s!]\n", solfile);


	for (i = 0; i < a.blockCount; i++) {
	
		totalNumVars += a.blocks[i].eqnCount;
	}

	solfile << totalNumVars << endl;
	

	for (i = 0; i < a.blockCount; i++) {

		for (j = 0; j < a.blocks[i].eqnCount; j++) {

			switch (a.blocks[i].eqns[a.blocks[i].refs[j]].lhs) {
				case 0:
					solfile << "F" << endl; //<< i << '_' << a.blocks[i].eqns[a.blocks[i].refs[j]].lhsId << endl;
					break;
				case 1:
					solfile << "T" << endl; //<< i << '_' << a.blocks[i].eqns[a.blocks[i].refs[j]].lhsId << endl;
					break;
				default:
					solfile << "Error at block" << i << " equation "<< j << endl;
					break;
			}
		}		
	}

	solfile.close(); // end write data to file
	printf("[done]\n");

	return 0;
}

int writeEval2File(const char* filename, double** data, int rows) {

	int i, j;
	int maxLen;
	int strLen;
//	int orig, rev, origPar, revPar, asc, dsc;
	ofstream datfile, plotfile;
	char *datFile, *plotFile;
	int* ordering;

	ordering = (int*) malloc(sizeof(int) * rows);
	maxLen = 0;
	//orig = 0;
	//origPar = 1;
	//rev = 2;
	//revPar = 3;
	//asc = 4;
	//dsc = 5;

	//detemine the maximum length of the fields
	for(i = 0; i < rows; i++) {
		if( (int) data[i][0] > maxLen) maxLen = (int) data[i][0];
		//printf("Data[%d][0]: %4.1f\n", i, data[i][0]);
	}

	//// write data to file
	printf("Writing evaluation data for %-40.35s \t", filename);
	strLen = strlen(filename);
	datFile = (char*) malloc(sizeof(char) * (strLen + 5));
	strcpy(datFile, filename);
	strcat(datFile, ".dat");

	datfile.open(datFile);
	if( ! datfile.is_open() ) printf("[Error: could not open %s!]\n", datfile);

	for(i = 2; i < maxLen + 2; i++) {
		
		//datfile << i << "\t\t";

		for(j = 0; j < rows; j++) {

			if(i > data[j][0]) datfile << "0.00000\t\t";
			else datfile << fixed << setprecision(5) << data[j][i] << "\t\t";
		}
	
		datfile << endl;	
	}
	datfile.close(); // end write data to file
	printf("[done]\n");


	// write plot information to file
	printf("Writing plot information for %-30.20s \t\t", filename);
	strLen = strlen(filename);
	plotFile = (char*) malloc(sizeof(char) * (strLen + 6));
	strcpy(plotFile, filename);
	strcat(plotFile, ".plot");

	plotfile.open(plotFile);
	if( ! plotfile.is_open() ) printf("[Error: could not open %s!]\n", plotfile);
	plotfile << "clear" << endl;
	plotfile << "set term pdf monochrome enhanced dashed" << endl;
	plotfile << "set output \"" << filename << ".pdf\"" << endl;
	//plotfile << "set yrange[0:1]" << endl;
	if (maxLen > 1) plotfile << "set xrange[0:" << maxLen << "]" << endl;

	plotfile << "set style line 100 lt 1 lc rgb \"red\" lw 3" << endl;
	plotfile << "set style line 101 lt 1 lc rgb \"blue\" lw 2" << endl;
	plotfile << "set style line 102 lt 1 lc rgb \"green\" lw 2" << endl;
	plotfile << "set style line 103 lt 1 lc rgb \"yellow\" lw 2" << endl;
	plotfile << "set style line 104 lt 1 lc rgb \"black\" lw 2" << endl;


	plotfile << "plot";
	for(i = 0; i < rows; i++) {

		switch(i) {
		case 0:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Original: "		<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << 100 << " with lines";
			break;

		case 1:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Parallel: "		<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << 101 << " with lines";
			break;

		case 2:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Reverse: "		<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << 102 << " with lines";
			break;

		case 3:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Random: "		<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << 103 << " with lines";
			break;

		case 4:	
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Ordered(Dsc): " << fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << 104 << " with lines";
			break;

		case 5:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Parallel: "		<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << i << " with lines";
			break;

		case 6:	
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Descending RHS: " << fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << 103 << " with lines";
			break;

		case 7:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \"Parallel: "		<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << i << " with lines";
			break;

		default:
			plotfile << "\t\t\"" << datFile << "\" using " << i+1 << " ti \""				<< fixed << setprecision(0) << data[i][0] << "/" << data[i][1] << "\" ls " << i << " with lines";
			break;
		}

		if(i < rows-1) plotfile << ", \\" << endl;
	}

	plotfile.close(); // end write plot information to file
	printf("[done]\n");

	return 0;
}