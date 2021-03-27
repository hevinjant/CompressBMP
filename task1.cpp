// name: Hevin Jantasmin
// assignment: Midterm CPE 357
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
using namespace std;

#pragma pack(push,1)
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} tagBITMAPFILEHEADER;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biC1rUsed;
    DWORD biC1rImportant;
} tagBITMAPINFOHEADER;
#pragma pack(pop)

typedef struct Headers {
    tagBITMAPFILEHEADER* fileHeader;
    tagBITMAPINFOHEADER* infoHeader;
    unsigned char* bitmapData;
} Headers;

class Stack {
    private:
    char* array;
    int size;

    public:
    Stack() {
        array = new char[100];
        size = 0;
    }

    void push(char code) {
        array[size] = code;
        size++;
    }

    char pop() {
        if (size > 0) {
            char code = array[size-1];
            size--;
            return code;
        }
        return ' ';
    }

    string getCode() {
        string code = "";
        for (int i = 0; i < size; i++) {
            code += array[i];
        }
        return code;
    }

    int getSize() {
        return size;
    }

    ~Stack() {
        //delete[] array;
    }
};

typedef struct Node {
    int value;
    int freq;
    struct Node* left;
    struct Node* right;
    string code;
} Node;

class LeafNodes {
    private:
    Node** array; // array of addresses of Node
    int size;

    public:
    LeafNodes(int initialSize) {
        array = new Node*[initialSize];
        size = 0;
    }

    void append(int value, int freq, Node* left, Node* right) {
        Node* newNode = new Node;
        newNode->value = value;
        newNode->freq = freq;
        newNode->right = right;
        newNode->left = left;
        newNode->code = "";

        array[size] = newNode;

        size++;

        sortNodes();
    }

    void appendNode(Node* node) {
        array[size] = node;

        size++;

        sortNodes();
    }

    Node* pop() {
        if (size > 0) {
            Node* smallestFreq = (Node*)array[size-1];
            size--;
            return smallestFreq;
        }
        return NULL;
    }

    Node* getNodeAt(int index) {
        if (size > 0 && index < size && index >= 0) {
            Node* node = (Node*)array[index];
            return node;
        }
        return NULL;
    }

    void sortNodes() { // sort the array based on their freqs, descending order (last one has the smallest freq)
        if (size > 0) {
            Node* temp;
            for (int i = 0; i < size-1; i++) {
                for (int j = 0; j < size-i-1; j++) {
                    if (array[j]->freq < array[j+1]->freq) {
                        temp = array[j+1];
                        array[j+1] = array[j];
                        array[j] = temp;
                    }
                    else if (array[j]->freq == array[j+1]->freq) {
                        if (array[j]->value < array[j+1]->value) {
                            temp = array[j+1];
                            array[j+1] = array[j];
                            array[j] = temp;
                        }
                    }
                }
            }
        }
    }

    void printArray() {
        cout << "Size: " << size << endl;
        for (int i = 0; i < size; i++) {
            Node* current = (Node*)array[i];
            cout << "Value: " << current->value << " Freq: " << current->freq;
            cout << " Code: " << current->code << endl;
        }
    }

    int getSize() {
        return size;
    }

    string getValueCode(int value) {
        for (int i = 0; i < size; i++) {
            if (value == array[i]->value) {
                return array[i]->code;
            }
        }
        return "";
    }

    ~LeafNodes() {
        delete[] array;
    }
};

unsigned char* loadBMPFile(char* filename, tagBITMAPINFOHEADER* bitmapInfoHeader, tagBITMAPFILEHEADER* bitmapFileHeader);
string loadHBMPFile(char* filename, tagBITMAPINFOHEADER* bitmapInfoHeader, tagBITMAPFILEHEADER* bitmapFileHeader);
void writeBMPFile(char* filename,tagBITMAPINFOHEADER* bitmapInfoHeader,tagBITMAPFILEHEADER* bitmapFileHeader,unsigned char* bitmapBits);
void writeHBMPFile(char* filename,Headers header,LeafNodes* leafNodesUNMOD,string passwordBits,string flag);
unsigned char getRed(int index,unsigned char* bitmapBits);
unsigned char getGreen(int index,unsigned char* bitmapBits);
unsigned char getBlue(int index,unsigned char* bitmapBits);
unsigned char* colorImage(unsigned char* bitmapBits, unsigned char* dest, tagBITMAPINFOHEADER* bitmapInfoHeader, string flag, int lowerBound, int upperBound);
int bmpFileIsValid(char* filename);
int hbmpFileIsValid(char* filename);
int flagIsValid(char* flag);
LeafNodes* createHuffmanCode(unsigned char* bitmapBits, int bitmapSize);
void traverseTree(Node* root, Stack stack); // traverse the Huffman tree to get the code for each value
void printTree(Node* root);
int toDecimal(long long n);
unsigned long toULLDecimal(unsigned long n);
string toBinary(int n, int numOfBits); // convert an integer into its binary and automatically adds padding
string getBytes(string &bits, int numOfBytes); // reads numOfBytes bytes of a string
LeafNodes* extractInfo(string& info, char* password, string* flag);
Node* createHuffmanTree(LeafNodes* leafNodes);
unsigned char* createBitmap(Node* root, string info, tagBITMAPINFOHEADER* bitmapInfoHeader, string flag);
string encryptPassword(char* password);
string decryptPassword(char* password);
string convertPassword(string password);
void trimExtension(char* string);

