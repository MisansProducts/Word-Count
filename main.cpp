//Made by Alex

//======Libraries======
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

//======Structures======
struct Unigram {
	string word;
	int freq;
};

struct Bigram {
	string word1;
	string word2;
	int freq;
};

//======Function Prototypes======
void caseFoldString(string&);
bool readdata(string, vector<string>&, int&);
int cleanup(string&);

void generate_unigrams(vector<string>, vector<Unigram>&);
void sortgms(vector<Unigram>&);
void write1gmslog(vector<Unigram>);
int inList(vector<Unigram>, string);

void generate_bigrams(vector<string>, vector<Bigram>&);
void sortbgms(vector<Bigram>&);
int getlongestword(vector<Bigram>, int);
void write2gmslog(vector<Bigram>);
int inList(vector<Bigram>, string, string);

//======Main======
int main(int argc, char** argv) {
	// argc are the amount of arguments
	// argv is the list of arguments

	//Too few or too many arguments
	if (argc != 2) {
		cout << "Usage: ./output [FILENAME.txt]" << endl;
		return EXIT_FAILURE; //Terminates the program
	}

	//Variables
	vector<string> wordList; //Initial word list
	int wordCount = 0; //Amount of words in list
	char cleanupInput; //Input
	bool cleanupInputFlag = false; //Flag for error

	//Reads data from given file name (argv[1])
	bool fileExists = readdata(argv[1], wordList, wordCount);

	//File does not exist
	if (!fileExists) {
		cout << "File name does not exist." << endl;
		return EXIT_FAILURE; //Terminates the program
	}

	string userInput; //User Input

	//Log cleanup menu
	do {
		//Prompt
		cout << "Log cleanup? y/n: ";
		cin >> cleanupInput;

		cleanupInput = tolower(cleanupInput); //Converts to lowercase
		int nonAlphaFlag = 0; //Flag to keep track of changed non alpha words

		//Yes
		if (cleanupInput == 'y') {
			cout << "Writing to \"changelog.txt\"" << endl;
			ofstream fout("changelog.txt"); //Creates log file
			string oldWord; //Keeps old word
			fout << "Nonalphanumeric Replacement Log file:\n" << endl;

			//Cleans up the vector
			//Loops through vector
			for (int wordIndex = 0; wordIndex < wordCount; wordIndex++) {
				oldWord = wordList[wordIndex]; //Keeps old word
				caseFoldString(oldWord); //Lowercase old word
				nonAlphaFlag = cleanup(wordList[wordIndex]); //Gets flag result
				//Logs changes to old word if flag is active
				if (nonAlphaFlag == 1) {
					fout << left << setw(30) << oldWord << setw(2) << "->" << right << setw(30) << wordList[wordIndex] << endl;
				}
				//Alphanumeric to alpha
				if (nonAlphaFlag == 2) {
					fout << left << setw(30) << oldWord << setw(2) << "->" << right << setw(30) << wordList[wordIndex] << endl;
					oldWord = wordList[wordIndex]; //Keeps old word
					cleanup(wordList[wordIndex]); //Re-runs cleaner
					fout << left << setw(30) << oldWord << setw(2) << "->" << right << setw(30) << wordList[wordIndex] << endl;
				}
			}
			fout.close(); //Closes file
		}
		//No
		else if (cleanupInput == 'n') {
			//Cleans up the vector
			for (int wordIndex = 0; wordIndex < wordCount; wordIndex++) {
				nonAlphaFlag = cleanup(wordList[wordIndex]); //Gets flag result
				//Alphanumeric to alpha
				if (nonAlphaFlag == 2) {
					cleanup(wordList[wordIndex]); //Re-runs cleaner
				}
			}
		}
		//Error
		else {
			cout << "Input error. Please try again." << endl;
			cin.clear(); //Clears input flag
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cleanupInputFlag = true;
		}

	} while (cleanupInputFlag); //Loops on error

	vector<Unigram> unigrams; //Creates vector of structure
	vector<Bigram> bigrams; //Creates vector of structure

	generate_unigrams(wordList, unigrams); //Gets unigrams
	generate_bigrams(wordList, bigrams); //Gets bigrams

	int unigramsSize = unigrams.size();
	int bigramsSize = bigrams.size();

	//Longest word
	cout << "Longest word: \"" << bigrams[getlongestword(bigrams, bigramsSize)].word1 << "\"" << endl;

	//Menu
	do {
		cout << "======Commands======\n";
		cout << "Unigram - search for unigram\n";
		cout << "Bigram  - search for bigram\n";
		cout << "Save    - save to unigrams.txt and bigrams.txt\n";
		cout << "Index   - print index of unigram or bigram\n";
		cout << "Quit    - terminate program\n";
		cout << "Response: ";
		cin >> userInput;

		//Input error
		if (cin.fail()) {
			cin.clear(); //Clears input flag
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		//Converts user input to lower case
		caseFoldString(userInput);

		//Options
		//Unigrams
		if (userInput == "unigram") {
			int index = 0;
			cout << "Enter word to search: ";
			cin >> userInput;
			caseFoldString(userInput);
			cout << "Searching for " << userInput << "..." << endl;
			index = inList(unigrams, userInput);
			if (index == -1) {
				cout << "Unigram not found." << endl;
			}
			else {
				cout << "Unigram found at index: " << index << " with a frequency of " << unigrams[index].freq << "." << endl;
			}
		}
		//Bigrams
		else if (userInput == "bigram") {
			int index = 0;
			string word1, word2;
			cout << "Enter first word to search: ";
			cin >> word1;
			caseFoldString(word1);
			cout << "Enter second word to search: ";
			cin >> word2;
			caseFoldString(word2);
			cout << "Searching for " << word1 << " " << word2 << "..." << endl;
			index = inList(bigrams, word1, word2);
			if (index == -1) {
				cout << "Bigram not found." << endl;
			}
			else {
				cout << "Bigram found at index: " << index << " with a frequency of " << bigrams[index].freq << "." << endl;
			}
		}
		//Save
		else if (userInput == "save") {
			write1gmslog(unigrams);
			write2gmslog(bigrams);
		}
		//Index
		else if (userInput == "index") {
			int index = 0;
			cout << "Enter index: ";
			cin >> index; //Index
			//Input error
			if (cin.fail()) {
				cin.clear(); //Clears input flag
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				continue;
			}
			cout << "Unigram or bigram?: ";
			cin >> userInput;
			//Input error
			if (cin.fail()) {
				cin.clear(); //Clears input flag
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			caseFoldString(userInput);
			if (userInput == "unigram") {
				if (index >= 0 && index < unigramsSize) {
					//Search for it
					cout << "Index " << index << ": \'" << unigrams[index].word <<"\'" << " with a frequency of " << unigrams[index].freq << "." << endl;
				}
				//Index error
				else {
					cout << "Invalid index: " << index << "." << endl;
				}
			}
			else if (userInput == "bigram") {
				if (index >= 0 && index < bigramsSize) {
					//Search for it
					cout << "Index " << index << ": \'" << bigrams[index].word1 << " " << bigrams[index].word2 <<"\'" << " with a frequency of " << bigrams[index].freq << "." << endl;
				}
				//Index error
				else {
					cout << "Invalid index: " << index << "." << endl;
				}
			}
			//Option error
			else {
				cout << "Invalid option: " << userInput << endl;
			}
		}
		//Quit
		else if (userInput == "quit" || userInput == "q") {
			cout << "Goodbye!\n";
			break; //Gets out of the loop
		}
		//Invalid option
		else {
			cout << "Invalid entry. Try again." << endl;
		}

	} while (true); //Menu loop

	return EXIT_SUCCESS; //Exits the program
}

//======Function Definitions======
//Converts strings to lowercase
void caseFoldString(string& str) {
	for (unsigned int i = 0; i < str.size(); i++) {
		str[i] = tolower(str[i]);
	}
}

//Read Data Function
bool readdata(string fileName, vector<string>& wordList, int& wordCount) {
	//Local variables
	fstream myFile; //File
	string word; //Word in file

	myFile.open(fileName); //Opens file

	//Error opening file
	if (!myFile.is_open()) {
		return false; //Returns false
	}

	cout << "Opening file: \"" << fileName << "\"" << endl;

	//Transcribes file to vector
	while (myFile >> word) {
		wordList.push_back(word);
	}

	myFile.close(); //Closes file

	wordCount = wordList.size(); //Sets word count to vector's size

	cout << "Finished reading " << wordCount << " words.\n";
	
	return true; //Continues with the program
}

//Longest Word Function
int getlongestword(vector<Bigram> Bigrams, int wordCount) {
	//Local variables
	int longestWordIndex = 0;
	int length = 0; //Length of word
	int wordSize;

	//Loops through the word list
	for (int wordIndex = 0; wordIndex < wordCount; wordIndex++) {
		wordSize = Bigrams[wordIndex].word1.size();
		if (length <= wordSize) {
			length = wordSize; //Sets current longest length
			longestWordIndex = wordIndex; //Sets current longest word index
		}
	}

	return longestWordIndex; //Returns index of the longest word in the word list
}

//Cleanup Function
int cleanup(string& word) {
	//Local variables
	vector<char> nonAlphaChars; //Not allowed characters
	string alphaWord = "";
	int wordSize = word.size();
	int alphaWordSize;
	bool nonAlphaFlag = false;

	//Loops through string
	for (int charIndex = 0; charIndex < wordSize; charIndex++) {
		word[charIndex] = tolower(word[charIndex]); //Converts each character to lowercase
		if ((word[charIndex] >= '0' && word[charIndex] <= '9') ||
		(word[charIndex] >= 'a' && word[charIndex] <= 'z')) {
			alphaWord += word[charIndex];
		}
	}

	//Numbers are NONALPHA
	alphaWordSize = alphaWord.size();
	for (int charIndex = 0; charIndex < alphaWordSize; charIndex++) {
		if (alphaWord[charIndex] >= '0' && alphaWord[charIndex] <= '9') {
			nonAlphaFlag = true;
		}
	}

	//Changes non alphanumeric word to alphanumeric
	if (word != alphaWord) {
		word = alphaWord;
		//Checks for numbers
		for (int charIndex = 0; charIndex < alphaWordSize; charIndex++) {
			if (alphaWord[charIndex] >= '0' && alphaWord[charIndex] <= '9') {
				return 2; //Cleaned up non alphanumeric, reruns
			}
		}
		return 1; //Sets flag to active
	}
	//Changes alphanumeric word to NONALPHA
	else if (nonAlphaFlag) {
		word = "NONALPHA";
		return 1; //Sets flag to active
	}


	return 0; //Sets flag to inactive
}

//Index List Overloaded
//Searches for Unigrams
int inList(vector<Unigram> unigrams, string word) {
	int uniSize = unigrams.size();
	for (int i = 0; i < uniSize; i++) {
		if (unigrams[i].word == word) {
			return i;
		}
	}
	return -1;
}

//Searchs for Bigrams
int inList(vector<Bigram> bigrams, string word1, string word2) {
	int biSize = bigrams.size();
	for (int i = 0; i < biSize; i++) {
		if (bigrams[i].word1 == word1 && bigrams[i].word2 == word2) {
			return i;
		}
	}
	return -1;
}

//Generate Unigrams
void generate_unigrams(vector<string> wordList, vector<Unigram>& unigrams) {
	//Local variables
	bool findMatch = false; //Flag to find match
	int listSize = wordList.size();
	int uniSize;

	//Loops through word list
	for (int i = 0; i < listSize; i++) {
		findMatch = false; //Initializes flag
		//Loops through unigrams list
		uniSize = unigrams.size();
		for (int j = 0; j < uniSize; j++) {
			//Word match
			if (unigrams[j].word == wordList[i]) {
				unigrams[j].freq++;
				findMatch = true;
			}
		}
		//New word
		if (!findMatch) {
			unigrams.push_back(Unigram()); //New element
			//Edits structure
			unigrams.back().word = wordList[i];
			unigrams.back().freq = 1;
		}
	}

	sortgms(unigrams); //Sorts unigrams
}

//Generate Bigrams
void generate_bigrams(vector<string> wordList, vector<Bigram>& bigrams) {
	//Local variables
	bool findMatch = false; //Flag to find match
	int listSize = wordList.size();
	int biSize;

	//Loops through word list
	for (int i = 1; i < listSize; i++) {
		findMatch = false; //Initializes flag
		//Loops through bigrams list
		biSize = bigrams.size();
		for (int j = 0; j < biSize; j++) {
			//Words match
			if (bigrams[j].word1 == wordList[i-1] && bigrams[j].word2 == wordList[i]) {
				bigrams[j].freq++;
				findMatch = true;
			}
		}
		//New word
		if (!findMatch) {
			bigrams.push_back(Bigram()); //New element
			//Edits structure
			bigrams.back().word1 = wordList[i-1];
			bigrams.back().word2 = wordList[i];
			bigrams.back().freq = 1;
		}
	}

	sortbgms(bigrams); //Sorts bigrams
}

//Sort Unigrams
void sortgms(vector<Unigram>& unigrams) {
	//Local variables
	int j;
	struct Unigram key;
	int uniSize = unigrams.size();
	
	for (int i = 1; i < uniSize; i++) {
		key = unigrams[i];
		j = i - 1;
		
		//Insertion sort
		while (j >= 0 && unigrams[j].freq < key.freq) {
			unigrams[j + 1] = unigrams[j];
			j = j - 1;
		}
		unigrams[j + 1] = key;
	}
}

//Sort Bigrams
void sortbgms(vector<Bigram>& bigrams) {
	//Local variables
	int j;
	struct Bigram key;
	int biSize = bigrams.size();

	//First Word
	for (int i = 1; i < biSize; i++) {
		key = bigrams[i];
		j = i - 1;
		
		//Insertion sort
		while (j >= 0 && bigrams[j].word1 > key.word1) {
			bigrams[j + 1] = bigrams[j];
			j = j - 1;
		}
		bigrams[j + 1] = key;
	}

	//Second Word
	for (int i = 1; i < biSize; i++) {
		key = bigrams[i];
		j = i - 1;
		
		//Insertion sort
		while (j >= 0 && bigrams[j].word2 > key.word2 &&
		bigrams[j].word1 == key.word1) {
			bigrams[j + 1] = bigrams[j];
			j = j - 1;
		}
		bigrams[j + 1] = key;
	}
}

//Writes Unigrams
void write1gmslog(vector<Unigram> unigrams) {
	int uniSize = unigrams.size();
	cout << "Writing to \"unigrams.txt\"" << endl;

	ofstream fout("unigrams.txt"); //Creates file
	fout << "Number of unigrams: " << unigrams.size() << endl;

	for (int i = 0; i < uniSize; i++) {
		fout << left << setw(30) << unigrams[i].word << right << unigrams[i].freq << endl;
	}

	fout.close(); //Closes file

}

//Writes Bigrams
void write2gmslog(vector<Bigram> bigrams) {
	int biSize = bigrams.size();
	cout << "Writing to \"bigrams.txt\"" << endl;

	ofstream fout("bigrams.txt"); //Creates file
	fout << "Number of bigrams: " << bigrams.size() << endl;

	for (int i = 0; i < biSize; i++) {
		fout << left << setw(30) << bigrams[i].word1 << left << setw(30) << bigrams[i].word2 << right << bigrams[i].freq << endl;
	}

	fout.close(); //Closes file
}