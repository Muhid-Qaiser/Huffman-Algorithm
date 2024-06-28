#include <fstream>
#include <iostream>
#include "MyVector1.h"
using namespace std;

#define totalascii 256

class MyTree; // * declaring class prototype

struct Code // * structure of the code of each node
{
    char asci;
    MyVector<char> bits;
    Code() : asci(0), bits(){};
    Code(const char c, MyVector<char> b) : asci(c), bits(b){};
    const bool operator<(const Code &other) const { return this->asci < other.asci; }   // * operator overloading to facilitate operations in the vector library
    const bool operator==(const Code &other) const { return this->asci == other.asci; } // * operator overloading to facilitate operations in the vector library
};

class Node // * node class
{
    int freq;
    char val;
    Node *left;
    Node *right;
    MyVector<char> code; // * contains code of each node

public:
    Node(const Node &other) : code()
    {
        freq = other.freq;
        val = other.val;
        left = other.left;
        right = other.right;
    }
    Node(char v, MyVector<char> b) : val(v), code(b), left(nullptr), right(nullptr), freq(0) {} // * initializes a node with code b and value v
    Node(const int f = 0, const char v = 0, Node *l = nullptr, Node *r = nullptr)
        : freq(f), val(v), left(l), right(r), code() {} // * creates new node
    Node(Node l, Node r) : left(new Node(l)), right(new Node(r)),
                           val(0), freq(l.freq + r.freq), code() {} // * used for merging 2 nodes
    const bool operator<(const Node &other) const
    {
        return this->freq < other.freq;
    }
    const bool operator>(const Node &other) const
    {
        return this->freq > other.freq;
    }
    const bool operator<=(const Node &other) const
    {
        return this->freq <= other.freq;
    }

    friend MyTree;
    friend ostream &operator<<(ostream &, const Node &); // * used to efficiently display the nodes
};

ostream &operator<<(ostream &output, const Node &node)
{
    output << '(' << node.freq << ',' << node.val << ')';
    return output;
}

class MyTree // * MyTree class for carring out the work
{
    Node *root;
    int total;

    // * Private Member Functions
    int countLetters(MyVector<char> &, int[]);
    void setupMyTree(MyVector<Node> &);
    void assignCodes(MyVector<Node> &);
    void LDRRetrieve(Node *, MyVector<Code> &);
    void LDRAssign(Node *, MyVector<char> &);
    void getCodes(MyVector<Code> &);
    void insertPathBased(Node *, MyVector<char> &, const int, const char);

public:
    MyTree();

    // * Encoder
    MyVector<Code> retrieveCodes();
    void assignCodes(MyVector<char> &);
    MyVector<char> encodeMsg(MyVector<char> &, MyVector<Code> &);

    MyVector<char> readFromFile(const char *);
    void saveToFile(const char *, MyVector<Code> &);
    void saveToFile(const char *, MyVector<char> &);

    // * Decoder
    MyVector<char> decodeMessage(MyVector<char> encoded);
    void reconstruct(MyVector<Code> &codes);
};

MyVector<Code> MyTree::retrieveCodes()
{
    MyVector<Code> dict(total);
    getCodes(dict); // * create a vector of codes of all nodes
    return dict;
}
void MyTree::assignCodes(MyVector<char> &filedata)
{
    // * creating an array of size totalascii and initializing it's values to 0
    int ascii[totalascii] = {
        0,
    };
    int total = countLetters(filedata, ascii); // * counts total number of unique letters in the text and updates their frequency in ascii

    MyVector<Node> nodes(total + 10, 'a'); // * extra 10 inorder to avoid rescaling

    for (int i = 32; i < totalascii; i++)
        // for (int i = 0; i < totalascii; i++)
        if (ascii[i] != 0)
            nodes.insertSorted(Node(ascii[i], i)); // * creating nodes and storing them in a vector

    cout << "\nNodes: ";
    nodes.display();
    setupMyTree(nodes); // * setting up the Tree
    assignCodes(nodes); // * assigning each node, a code from it's path
}