int main(int argc, char* argv[]) {
    printf("Program started:\n");

    // check if there are five arguments being passed
    if (argc != 4) {
        printf("ERROR: Please check your parameters again.\n");
        printf("Format: [programName] [flag] [filename] [password]\n");
        return 0;
    }

    unsigned char* bitmapData;
    unsigned char* mmapMemory;
    string flag;
    char fileName[100];
    char fileNameCP[100];
    char password[7];
    int isCompress = 0;

    // check if password is valid
    if (strlen(argv[3]) != 6) {
        printf("ERROR: Invalid password.\n");
        return 0;
    }
    else {
        strcpy(password,argv[3]);
    }

    // check if the bmp or hbmp filename is valid
    if (bmpFileIsValid(argv[2])) {
        strcpy(fileName,argv[2]);
        strcpy(fileNameCP,argv[2]);
        trimExtension(fileNameCP);
        strcat(fileNameCP,".hbmp");
        isCompress = 1;
    }
    else if (hbmpFileIsValid(argv[2])) {
        strcpy(fileName,argv[2]);
        strcpy(fileNameCP,argv[2]);
        trimExtension(fileNameCP);
        strcat(fileNameCP,".bmp");
        isCompress = 0;
    }
    else {
        printf("ERROR: Invalid bmp or hbmp file.\n");
        return 0;
    }

    // check if flag is valid
    if (flagIsValid(argv[1])) {
        flag = argv[1];
    }
    else {
        printf("ERROR: Invalid flag.\n");
        return 0;
    }

    tagBITMAPFILEHEADER bitmapFileHeader;
    tagBITMAPINFOHEADER bitmapInfoHeader;

    Headers header;
    header.fileHeader = &bitmapFileHeader;
    header.infoHeader = &bitmapInfoHeader;

    if (isCompress == 1) {
        printf("Compressing image..\n");

        string encryptedP = encryptPassword(password);
        string bitsP = convertPassword(encryptedP);

        clock_t start = clock();

        LeafNodes *leafNodesUNMOD;
        bitmapData = loadBMPFile(fileName,&bitmapInfoHeader,&bitmapFileHeader);
        header.bitmapData = bitmapData;

        mmapMemory = (unsigned char*)mmap(NULL, (header.infoHeader->biSizeImage), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        
        // With fork()
        if (fork() == 0) {
            colorImage(header.bitmapData,mmapMemory,header.infoHeader,flag,0,header.infoHeader->biHeight/2);
            return 0;
        }
        else {
            colorImage(header.bitmapData,mmapMemory,header.infoHeader,flag,header.infoHeader->biHeight/2,header.infoHeader->biHeight);
            wait(0);
        }
        header.bitmapData = mmapMemory;

        leafNodesUNMOD = createHuffmanCode(header.bitmapData,header.infoHeader->biSizeImage);
        
        writeHBMPFile(fileNameCP,header,leafNodesUNMOD,bitsP,flag);

        munmap(mmapMemory,header.infoHeader->biSizeImage);

        clock_t end = clock();

        printf("Done!\n");
        double duration = (double)(end-start)/CLOCKS_PER_SEC;
        printf("Program duration: %f\n",duration);
    }
    else {
        printf("Decompressing image..\n");

        clock_t start = clock();

        LeafNodes* leafNodes;
        string info = loadHBMPFile(fileName,&bitmapInfoHeader,&bitmapFileHeader);
        unsigned char* bitmapBits;

        leafNodes = extractInfo(info,password,&flag);

        Node* root = createHuffmanTree(leafNodes);

        bitmapBits = createBitmap(root,info,&bitmapInfoHeader,flag);

        writeBMPFile(fileNameCP,&bitmapInfoHeader,&bitmapFileHeader,bitmapBits);

        clock_t end = clock();

        printf("Done!\n");
        double duration = (double)(end-start)/CLOCKS_PER_SEC;
        printf("Program duration: %f\n",duration);
    }

    return 0;
}

unsigned char* loadBMPFile(char *filename, tagBITMAPINFOHEADER* bitmapInfoHeader, tagBITMAPFILEHEADER* bitmapFileHeader) {
    FILE *filePtr;
    unsigned char* bitmapData;
    int index = 0;
    unsigned char temp;

    // open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL) {
        printf("ERROR: BMP File doesn't exist.\n");
        exit(1);
        return NULL;
    }
    
    // read the bitmap file header
    fread(bitmapFileHeader,sizeof(tagBITMAPFILEHEADER),1,filePtr);
    if (bitmapFileHeader->bfType != 0x4D42) {
        fclose(filePtr);
    }

    // read the bitmap info header
    fread(bitmapInfoHeader, sizeof(tagBITMAPINFOHEADER),1,filePtr);

    // move file point to the bitmap data
    fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);

    // allocate memory for bitmap image data
    bitmapData = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    // verify memory allocation
    if (!bitmapData) {
        free(bitmapData);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapData,bitmapInfoHeader->biSizeImage,1,filePtr);
    if (bitmapData == NULL) {
        fclose(filePtr);
        return NULL;
    }

    fclose(filePtr);
    return bitmapData;
}

