#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <algorithm>
using namespace std;

#define DEBUG 0

extern int makeFileCompareResult(char*, char*);

bool checkAllCapitalWord(string word)
{
    if (word.size() == 1)
        return false;

    for (int i=0; i<word.size(); i++)
    {
        if (islower(word[i]))
            return false;
    }
    return true;
}

string preProcessingForSensitiveWord(string word)
{
    if (!checkAllCapitalWord(word))
	{
		for (int i=0; i<word.size(); i++)
			word[i] = tolower(word[i]);
	}
	return word;
}

set<string> compareAndFindDifferentWords(char* fp1, char* fp2)
{
	set<string> first;
	set<string> result;
	string temp;
	ifstream in;

	in.open(fp1);
	while (getline(in, temp))
	{	
		temp.erase(remove(temp.begin(), temp.end(), '\r'), temp.end());
		temp = preProcessingForSensitiveWord(temp);
		first.insert(temp);
	}
	in.close();

	set<string>::iterator iter;
	in.open(fp2);
	while (getline(in, temp))
	{
		temp.erase(remove(temp.begin(), temp.end(), '\r'), temp.end());
		temp = preProcessingForSensitiveWord(temp);
		iter = first.find(temp);
		if (iter != first.end())
		{
#if DEBUG
			cout << *iter << " is exist in first list" << endl;
#endif
		}
		else
		{
			result.insert(temp);
		}
	}
	in.close();

	return result;
}

//if you want to use this function in C file with gcc,
//then you need to append "C" keyword like below.
//extern "C" int makeFileCompareResult(char* firstfile, char* secondfile){
extern int makeFileCompareResult(char* firstfile, char* secondfile)
{
	set<string> result1, result2;
	set<string>::iterator iter;
	ofstream outFile("CompareResult.txt");

	result1 = compareAndFindDifferentWords(firstfile, secondfile);
	outFile << "****It's not in " << firstfile << " but in " << secondfile << "*****\n";
	for (iter = result1.begin(); iter!=result1.end(); ++iter)
	{
		outFile << *iter << endl;
	}

	result2 = compareAndFindDifferentWords(secondfile, firstfile);
	outFile << "****It's not in " << secondfile << " but in " << firstfile << "*****\n";
	for (iter = result2.begin(); iter!=result2.end(); ++iter)
	{
		outFile << *iter << endl;
	}

	outFile.close();
	cout << "Check the CompareResult.txt\n";
	return 0;
}
