#include <iostream>
#include <fstream>
#include <string>
#include<vector>

using namespace std;

bool H[8][16] = {			{1, 1, 1, 1, 0, 1, 0, 1,	1, 0, 0, 0, 0, 0, 0, 0},
							{1, 1, 0, 1, 1, 0, 1, 1,	0, 1, 0, 0, 0, 0, 0, 0},
							{1, 0, 1, 0, 0, 1, 0, 0,	0, 0, 1, 0, 0, 0, 0, 0},
							{1, 0, 1, 1, 1, 0, 1, 1,	0, 0, 0, 1, 0, 0, 0, 0},
							{0, 1, 0, 1, 0, 1, 0, 1,	0, 0, 0, 0, 1, 0, 0, 0},
							{0, 1, 1, 0, 1, 0, 1, 0,	0, 0, 0, 0, 0, 1, 0, 0},
							{0, 0, 1, 1, 0, 1, 1, 1,	0, 0, 0, 0, 0, 0, 1, 0},
							{0, 0, 1, 1, 1, 1, 0, 1,	0, 0, 0, 0, 0, 0, 0, 1}, };

// funkcja konwertuje wektor charów na wektor booleanow
vector<bool> convertCharToBool(vector<unsigned char> content) {
	vector<bool> binarycontent;

	for (int i = 0; i < content.size()-1; i++) {
		int oneChar = content[i];
		for (int n = 0; n < 8; n++) {
			binarycontent.push_back(oneChar % 2);
			oneChar = oneChar / 2;
		}
	}
	return binarycontent;
}

//funkcja ktora zmienia char 0 lub 1 na bool
vector<bool> changeCharAndBool(vector<unsigned char> content) {
	vector<bool> boolText;

	for (int i = 0; i < content.size()-1; i++) {
		if (content[i] == '1') {
			boolText.push_back(1);
		}
		else {
			boolText.push_back(0);
		}
	}
	return boolText;
}

//funkcja obliczajaca bity parzystosci albo obliczajaca iloczyn HE
vector<bool> addParityBits(vector<bool> message, bool calculateHE=false) {
	vector<bool> parityBits;
	int k = 1;
	if (calculateHE == true) {
		k = 2;
	}
	for (int i = 0; i < 8; i++) {
		int c = 0;
		for (int j = 0; j < k*8; j++) {
			c += H[i][j]*message[j];
		}
		c=c%2;
		parityBits.push_back(c);
	}
	return parityBits;
}

//funkcja ktora zwraca wiadomosc wraz z dodanymi bitami parzystosci
vector<bool> encode(vector<bool> message) {
	vector<bool> content;
	vector<bool> parityBits;
	vector<bool> finalContent;

	for (int i = 0; i < message.size(); i++) {
		content.push_back(message[i]);
		finalContent.push_back(message[i]);
		if ((i + 1) % 8 == 0) { 
			parityBits=addParityBits(content);
			for (int j = 0; j < 8; j++) {
				finalContent.push_back(parityBits[j]);
			}
			content.clear();
			parityBits.clear();
		}
	}
	return finalContent;
}

// funkcja porownujaca vektor bledu z kolumnami macierzy H - przypadek 1 bitu blednego
void correctionv1(vector<bool> E, vector<bool>& message) {
	bool correction=false;
	
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++) {
			if (E[j]==H[j][i]) {
				correction = true;
			}
			else {
				correction = false;
				break;
			}
		}
		if (correction == true) {
			message[i] = (~message[i])%2;
		}
	}
}

// funkcja porownujaca vektor bledu z kolumnami macierzy H - przypadek 2 bitow blednych
void correctionv2(vector<bool>E, vector<bool>& message) {
	bool correction = false;

	for (int i = 0; i < 16; i++) {
		for (int j = i+1; j < 16; j++) {
			for (int k = 0; k < 8; k++) {
				if (E[k] == (H[k][i] + H[k][j]) % 2) {
					correction = true;
				}
				else {
					correction = false;
					break;
				}
			}
			if (correction == true) {
				message[i] = (~message[i]) % 2;
				message[j] = (~message[j]) % 2;
			}
		}
	}

}

// funkcja sprawdzajaca czy w wektorze bledu wystepuja 1
void check(vector<bool> &message, int choice) {
	vector<bool> E;
	bool requiredCorrection = false;

	E = addParityBits(message, true);
	for (int i = 0; i < E.size(); i++) {
		if (E[i] == 1) {
			requiredCorrection = true;
		}
	}
	if (requiredCorrection == true) {
		if (choice == 2) {
			correctionv1(E, message);
		}
		else {
			correctionv2(E, message);
		}
	}
}
//funkcja konwertujaca vector bool do chara
char convertBoolToChar(vector<bool> message) {
	char sign = 0;
	for (int i = 0; i < 8; i++) {
		sign += message[i] * pow(2, i);
	}
	return sign;
}
// funkcja ktora weryfikuje i w verified i verifiedText jest przechowywana skorygowana wiadomosc
void verification(vector<vector<bool>> &separatedMessage, int size, int choice, vector<bool> &verified, vector<unsigned char> &verifiedText) {

	for (int i = 0; i < size; i++) {
		check(separatedMessage[i], choice);
		verified.insert(verified.end(), separatedMessage[i].begin(), separatedMessage[i].end());
		verifiedText.push_back(convertBoolToChar(separatedMessage[i]));
	}
}