string loadHBMPFile(char *filename, tagBITMAPINFOHEADER* bitmapInfoHeader, tagBITMAPFILEHEADER* bitmapFileHeader) {
    FILE *filePtr;
    string info = "";

    // open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL) {
        printf("ERROR: HBMP File doesn't exist.\n");
        exit(1);
    }
    
    // read the bitmap file header
    fread(bitmapFileHeader,sizeof(tagBITMAPFILEHEADER),1,filePtr);
    if (bitmapFileHeader->bfType != 0x4D42) {
        fclose(filePtr);
    }

    // read the bitmap info header
    fread(bitmapInfoHeader, sizeof(tagBITMAPINFOHEADER),1,filePtr);

    // move file point to the Huffman info
    fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);

    unsigned char byte, r;
    unsigned char mask = 1;  
    do {
        if (fread(&byte,1,1,filePtr) != 1) {
            break;
        }
        for (int i = 0; i < 8; i++) {
            string bitS;
            r = byte & mask;
            if (r == 0) {
                info += "0";
            }
            else {
                info += "1";
            }
            byte = byte >> 1;    
        }
    } while(1);
    
    fclose(filePtr);

    //cout << info << endl;

    return info;
}

void writeBMPFile(char* filename,tagBITMAPINFOHEADER* bitmapInfoHeader,tagBITMAPFILEHEADER* bitmapFileHeader,unsigned char* bitmapBits) {
    FILE *filePtr;
    unsigned char temp;
    int index = 0;

    filePtr = fopen(filename,"wb");

    fwrite(bitmapFileHeader,sizeof(tagBITMAPFILEHEADER),1,filePtr);
    fwrite(bitmapInfoHeader,sizeof(tagBITMAPINFOHEADER),1,filePtr);
    fwrite(bitmapBits,bitmapInfoHeader->biSizeImage,1,filePtr);

    fclose(filePtr);
}

