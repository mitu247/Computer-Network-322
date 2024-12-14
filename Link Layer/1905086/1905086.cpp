#include<bits/stdc++.h>
#include<windows.h>
#include<random>
#include<cstdlib>
#include<ctime>
using namespace std;
void print_error_colour(vector<string> msg, vector<int> pos){
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    WORD originalAttributes = consoleInfo.wAttributes;

    for(int i = 0; i < msg.size(); i++){
       for(int j = 0; j < msg[0].size(); j++){
            if(find(pos.begin(),pos.end(),(msg.size()*j)+i) == pos.end()){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
                cout<<msg[i][j]<<flush;
            }
            else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
                cout<<msg[i][j]<<flush;
            }
       }
       cout<<endl;
    }
    cout<<endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
}
void printColouredCheckBits(vector<string> msg) {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    WORD originalAttributes = consoleInfo.wAttributes;

    for (string str: msg) {
        int mask = 1;
        for (int i=1; i <= str.size(); i++) {
            if (i == mask) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
                cout << str[i-1] << flush;
                mask = mask * 2;
            }
            else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
                cout << str[i-1] << flush;
            }
        }
        cout << endl;
    }

    cout << endl;
}
void printCyan(string msg,int sz){
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    //cout<<msg<<endl;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    WORD originalAttributes = consoleInfo.wAttributes;
    for(int i = 0; i < msg.size(); i++){
        if(i>=msg.size()-sz){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
            cout<<msg[i]<<flush;
        }
        else{
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
            cout<<msg[i]<<flush;
        }
    }
    cout<<endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
}
double generateRandomNumber(int cnt) {
    //random_device rd;
    srand(cnt);
    mt19937 gen(rand());
    uniform_real_distribution<> dis(0.0, 1.0);

    return dis(gen);
}
string getRemainder(string msg, string polynomial){
    string factor,rem;
    string dd(msg.begin(), msg.begin()+polynomial.size());

    for(int i = polynomial.size(); i < msg.size()+1; ){
        factor = "";
        rem = "";
        if(dd[0] == '1'){
            factor = polynomial;
        }
        else{
           for(int j = 0; j<polynomial.size(); j++){
            factor+="0";
           }
        }
        //cout<<"D   "<<polynomial<<" DD  "<<dd<<" FACTOR  "<<factor<<"   ";
        int init_zeroCount = 0;
        for(int j = 0; j<polynomial.size(); j++){
            if(dd[j] == factor[j]){
                rem+="0";
            }
            else{
                rem+="1";
            }
        }
        for(int j = 0; j<rem.size(); j++){
        if(rem[j] == '1'){
          break;
        }
        init_zeroCount++;
        }
        //cout<<rem<<endl;
        dd.clear();

        for(int j = 0; j < init_zeroCount; j++){
            if(i+j<msg.size())
             rem += msg[i+j];
        }
        dd = rem;
        dd.erase(0,init_zeroCount);
        //rem.erase(0,1);
        i+=init_zeroCount;
    }
    while(rem.size() != polynomial.size()-1){
        rem.erase(0,1);
    }
    //cout<<rem<<" Size: "<<rem.size();
    return rem;
}
int main()
{
    string data_string;
    int m;
    double p;
    string gen_poly;
    vector<int> errorIdx;
    cout<<"enter data string: ";
    getline(cin, data_string);
    cout<<"enter number of data bytes in a row (m): ";
    cin>>m;
    cout<<"enter probability (p): ";
    cin>>p;
    cout<<"enter generator polynomial: ";
    cin>>gen_poly;
   // cout<<data_string<<"   "<<m<<" "<<p<<"  "<<gen_poly;
    if(data_string.size()%m != 0)
    {
        int padding_count = m - (data_string.size()%m);
        cout<<padding_count<<endl;
        for(int i = 0; i<padding_count; i++)
        {
            data_string+="~";
        }
    }
    cout<<endl;
    cout<<"data string after padding: "<< data_string <<endl;
    cout<<endl;
    string bitString = "";

    for(int i = 0; i<data_string.size(); i++)
    {
        bitString += std::bitset<8>(data_string[i]).to_string();
    }

    int blockCount = data_string.size()/m;
    int copySize = 8*m;
    int start_position = 0;
    vector<string>dataBlocks;
    for(int i = 1; i <= blockCount; i++)
    {
        dataBlocks.push_back(bitString.substr(start_position,copySize));
        start_position = i*copySize;
    }
    cout<<endl;
    cout<<"data block (ascii code of m characters per row):"<<endl;
    for(int i = 0; i<dataBlocks.size(); i++)
    {
        cout<<dataBlocks[i]<<endl;
    }
    cout<<endl;
    int parity_bit = 0;

    while(pow(2,parity_bit)-parity_bit < copySize+1)
    {
        parity_bit++;
    }

    for(int i = 0; i<dataBlocks.size(); i++)
    {
        dataBlocks[i].insert(0,"X");
        int r = 1;
        for(int j = 0; j < parity_bit; j++)
        {
            dataBlocks[i].insert(r,"X");
            r = 2*r;
        }
    }
    for(int i = 0; i<dataBlocks.size(); i++)
    {
        int r = 1;
        for(int j = 0; j < parity_bit; j++)
        {
            char bit = 'x';
            for(int k = r+1; k<dataBlocks[i].size(); k++)
            {

                if(r == (r&k))
                {
                    if(bit == 'x')
                    {
                        bit = dataBlocks[i][k];
                    }
                    else if(bit == dataBlocks[i][k])
                    {
                        bit = '0';
                    }
                    else if(bit != dataBlocks[i][k])
                    {
                        bit = '1';
                    }
                }
            }
            dataBlocks[i].erase(r,1);
            dataBlocks[i].insert(r,1,bit);
            r=2*r;
        }
        dataBlocks[i].erase(0,1);
    }
    cout<<"data block after adding check bits: "<<endl;
    printColouredCheckBits(dataBlocks);

    string column_wise_data= "";

    for(int i = 0; i < dataBlocks[0].size(); i++){
        for(int j = 0; j< dataBlocks.size(); j++){
            column_wise_data += dataBlocks[j][i];
        }
    }
    cout<<"data bits after column-wise serialization: "<<endl;
    cout<<column_wise_data<<endl;
    cout<<endl;

    for(int i = 0; i < gen_poly.size()-1; i++){
        column_wise_data +="0";
    }

    string remainder = getRemainder(column_wise_data,gen_poly);
    for(int i = 0; i< gen_poly.size() - 1; i++){
        column_wise_data.pop_back();
    }
    column_wise_data+=remainder;

    cout<<"data bits after appending CRC checksum <sent frame>: "<<endl;
    printCyan(column_wise_data,remainder.size());
    cout<<endl;

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    WORD originalAttributes = consoleInfo.wAttributes;
    cout<<"received frame: "<<endl;

    for(int i = 0; i < column_wise_data.size(); i++){
        double random = generateRandomNumber(i+1);

        if(random <= p){
            if(column_wise_data[i] == '0'){
                column_wise_data[i] = '1';
            }
            else{
                column_wise_data[i] = '0';
            }
            errorIdx.push_back(i);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
            cout<<column_wise_data[i]<<flush;
        }
        else{
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
            cout<<column_wise_data[i]<<flush;
        }
    }
    cout<<endl;
    cout<<endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), originalAttributes);
    string checkRem = getRemainder(column_wise_data,gen_poly);
   // cout<<checkRem<<endl;
    bool check = true;
    for(int i = 0; i < checkRem.size(); i++){
        if(checkRem[i] != '0'){
            check = false;
            break;
        }
    }
    if(check == false){
        cout<<"result of CRC checksum matching: error detected"<<endl;
    }
    else{
        cout<<"result of CRC checksum matching: no error detected"<<endl;
    }
    cout<<endl;
    for(int i = 0 ; i < gen_poly.size()-1; i++){
        column_wise_data.pop_back();
    }

    blockCount = column_wise_data.size()/((8*m)+parity_bit);
    copySize = 8*m + parity_bit;
    vector<string> receivedBlocks(blockCount);

    for (int i = 0; i < blockCount; ++i) {
        for (int j = 0; j < copySize; ++j) {
            receivedBlocks[i] += column_wise_data[i + (j * blockCount)];
        }
    }
    cout<<endl;
    cout<<"data block after removing CRC checksum bits: "<<endl;
    print_error_colour(receivedBlocks,errorIdx);
    cout<<endl;

    vector<string> bitSeqence;
    for(int i = 0; i<receivedBlocks.size(); i++)
    {
        receivedBlocks[i].insert(0,"X");
        int r = 1;
        string binSeq = "";
        for(int j = 0; j < parity_bit; j++)
        {
            char bit = 'x';
            for(int k = r+1; k<receivedBlocks[i].size(); k++)
            {

                if(r == (r&k))
                {
                    if(bit == 'x')
                    {
                        bit = receivedBlocks[i][k];
                    }
                    else if(bit == receivedBlocks[i][k])
                    {
                        bit = '0';
                    }
                    else if(bit != receivedBlocks[i][k])
                    {
                        bit = '1';
                    }
                }
            }
            if(receivedBlocks[i][r] == bit){
                binSeq +="0";
            }
            else{
                binSeq +="1";
            }
            r=2*r;
        }
        bitSeqence.push_back(binSeq);
        receivedBlocks[i].erase(0,1);
    }
    int i = 0;
    for(string binSeq: bitSeqence){
        reverse(binSeq.begin(),binSeq.end());

    std::bitset<64> binaryBits(binSeq);
    unsigned long decimalNumber = binaryBits.to_ulong();
    if(decimalNumber != 0){
       receivedBlocks[i][decimalNumber-1] = (receivedBlocks[i][decimalNumber-1] == '0')? '1':'0';
    }
    i++;
    //cout << "Binary String: " <<binSeq << endl;
    //cout << "Decimal Number: " << decimalNumber << endl;
    }

    for (int i = 0; i < receivedBlocks.size(); i++) {
    int blockSize = receivedBlocks[i].size();
    int result = log2(static_cast<double>(blockSize));
    int pa = pow(2,result);
    for (; pa >= 1; pa /= 2) {
        int position = pa - 1;
        //cout << position << endl;
        receivedBlocks[i].erase(position, 1);
    }
    }
    cout<<"data block after removing check bits:"<<endl;
    for(string msg:receivedBlocks){
        cout<<msg<<endl;
    }
    cout<<endl;
    string output = "";
    for(int i = 0; i < receivedBlocks.size(); i++){
        for(int j = 0; j < m; j++){
            string binAscii = receivedBlocks[i].substr(j*8,8);
            int asciiValue = std::bitset<8>(binAscii).to_ulong();
            char character = static_cast<char>(asciiValue);
            output+=character;
        }
    }
    cout<<"output frame: "<<output<<endl;
    return 0;
}