int main()
{
	vector<bool> binarymessage;
	vector<unsigned char> message;
	vector<bool> encodedmessage;
	int choice;
	int size=0;
	int mem;
	do{
	cout << "1. Encode file" << endl;
	cout << "2. Verify file - 1 mistake in 16 bits" << endl;
	cout << "3. Verify file - 2 mistakes in 16 bits" << endl;
	cout << "4. Modify content of received file " << endl;
	cout << "5. End program" << endl;
	cout << "Description" << endl << "Before choosing option 2 and 3 choose option 1 to encode file"<<endl;
	cout << "Before choosing option 2 or 3 it is recommended to modify file encodedFile.txt" << endl;
	cout << "Choice: ";
	cin >> choice;

		if (choice == 1) {
			fstream encodedFile("encodedFile.txt");
			ifstream receivedFile("receivedFile.txt");
			if (!receivedFile) {
				cout << "That file doesn't exist";
				return 0;
			}
			// Odczyt z wiadomosci z pliku
			while (!receivedFile.eof()) {
				unsigned char b = receivedFile.get();
				message.push_back(b);
			}
			receivedFile.close();
			cout << "Message: ";
			for (int i = 0; i < message.size(); i++) {
				cout << message[i];
			}
			size = message.size() - 1;
			// Zapis wiadomosci w postaci binarnej 
			binarymessage = convertCharToBool(message);
			cout << endl << "Binary Message: ";
			for (int i = 0; i < binarymessage.size(); i++) {
				cout << binarymessage[i];
				if ((i + 1) % 8 == 0) {
					cout << " ";
				}
			}

			//Zakodowanie - czyli dodanie bitow parzystosci
			 encodedmessage = encode(binarymessage);
			cout << endl << "Encoded Message: ";
			for (int i = 0; i < encodedmessage.size(); i++) {
				cout << encodedmessage[i];
				encodedFile << encodedmessage[i];
				if ((i + 1) % 16 == 0) {
					cout << " ";
				}
			}
			encodedFile.close();
		}
		else if (choice == 2 || choice ==3) {
			vector<unsigned char> changedmessage;
			fstream encodedFile2("encodedFile.txt");

			while (!encodedFile2.eof()) {
				unsigned char b = encodedFile2.get();
				changedmessage.push_back(b);
			}
			vector<bool> changedMessage = changeCharAndBool(changedmessage);
			cout << "Changed Message: ";
			for (int i = 0; i < changedMessage.size(); i++) {
				cout << changedMessage[i];
				if ((i + 1) % 16 == 0) {
					cout << " ";
				}
			}

			vector<vector<bool>> separatedMessage;
			for (int i = 0; i < size; i++) {
				vector<bool> row;
				separatedMessage.push_back(row);
			}

			int j = 0;
			for (int i = 0; i < changedMessage.size(); i++) {
				separatedMessage[j].push_back(changedMessage[i]);
				if ((i + 1) % 16 == 0) {
					j++;
				}
			}
			vector<bool> verified;
			vector<unsigned char> verifiedText;
			verification(separatedMessage, size, choice, verified, verifiedText);
			cout << endl<<"Verified message: ";
			for (int i = 0; i < verified.size(); i++) {
				cout << verified[i];
				if ((i + 1) % 16 == 0) {
					cout << " ";
				}
			}
			fstream restoredFile("restoredFile.txt");
			cout << endl << "Restored message: ";
			for (int i = 0; i < size; i++) {
				cout << verifiedText[i];
				restoredFile << verifiedText[i];
			}
			restoredFile.close();
			encodedFile2.close();
			cout <<endl<<"Click any key to continue ";
			getchar();
			getchar();
			system("cls");
		}

		if (choice == 4) {
			ofstream encodedFile("encodedFile.txt");
			ofstream restoredFile("restoredFile.txt");
			ofstream receivedFile("receivedFile.txt");
			cout << endl<<"Enter new content of reveivedFile: ";
			receivedFile.clear();
			string text;
			getchar();
			getline(cin, text);
			receivedFile << text;
			receivedFile.close();
			encodedFile.close();
			restoredFile.clear();
			cout <<"Remember to choose 1 after modification in order to encode it"<<endl;
		}
		cout << endl;
		message.clear();
		//encodedmessage.clear();
	} while (choice != 5);
}