void writeHBMPFile(char* filename,Headers header,LeafNodes* leafNodesUNMOD,string passwordBits,string flag) {
    string info = "";
    // info of encrypted password bits
    info += passwordBits;

    // info whether it's in gray scale or in color
    info += toBinary((int)(flag.at(0)),8);

    // info of how many leaf nodes there are
    info += toBinary(leafNodesUNMOD->getSize(),16);

    // create dictionary for value-code pair in 2 bytes
    for (int i = 0; i < leafNodesUNMOD->getSize(); i++) {
        Node* node = leafNodesUNMOD->getNodeAt(i);
        info += toBinary(node->value,8); // get the value of node and convert it to binary
        //info += "-";
        info += toBinary(node->freq,24); // get the freq of node and convert it to binary 
        //info += "/";
    }
    
    if (flag == "g") {
        // store the huffman code of each pixel
        for (int index = 0; index < header.infoHeader->biSizeImage; index+=3) {
            // for each pixel, get the code of each color and append it to the string
            info += leafNodesUNMOD->getValueCode((int)header.bitmapData[index+0]);
        }
    }
    else {
        // store the huffman code of each pixel
        for (int index = 0; index < header.infoHeader->biSizeImage; index+=3) {
            // for each pixel, get the code of each color and append it to the string
            info += leafNodesUNMOD->getValueCode((int)header.bitmapData[index+0]);
            info += leafNodesUNMOD->getValueCode((int)header.bitmapData[index+1]);
            info += leafNodesUNMOD->getValueCode((int)header.bitmapData[index+2]);
        }
    }

    FILE *filePtr;

    filePtr = fopen(filename,"wb");

    fwrite(header.fileHeader,sizeof(tagBITMAPFILEHEADER),1,filePtr);
    fwrite(header.infoHeader,sizeof(tagBITMAPINFOHEADER),1,filePtr);

    int currentBit = 0;
    int bit;
    unsigned char bitBuffer;
    for (int i = 0; i < info.size(); i++) {
        bit = (int)(info.at(i) - '0');
        if (bit) {
            bitBuffer |= (1 << currentBit);
        }
        currentBit++;
        if (currentBit == 8) {
            fwrite(&bitBuffer,1,1,filePtr);
            currentBit = 0;
            bitBuffer = 0;
        }
        if (currentBit != 8 && i == (info.size()-1)) {
            int paddingNeeded = 7-currentBit;
            bitBuffer = bitBuffer << paddingNeeded;
            bitBuffer = bitBuffer >> paddingNeeded;
            fwrite(&bitBuffer,1,1,filePtr);
        }
    }

    fclose(filePtr);
}

unsigned char getRed(int index,unsigned char* bitmapBits) {
    return (unsigned char)bitmapBits[index+2];
}
unsigned char getGreen(int index,unsigned char* bitmapBits) {
    return (unsigned char)bitmapBits[index+1];
}
unsigned char getBlue(int index,unsigned char* bitmapBits) {
    return (unsigned char)bitmapBits[index+0];
}

unsigned char* colorImage(unsigned char* bitmapBits, unsigned char* dest, tagBITMAPINFOHEADER* bitmapInfoHeader, string flag, int lowerBound, int upperBound) {
    unsigned char red, green, blue;
    unsigned char gray;

    if (flag == "g") {
        // with nested loop
        int width = bitmapInfoHeader->biWidth;
        int index = 0;
        for (int y = lowerBound; y < upperBound; y++) {
            for (int x = 0; x < width; x++) {
                index = (width * y)*3 + y*(width % 4) + (x*3); // index of current pixel

                // get colors of the first bmp file
                blue = getBlue(index,bitmapBits);
                green = getGreen(index,bitmapBits);
                red = getRed(index,bitmapBits);

                gray = (unsigned char)( ((int)red + (int)green + (int)blue) / 3 );

                // assign colors to result
                dest[index+0] = gray;
                dest[index+1] = gray;
                dest[index+2] = gray;
            }
        }
    }
    else {
        int width = bitmapInfoHeader->biWidth;
        //int height = bitmapInfoHeader->biHeight;
        int index = 0;
        for (int y = lowerBound; y < upperBound; y++) {
            for (int x = 0; x < width; x++) {
                index = (width * y)*3 + y*(width % 4) + (x*3); // index of current pixel

                // get colors of the first bmp file
                blue = getBlue(index,bitmapBits);
                green = getGreen(index,bitmapBits);
                red = getRed(index,bitmapBits);

                // assign colors to result
                dest[index+0] = blue;
                dest[index+1] = green;
                dest[index+2] = red;
            }
        }
    }
    
    return dest;
}

int bmpFileIsValid(char* filename) {
    int size = strlen(filename);
    char ext[4];
    char expectedExt[5] = ".bmp";
    int isValid = 1;

    if (size < 5) {
        isValid = 0;
        return isValid;
    }

    ext[0] = filename[size-4];
    ext[1] = filename[size-3];
    ext[2] = filename[size-2];
    ext[3] = filename[size-1];

    for (int i = 0; i < 4; i++) {
       if (ext[i] != expectedExt[i]) {
          isValid = 0;
          break;
       }
    }

    return isValid;
}