MyTree::MyTree() // * default constructor
{
    root = nullptr;
    total = 0;
}
void MyTree::LDRAssign(Node *curr, MyVector<char> &code) // * calculating the code for each node
{
    if (curr == nullptr)
        return;

    code.insert('0');
    LDRAssign(curr->left, code);
    code.pop();

    if (curr->val != 0)
        curr->code = code;

    code.insert('1');
    LDRAssign(curr->right, code);
    code.pop();
}

void MyTree ::assignCodes(MyVector<Node> &nodes)
{
    MyVector<char> code(nodes.getSize());
    LDRAssign(root, code);
}

void MyTree::LDRRetrieve(Node *curr, MyVector<Code> &codes)
{
    if (curr == nullptr)
        return;
    LDRRetrieve(curr->left, codes);
    if (curr->val != 0)
        codes.insert(Code(curr->val, curr->code)); // * creating a Code object with it's code and value and storing it in a vector
    LDRRetrieve(curr->right, codes);
}
void MyTree::getCodes(MyVector<Code> &codes)
{
    LDRRetrieve(root, codes);
}

void MyTree::setupMyTree(MyVector<Node> &nodes)
{
    while (nodes.getSize() != 1) // * while there are more than 1 nodes in the vector
    {
        nodes.insertSorted(Node(nodes[0], nodes[1])); // * merge the smallest 2 nodes
        nodes.remove(0);
        nodes.remove(0);
    }
    root = new Node(nodes[0]);
}
MyVector<char> MyTree::encodeMsg(MyVector<char> &message, MyVector<Code> &codes)
{
    MyVector<char> encoded;
    for (int i = 0; i < message.getSize(); i++)
    {
        int idx = codes.search(Code(message[i], MyVector<char>())); // * find index of Code object whose code we want to encode
        if (idx == -1)
            continue;
        for (int j = 0; j < codes[idx].bits.getSize(); j++) // * insert it's bits into the encoded vector
            encoded.insert(codes[idx].bits[j]);
    }
    return encoded;
}
int MyTree::countLetters(MyVector<char> &str, int ascii[])
{
    int total = 0;
    for (int i = 0; i < str.getSize(); i++) // * counts frequency of a letter
        ascii[str[i]]++;
    for (int i = 0; i < totalascii; i++) // * counts total number of unique letters
        if (ascii[i] != 0)
            total++;
    return total;
}
MyVector<char> MyTree::readFromFile(const char *filename)
{
    char in;
    MyVector<char> filedata;
    ifstream file(filename, ios::in);
    while ((in = file.get()) != EOF) // * insert char into filedata while not End Of File
        filedata.insert(in);
    file.close();
    return filedata;
}

void MyTree::saveToFile(const char *filename, MyVector<Code> &codes) // * saves Code objects in a file
{
    char nl = '\n';

    ofstream file(filename, ios::out);
    for (int i = 0; i < codes.getSize(); i++)
    {
        file.write(&codes[i].asci, sizeof(char)); // * store the char in the file
        for (int j = 0; j < codes[i].bits.getSize(); j++)
            file.write(&codes[i].bits[j], sizeof(char)); // * storing it's code
        file.write(&nl, sizeof(char));
    }
    file.close();
}
void MyTree::saveToFile(const char *filename, MyVector<char> &encoded)
{
    ofstream file(filename, ios::out);
    for (int i = 0; i < encoded.getSize(); i++) // * storing every char into the file
        file.write(&encoded[i], sizeof(char));
    file.close();
}

void MyTree::insertPathBased(Node *curr, MyVector<char> &bits, const int idx, const char asci)
{
    if (idx == bits.getSize() - 1) // * if last bit, meaning node is one step away. create node in the next step
    {
        if (bits[idx] == '0')
            curr->left = new Node(asci, bits);
        else
            curr->right = new Node(asci, bits);
        return;
    }

    if (bits[idx] == '0') // * turn left
    {
        if (curr->left == nullptr)
            curr->left = new Node();
        insertPathBased(curr->left, bits, idx + 1, asci);
    }
    else // * turn right
    {
        if (curr->right == nullptr)
            curr->right = new Node();
        insertPathBased(curr->right, bits, idx + 1, asci);
    }
}
void MyTree::reconstruct(MyVector<Code> &codes)
{
    char rootChar = 0;
    if (codes[0].bits.getSize() == 1)
        rootChar = codes[0].bits[0];

    root = new Node(rootChar);
    for (int i = 0; i < codes.getSize(); i++) // * creating the tree structure for each Code object
        insertPathBased(root, codes[i].bits, 0, codes[i].asci);
}
MyVector<char> MyTree::decodeMessage(MyVector<char> encoded)
{
    MyVector<char> result(encoded.getSize());
    Node *curr = root;
    for (int i = 0; i < encoded.getSize() + 1; i++) // * Decoding the code for each letter
    {
        if (curr->left == nullptr && curr->right == nullptr) // * If leaf node
        {
            result.insert(curr->val);
            curr = root;
            i--;
        }
        else // * if not leaf node
        {
            if (encoded[i] == '0') // * turn left
            {
                curr = curr->left;
            }
            else // * turn right
            {
                curr = curr->right;
            }
        }
    }
    return result;
}

// inline void MyTree::LevelOrderTraversal()
//{
//	queue<Node*> Que;
//	Node* curr = root;
//	Que.push(root);
//
//	while (!Que.empty())
//	{
//		curr = Que.front();
//		Que.pop();
//		if (curr->val != 0) {
//			cout << curr->val << ":";
//			curr->code.display();
//			cout << endl;
//		}
//		if (curr->left != nullptr)
//			Que.push(curr->left);
//		if (curr->right != nullptr)
//			Que.push(curr->right);
//	}
// }

MyVector<char> readMessage(const char *filename)
{
    char in;
    MyVector<char> filedata;
    ifstream file(filename, ios::in);
    while ((in = file.get()) != EOF) // * read each char from the file into filedata
        filedata.insert(in);
    file.close();
    return filedata;
}
bool isBit(const char ch) // * check if it is a bit
{
    return ch == '0' || ch == '1';
}
MyVector<Code> readCodes(const char *filename)
{
    char in;
    int idx = -1;
    MyVector<Code> codes;
    ifstream file(filename, ios::in);
    while ((in = file.get()) != EOF) // * reading code for each letter
        if (isBit(in))
            codes[idx].bits.insert(in); // * store code of the value
        else if (in == '\n')            // * used as a delimiter to know that line as ended for a code
            continue;
        else
        {
            codes.insert(Code(in, MyVector<char>())); // * store the value
            idx++;
        }
    file.close();
    return codes;
}

void displayCodes(MyVector<Code> codes)
{
    for (int i = 0; i < codes.getSize(); i++) // * displays the Code objects
    {
        cout << "\n"
             << codes[i].asci << ":";
        for (int j = 0; j < codes[i].bits.getSize(); j++)
            cout << codes[i].bits[j];
    }
}

// * Main Encoder
void encode()
{
    MyTree tree;

    MyVector<char> filedata = tree.readFromFile("file.txt");
    tree.assignCodes(filedata);

    MyVector<Code> codes = tree.retrieveCodes();
    tree.saveToFile("codes.txt", codes);

    MyVector<char> encodedmsg = tree.encodeMsg(filedata, codes);
    tree.saveToFile("encoded.txt", encodedmsg);
}

// * Main Decoder
void decode()
{
    MyTree tree;
    MyVector<Code> codes = readCodes("codes.txt");
    tree.reconstruct(codes);

    MyVector<char> encoded = readMessage("encoded.txt");
    MyVector<char> decoded = tree.decodeMessage(encoded);
    tree.saveToFile("decoded.txt", decoded);

    cout << "\n\nDecoded Text: ";
    decoded.display();
}

// * Interface
void Encoder_Decoder()
{
    cout << "\nWelcome to the Encoding/Decoding Algorithm!"
         << "\n====================================================\n\n";

    encode();
    decode();

    cout << "\n\n\nExiting..."
         << "\n\n====================================================\n\n";
}

int main()
{
    Encoder_Decoder();

    return 0;
}