int hbmpFileIsValid(char* filename) {
    int size = strlen(filename);
    char ext[5];
    char expectedExt[6] = ".hbmp";
    int isValid = 1;

    if (size < 6) {
        isValid = 0;
        return isValid;
    }

    ext[0] = filename[size-5];
    ext[1] = filename[size-4];
    ext[2] = filename[size-3];
    ext[3] = filename[size-2];
    ext[4] = filename[size-1];

    for (int i = 0; i < 4; i++) {
       if (ext[i] != expectedExt[i]) {
          isValid = 0;
          break;
       }
    }

    return isValid;
}

int flagIsValid(char* flag) {
    if (strlen(flag) != 1) {
        return 0;
    }
    if (strchr(flag,'g') != NULL || strchr(flag,'c') != NULL) {
        return 1;
    }

    return 0;
}

LeafNodes* createHuffmanCode(unsigned char* bitmapBits,int bitmapSize) {
    // get number of occurrence of each color
    int hist[256];
    for (int i = 0; i < 256; i++) {
        hist[i] = 0;
    }
    for (int i = 0; i < bitmapSize; i++) {
        hist[(int)bitmapBits[i]] += 1;
    }

    // get number of value that has occurrence > 0
    int totalLeafNodes = 0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] != 0) {
            totalLeafNodes += 1;
        }
    }

    LeafNodes leafNodes(totalLeafNodes); // Array that initially stores the leaf nodes, then the Huffman coding will be applied to this array,
    // at the end of Huffman coding, this array should only have one node, which is the root node of the Huffman tree. 
    LeafNodes* leafNodesUNMOD = new LeafNodes(totalLeafNodes); // Array that stores all the original leaf nodes, use this to get the Huffman code for each value
    // after it got assigned later.

    // fill leafNodes
    for (int i = 0; i < 256; i++) {
        if (hist[i] != 0) {
           leafNodes.append(i,hist[i],NULL,NULL);
        }
    }

    // Build the Huffman tree
    while(leafNodes.getSize() != 1) {
        Node* left = leafNodes.pop();
        Node* right = leafNodes.pop();
        
        // store only all the leaf nodes into leafNodesUNMOD
        if (left->left == NULL && left->right == NULL) {
            leafNodesUNMOD->appendNode(left);
        }
        if (right->left == NULL && right->right == NULL) {
            leafNodesUNMOD->appendNode(right);
        }

        leafNodes.append(0,left->freq + right->freq,left,right);
    }

    // Root node for the Huffman tree
    Node* rootNode = leafNodes.pop();

    // Traverse the Huffman tree and assign the code for each value, and store the code into node->code
    Stack s;
    traverseTree(rootNode,s);

    //leafNodesUNMOD->printArray();

    return leafNodesUNMOD;
 }

void traverseTree(Node* root, Stack stack) {
    // if it's a leaf node
    if (root == NULL) {
        return;
    }
    if (root->left == NULL && root->right == NULL) {
        string code = stack.getCode();
        root->code = code;

        return;
    }

    stack.push('0');
    traverseTree(root->left,stack);
    stack.pop();

    stack.push('1');
    traverseTree(root->right,stack);
    stack.pop();
}

void printTree(Node* root) {
    if (root->left == NULL && root->right == NULL) {
        cout << "*******\n";
        cout << " Value: " << root->value << endl;
        cout << " Code: " << root->code << endl;
        return;
    }

    printTree(root->left);
    printTree(root->right);
}

int toDecimal(long long n)
{
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

unsigned long toULLDecimal(unsigned long n)
{
    unsigned long long decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

string toBinary(int n, int numOfBits) {
    string r;
    int numBits = numOfBits;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    
    // if the binary is not numOfBits bits of length, add paddings
    if (r.size() < numBits) {
        int size = r.size();
        // reverse the string
        for (int i = 0; i < size / 2; i++) {
            swap(r[i], r[size - i - 1]);
        }
        // add paddings
        for (int i = 0; i < numBits-size; i++) {
            r += "0";
        }
        // reverse the string again
        int newSize = r.size();
        for (int i = 0; i < newSize / 2; i++) {
            swap(r[i], r[newSize - i - 1]);
        }
    }

    return r;
}

string getBytes(string &bits, int numOfBytes) {
    string byte = "";
    int numBytes = numOfBytes * 8;

    for (int i = 0; i < numBytes; i++) {
        byte += bits.at(i);
    }
    bits.erase(0,numBytes);

    return byte;
}

LeafNodes* extractInfo(string& info, char* enteredPassword, string* flag) {
    // get the encrypted password bits
    string password = "";
    for (int i = 0; i < 6; i++) {
        password += (char)toDecimal(stoi(getBytes(info,1)));
    }
    // decrypting password
    char toDecrypt[6];
    strcpy(toDecrypt,password.c_str());
    string decrypted = decryptPassword(toDecrypt);

    if (strcmp(decrypted.c_str(),enteredPassword) != 0) {
        printf("ERROR!: Incorrect password for this image.\n");
        exit(1);
    }

    // get the color information
    *flag = (char)toDecimal(stoi(getBytes(info,1))) ;

    // get the number of leaf nodes
    string sizeBits = getBytes(info,2);
    int totalLeafs = toDecimal(stol(sizeBits));
    
    LeafNodes* leafNodes = new LeafNodes(totalLeafs);

    for (int i = 0; i < totalLeafs; i++) {
        string v = getBytes(info,1);
        string f = getBytes(info,3);
        
        int value = toDecimal(stoi(v));
        unsigned long freq = toULLDecimal(stoull(f));
        
        leafNodes->append(value,freq,NULL,NULL);

    }

    //leafNodes->printArray();
    
    return leafNodes;
}

Node* createHuffmanTree(LeafNodes* leafNodes) {
    // Build the Huffman tree
    while(leafNodes->getSize() != 1) {
        Node* left = leafNodes->pop();
        Node* right = leafNodes->pop();

        leafNodes->append(0,left->freq + right->freq,left,right);
    }

    // Root node for the Huffman tree
    Node* rootNode = leafNodes->pop();
    
    return rootNode;
}

unsigned char* createBitmap(Node* root, string info, tagBITMAPINFOHEADER* bitmapInfoHeader,string flag) {
    int size = info.size();
    unsigned char* bitmapBits = new unsigned char[bitmapInfoHeader->biSizeImage];
    string bit;
    Node* currentNode = root;
    int bitmapIndex = 0;

    if (flag == "g") {
        for (int i = 0; i < size; i++) {
            bit = info.at(i);
            if (bit == "0") {
                currentNode = currentNode->left;
                // We found a leaf
                if (currentNode->left == NULL && currentNode->right == NULL) {
                    //cout << currentNode->value << endl;
                    bitmapBits[bitmapIndex+0] = (unsigned char)currentNode->value;
                    bitmapBits[bitmapIndex+1] = (unsigned char)currentNode->value;
                    bitmapBits[bitmapIndex+2] = (unsigned char)currentNode->value;
                    currentNode = root;
                    bitmapIndex += 3;
                }
            }
            else if (bit == "1") {
                currentNode = currentNode->right;
                // We found a leaf
                if (currentNode->left == NULL && currentNode->right == NULL) {
                    //cout << currentNode->value << endl;
                    bitmapBits[bitmapIndex+0] = (unsigned char)currentNode->value;
                    bitmapBits[bitmapIndex+1] = (unsigned char)currentNode->value;
                    bitmapBits[bitmapIndex+2] = (unsigned char)currentNode->value;
                    currentNode = root;
                    bitmapIndex += 3;
                }
            }
        }
    }
    else {
        for (int i = 0; i < size; i++) {
            bit = info.at(i);
            if (bit == "0") {
                currentNode = currentNode->left;
                // We found a leaf
                if (currentNode->left == NULL && currentNode->right == NULL) {
                    //cout << currentNode->value << endl;
                    bitmapBits[bitmapIndex+0] = (unsigned char)currentNode->value;
                    currentNode = root;
                    bitmapIndex ++;
                }
            }
            else if (bit == "1") {
                currentNode = currentNode->right;
                // We found a leaf
                if (currentNode->left == NULL && currentNode->right == NULL) {
                    //cout << currentNode->value << endl;
                    bitmapBits[bitmapIndex+0] = (unsigned char)currentNode->value;
                    currentNode = root;
                    bitmapIndex ++;
                }
            }
        }
    }

    return bitmapBits;
}

string encryptPassword(char* password) {
    string encrypted = "";
    for (int i = 0; i < 6; i++) {
        password[i] ^= 6-i;
        encrypted += password[i];
    }

    return encrypted;
}

string decryptPassword(char* password) {
    string decrypted = "";
    for (int i = 0; i < 6; i++) {
        password[i] ^= 6-i;
        decrypted += password[i];
    }

    return decrypted;
}

string convertPassword(string password) {
    string bits = "";
    for (int i = 0; i < 6; i++) {
        bits += toBinary((int)(password.at(i)),8);
    }
    return bits;
}

void trimExtension(char* string) {
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == '.') {
            string[i] = '\0';
            break;
        }
    }
}