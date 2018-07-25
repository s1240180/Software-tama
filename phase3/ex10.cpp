#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <dirent.h>
#include <sqlite3.h>
#include <sstream>

using namespace std;

#define TABLE_NUM 4

class DB;

struct PAGE{
    string name;
    vector<string> link;
    vector<string> word;
    double page_score;
    double word_score;
};

struct USER{
    string name;
    vector<string> friend_list;
    map<string,double> book_review;
};

struct REVSC{
    string name;
    double score;
};

namespace CP{
    bool WS(const PAGE &d1,const PAGE &d2){
        return d1.word_score > d2.word_score;
    }
    
    bool PS(const PAGE &d1,const PAGE &d2){
        return d1.page_score > d2.page_score;
    }
    
    bool TS(const PAGE &d1,const PAGE &d2){
        return d1.page_score + d1.word_score > d2.page_score + d2.word_score;
    }
    
    bool RV(const REVSC &d1, const REVSC &d2){
        return d1.score > d2.score;
    }
}


bool InsertString(vector<string> &base, string in){
    bool same = false;
    
    for(vector<string>::size_type i = 0;i < base.size();i++){
        if(base[i] == in){
            same = true;
            break;
        }
    }
    if(!same)
        base.push_back(in);
    
    return !same;
}

bool DeleteString(vector<string> &page_data, string rmstr){
    bool exist = false;
    
    for(vector<string>::size_type i = 0;i < page_data.size();i++){
        if(page_data[i] == rmstr){
            exist = true;
            page_data.erase(page_data.begin() + i);
            break;
        }
    }
    
    return exist;
}



class DB{
private:
    vector<PAGE> page_data;
    vector<USER> users;
    vector<string> books;
    vector<string> documents;
    
    void ResitPage(string new_doc){
        PAGE tmp;
        ifstream fin;
        string path;
        string in;
        
        tmp.name = new_doc;
        path = "source/" + tmp.name;
        fin.open(path.c_str());
        while(fin){
            fin >> in;
            if(in.find(".html") != string::npos)
                tmp.link.push_back(in);
            else
                tmp.word.push_back(in);
        }
        tmp.word_score = 0;
        tmp.page_score = 1.0;
        
        page_data.push_back(tmp);
        fin.close();
    }
    
    void ResetDoc(){
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++){
            page_data[i].word_score = 1.0;
            page_data[i].page_score = 0;
        }
    }
    
    double sum(string name,vector<PAGE> &page_data){
        double result = 0;
        
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
            for(vector<string>::size_type j = 0;j < page_data[i].link.size();j++)
                if(name == page_data[i].link[j])
                    result += page_data[i].page_score / page_data[i].link.size();
        
        return result;
    }
    
    void DocumentScore(vector<string> &search_words){
        double word_max, page_max;
        
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
            for(vector<string>::size_type j = 0;j < page_data[i].word.size();j++)
                for(vector<string>::size_type k = 0;k < search_words.size();k++)
                    if(page_data[i].word[j] == search_words[k])
                        page_data[i].word_score += 1.0;
        
        for(int c = 0;c < 20;c++)
            for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
                page_data[i].page_score =  0.15 + 0.85 * sum(page_data[i].name,page_data);
        
        sort(page_data.begin(),page_data.end(),CP::WS);
        word_max = page_data[0].word_score;
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
            page_data[i].word_score = page_data[i].word_score / word_max;
        
        sort(page_data.begin(),page_data.end(),CP::PS);
        page_max = page_data[0].page_score;
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
            page_data[i].page_score = page_data[i].page_score / page_max;
        
        sort(page_data.begin(),page_data.end(),CP::TS);
    }
    
    map<string,double> ReviewScore(string uname){
        map<string,double> book_score_list;
        vector<USER>::size_type key = 0;
        bool exist = false;
        vector<REVSC> review_list;
        double rev_max;
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                exist = true;
                key = i;
                break;
            }
        }
        
        if(!exist){
            cout << uname << " is not found use list" << endl;
            return book_score_list;
        }
        
        for(vector<string>::size_type j = 0;j < books.size();j++)
            book_score_list[books[j]] = 0;
        
        for(vector<string>::size_type i = 0;i < users[key].friend_list.size();i++)
            for(vector<USER>::size_type j = 0;j < users.size();j++)
                if(users[key].friend_list[i] == users[j].name)
                    for(map<string,double>::iterator it = users[j].book_review.begin();it != users[j].book_review.end();it++)
                        book_score_list[it->first] += it->second;
        
        for(map<string,double>::iterator it = book_score_list.begin();it != book_score_list.end();it++){
            REVSC tmp;
            tmp.name = it->first;
            tmp.score = it->second;
            review_list.push_back(tmp);
        }
        
        sort(review_list.begin(),review_list.end(),CP::RV);
        rev_max = review_list[0].score;
        book_score_list.clear();
        
        if(rev_max > 0)
            for(vector<REVSC>::size_type i = 0;i < review_list.size();i++)
                book_score_list[review_list[i].name] = review_list[i].score / rev_max;
        
        return book_score_list;
    }
    
    
    
public:
    vector<USER> getUser(){
        return users;
    }
    
    vector<string> getDoc(){
        return documents;
    }
    
    vector<string> getBook(){
        return books;
    }
    
    void SetData(vector<USER> u, vector<string> b, vector<string> d){
        users = u;
        books = b;
        documents = d;
    }
    
    void InsertUser(string new_users){
        bool same = false;
        USER tmp;
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == new_users){
                same = true;
                break;
            }
        }
        
        if(!same){
            tmp.name = new_users;
            users.push_back(tmp);
            cout << new_users << " is enrolled as user" << endl << endl;
        }
        else
            cout << new_users << " is already existed as user" << endl << endl;
    }
    
    void SetFriend(string name,string fname){
        bool in_user = false;
        bool in_friend = false;
        bool same = false;
        
        if(name == fname){
            cout << "user name and friend name is same" << endl;
            return;
        }
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(name == users[i].name)
                in_user = true;
            if(fname == users[i].name)
                in_friend = true;
        }
        
        if(!in_user || !in_friend){
            cout << "input user is not found" << endl << endl;
            return;
        }
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == name){
                for(vector<string>::size_type j = 0;j < users[i].friend_list.size();j++){
                    if(users[i].friend_list[j] == fname){
                        same = true;
                        break;
                    }
                }
                if(!same)
                    users[i].friend_list.push_back(fname);
                break;
            }
        }
        
        cout << fname << " is enrolled as " << name << "'s friend" << endl << endl;
    }
    
    void InsertBook(string new_book,bool view = true){
        bool in = InsertString(books,new_book);
        
        if(!view)
            return;
        
        if(in)
            cout << new_book << " is enrolled as book" << endl << endl;
        else
            cout << new_book << " is already existed as book" << endl << endl;
    }
    
    void InsertDocument(string new_doc){
        if(InsertString(documents,new_doc)){
            cout << new_doc << " is enrolled as document" << endl << endl;
            ResitPage(new_doc);
            
            for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
                for(vector<string>::size_type j = 0;j < page_data[i].word.size();j++)
                    InsertBook(page_data[i].word[j],false);
        }
        else
            cout << new_doc << " is already existed as document" << endl << endl;
    }
    
    void RemoveUser(string uname){
        bool exist = false;
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                exist = true;
                users.erase(users.begin() + i);
                break;
            }
        }
        
        if(!exist){
            cout << uname << " is not found user list" << endl;
            return;
        }
        
        for(vector<USER>::size_type i = 0;i < users.size();i++)
            DeleteString(users[i].friend_list,uname);
        
        cout << uname << "'s data is removed" << endl << endl;
    }
    
    void RemoveFriend(string uname, string fname){
        bool u_exist = false;
        bool f_exist = false;
        
        for(vector<string>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname)
                u_exist = true;
            if(users[i].name == fname)
                f_exist = true;
        }
        
        if(!u_exist || !f_exist)
            return;
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                if(DeleteString(users[i].friend_list, fname))
                    cout << fname << " is removed from "<< uname << endl << endl;
                else
                    cout << uname << " and " << fname << " is not friend" << endl << endl;
                break;
            }
        }
    }
    
    void RemoveBook(string rmbook){
        if(DeleteString(books, rmbook))
            cout << rmbook << "'s page_data is removed" << endl << endl;
        else
            cout << rmbook << " is not found in book list" << endl << endl;
    }
    
    void RemoveDocument(string rmdoc){
        if(DeleteString(documents, rmdoc))
            cout << rmdoc << "'s page_data is removed" << endl << endl;
        else
            cout << rmdoc << " is not found in document list" << endl << endl;
    }
    
    void AddReview(string uname ,string book_title, double score){
        bool b_exist = false;
        bool u_exist = false;
        vector<USER>::size_type key = 0;
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                key = i;
                u_exist = true;
                break;
            }
        }
        
        if(!u_exist){
            cout << uname << " is not found user list" << endl;
            return;
        }
        
        for(vector<string>::size_type i = 0;i < books.size();i++){
            if(books[i] == book_title){
                b_exist = true;
                break;
            }
        }
        
        if(!b_exist){
            cout << book_title << " is not found book list" << endl << endl;
            return;
        }
        
        users[key].book_review[book_title] = score;
        cout << "review update is success" << endl << endl;
    }
    
    void DeleteReview(string uname, string book_title){
        bool b_exist = false;
        bool u_exist = false;
        vector<USER>::size_type key = 0;
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                key = i;
                u_exist = true;
                break;
            }
        }
        
        if(!u_exist){
            cout << uname << " is not found user list" << endl;
            return;
        }
        
        for(vector<string>::size_type i = 0;i < books.size();i++){
            if(books[i] == book_title){
                b_exist = true;
                break;
            }
        }
        
        if(!b_exist){
            cout << book_title << " is not found book list" << endl << endl;
            return;
        }
        
        for(map<string,double>::iterator it = users[key].book_review.begin();it != users[key].book_review.end();it++){
            if(it->first == book_title){
                users[key].book_review.erase(it);
                cout << "book review is deleted" << endl << endl;
                return;
            }
        }
        
        cout << uname << " didn't review " << book_title << endl << endl;
    }
    
    void InsertWL(string docname){
        string line;
        string::size_type pos = 0;
        
        if(docname.find(".html") == string::npos){
            cout << "input word is not document file" << endl << endl;
            return;
        }
        
        for(vector<PAGE>::size_type k = 0;k < page_data.size();k++){
            if(page_data[k].name == docname){
                cout << "input page contents" << endl;
                cin.ignore();
                getline(cin, line);
                
                for(string::size_type i = 0;i < line.size();i++){
                    if(line[i] == ' '){
                        if(line.substr(pos, i - pos).find(".html") != string::npos)
                            InsertString(page_data[k].link, line.substr(pos, i - pos));
                        else
                            InsertString(page_data[k].word, line.substr(pos, i - pos));
                        pos = i + 1;
                    }
                    
                    if(i == line.size() - 1){
                        if(line.substr(pos, line.size() - pos).find(".html") != string::npos)
                            InsertString(page_data[k].link, line.substr(pos,line.size() - pos));
                        else
                            InsertString(page_data[k].word, line.substr(pos,line.size() - pos));
                    }
                }
                
                for(vector<string>::size_type i = 0;i < page_data[k].word.size();i++)
                    InsertBook(page_data[k].word[i],false);
                
                cout << "page update is finished" << endl << endl;
                return;
            }
        }
        
        cout << "page is not found" << endl << endl;
    }
    
    void DeleteWL(string docname){
        string del;
        
        if(docname.find(".html") == string::npos){
            cout << "input word is not document file" << endl << endl;
            return;
        }
        
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++){
            bool success = false;
            
            if(page_data[i].name == docname){
                cout << "input delete word" << endl;
                cin >> del;
                
                if(del.find(".html") != string::npos)
                    success = DeleteString(page_data[i].link,del);
                else
                    success = DeleteString(page_data[i].word,del);
                
                if(success)
                    cout << "page update is success" << endl << endl;
                else
                    cout << "input word is not found in page " << docname << endl << endl;
                
                return;
            }
        }
        
        cout << "page is not found" << endl << endl;
    }
    
    void ReadDocumentHTML(){
        PAGE tmp;
        ifstream fin;
        string in;
        string path;
        
        for(vector<string>::size_type i = 0;i < documents.size();i++){
            tmp.name = documents[i];
            path = "source/" + tmp.name;
            fin.open(path.c_str());
            while(fin >> in){
                if(in.find(".html") != string::npos)
                    tmp.link.push_back(in);
                else
                    tmp.word.push_back(in);
            }
            tmp.word_score = 0;
            tmp.page_score = 1.0;
            
            page_data.push_back(tmp);
            fin.close();
            tmp.link.clear();
            tmp.word.clear();
        }
        
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++)
            for(vector<string>::size_type j = 0;j < page_data[i].word.size();j++)
                InsertBook(page_data[i].word[j],false);
    }
    
    void WriteDocumetsHTML(){
        ofstream fout;
        string path;
        
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++){
            path = "source/" + page_data[i].name;
            fout.open(path.c_str());
            
            for(vector<string>::size_type j = 0;j < page_data[i].link.size();j++)
                fout << page_data[i].link[j] << endl;
            for(vector<string>::size_type j = 0;j < page_data[i].word.size();j++)
                fout << page_data[i].word[j] << endl;
            
            fout.clear();
            fout.close();
        }
    }
    
    bool CheckUser(string name){
        bool exist = false;
        
        for(vector<string>::size_type i = 0;i < users.size();i++){
            if(users[i].name == name){
                exist = true;
                break;
            }
        }
        
        return exist;
    }
    
    void ShowUser(){
        for(vector<USER>::size_type i = 0;i < users.size();i++)
            cout << users[i].name << endl;
        cout << endl;
    }
    
    void ShowBook(){
        for(vector<string>::size_type i = 0;i < books.size();i++)
            cout << books[i] << endl;
        cout << endl;
    }
    
    void ShowDocumentTitle(){
        for(vector<string>::size_type i = 0;i < documents.size();i++)
            cout << documents[i] << endl;
        cout << endl;
    }
    
    void ShowUserFriend(string uname){
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                cout << "[" << users[i].name << "'s friends]" << endl;
                for(vector<string>::size_type j = 0;j < users[i].friend_list.size();j++)
                    cout << users[i].friend_list[j] << endl;
                cout << endl;
                return;
            }
        }
        
        cout << uname << " is not found user list" << endl << endl;
    }
    
    void ShowUserReview(string uname){
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].name == uname){
                cout << "[" << users[i].name << "'s book review and review score]" << endl;
                for(map<string,double>::iterator it = users[i].book_review.begin();it != users[i].book_review.end();it++)
                    cout << it->first << "|" << it->second << endl;
                cout << endl;
                return;
            }
        }
        
        cout << uname << " is not found user list" << endl << endl;
    }
    
    void ShowDocumentDetail(string title){
        for(vector<PAGE>::size_type i = 0;i < page_data.size();i++){
            if(page_data[i].name == title){
                for(vector<string>::size_type j = 0;j < page_data[i].link.size();j++)
                    cout << page_data[i].link[j] << endl;
                for(vector<string>::size_type j = 0;j < page_data[i].word.size();j++)
                    cout << page_data[i].word[j] << endl;
                cout << endl;
                return;
            }
        }
        
        cout << title << " is not found document list" << endl << endl;
    }
    
    void GetRecBookList(string name, vector<string> &search_words){
        map<string,double> rev;
        vector<REVSC> total_score;
        ResetDoc();
        DocumentScore(search_words);
        rev = ReviewScore(name);
        
        for(vector<string>::size_type i = 0;i < books.size();i++)
            for(vector<PAGE>::size_type j = 0;j < page_data.size();j++)
                rev[books[i]] += page_data[j].page_score + page_data[j].word_score;
        
        for(vector<string>::size_type i = 0;i < search_words.size();i++){
            for(vector<string>::size_type j = 0;j < books.size();j++){
                if(search_words[i] == books[j])
                    rev[books[j]] += 5.0;
                if(search_words[i].find(books[j]) != string::npos)
                    rev[books[i]] += 3.0;
            }
        }
        
        for(map<string,double>::iterator it = rev.begin();it != rev.end();it++){
            REVSC tmp;
            tmp.name = it->first;
            tmp.score = it->second;
            total_score.push_back(tmp);
        }
        
        sort(total_score.begin(),total_score.end(),CP::RV);
        
        cout << endl << "[" << name << "'s recommend book list]" << endl;
        for(vector<REVSC>::size_type i = 0;i < total_score.size();i++){
            cout << total_score[i].name << endl;
            if(i > 10)
                break;
        }
        cout << endl;
    }
};


namespace SQLf {
    int getUserFriend(void *get, int argc, char **argv, char **azColName){
        vector<USER> &data = *((vector<USER>*)get);
        vector<USER>::size_type key = 0;
        USER tmp;
        string uname = argv[0];
        string fname = argv[1];
        bool same = false;
        
        for(vector<USER>::size_type i = 0;i < data.size();i++){
            if(data[i].name == uname){
                same = true;
                key = i;
                break;
            }
        }
        
        if(same && fname != "no_friend")
            data[key].friend_list.push_back(fname);
        if(!same){
            tmp.name = uname;
            if(fname != "no_friend")
                tmp.friend_list.push_back(fname);
            data.push_back(tmp);
        }
        
        return SQLITE_OK;
    }
    
    int getUserReview(void *get, int argc, char **argv, char **azColName){
        map<string,double> &data = *(map<string,double>*)get;
        string arg = argv[0];
        double score = atof(argv[1]);
        data[arg] = score;
        return SQLITE_OK;
    }
    
    int getStrDB(void *get, int argc, char **argv, char **azColName){
        vector<string> *data = (vector<string>*)get;
        data->push_back(argv[0]);
        return SQLITE_OK;
    }
    
    void CreateTable(sqlite3 *db){
        char *error = 0;
        sqlite3_exec(db,"create table books (book text)",NULL,NULL,&error);
        sqlite3_exec(db,"create table doc (file text)",NULL,NULL,&error);
        sqlite3_exec(db,"create table FriendList (name text,friend text)",NULL,NULL,&error);
        sqlite3_exec(db,"create table ReviewList (name text,rbook text,review float)",NULL,NULL,&error);
    }
    
    bool CheckTable(sqlite3 *db){
        char *error = 0;
        vector<string> tables;
        sqlite3_exec(db,"select name from sqlite_master where type = 'table'",getStrDB,(void*)&tables,&error);
        return tables.size() == TABLE_NUM;
    }
    
    void ReadDataBase(sqlite3 *db,DB &DATA){
        char *error = 0;
        string order;
        vector<USER> user;
        vector<string> books;
        vector<string> doc;
        
        sqlite3_exec(db,"select * from books",getStrDB,(void*)&books,&error);
        sqlite3_exec(db,"select * from doc",getStrDB,(void*)&doc,&error);
        sqlite3_exec(db,"select * from FriendList",getUserFriend,(void*)&user,&error);
        
        for(vector<USER>::size_type i = 0;i < user.size();i++){
            order = "select rbook,review from ReviewList where name = '" + user[i].name + "'";
            sqlite3_exec(db,order.c_str(),getUserReview,(void*)&user[i].book_review,&error);
        }
        
        DATA.SetData(user,books,doc);
        DATA.ReadDocumentHTML();
    }
    
    void WriteDataBase(sqlite3 *db, DB &DATA){
        char *error;
        string order;
        stringstream r_order;
        vector<USER> users = DATA.getUser();
        vector<string> documents = DATA.getDoc();
        vector<string> books = DATA.getBook();
        
        //delete
        sqlite3_exec(db,"delete from FriendList",NULL,NULL,&error);
        sqlite3_exec(db,"delete from ReviewList",NULL,NULL,&error);
        sqlite3_exec(db,"delete from books",NULL,NULL,&error);
        sqlite3_exec(db,"delete from doc",NULL,NULL,&error);
        
        //user
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            if(users[i].friend_list.size() > 0){
                for(vector<string>::size_type j = 0;j < users[i].friend_list.size();j++){
                    order = "insert into FriendList (name, friend) values ('" + users[i].name + "','" + users[i].friend_list[j] + "')";
                    sqlite3_exec(db,order.c_str(),NULL,NULL,&error);
                }
            }
            else{
                order = "insert into FriendList (name, friend) values ('" + users[i].name + "','no_friend')";
                sqlite3_exec(db,order.c_str(),NULL,NULL,&error);
            }
        }
        
        for(vector<USER>::size_type i = 0;i < users.size();i++){
            for(map<string,double>::iterator it = users[i].book_review.begin();it != users[i].book_review.end();it++){
                r_order << "insert into ReviewList (name, rbook,score) values ('" << users[i].name << "','" << it->first << "'," << it->second << ")";
                sqlite3_exec(db,r_order.str().c_str(),NULL,NULL,&error);
            }
        }
        
        //book
        for(vector<string>::size_type i = 0;i < books.size();i++){
            order = "insert into books (book) values ('" + books[i] + "')";
            sqlite3_exec(db,order.c_str(),NULL,NULL,&error);
        }
        
        //doc
        for(vector<string>::size_type i = 0;i < documents.size();i++){
            order = "insert into doc (file) values ('" + documents[i] + "')";
            sqlite3_exec(db,order.c_str(),NULL,NULL,&error);
        }
        
        DATA.WriteDocumetsHTML();
    }
}

void SelectMode(DB&);
void DataInsert(DB&);
void DataRemove(DB&);
void DataUpdate(DB&);
void DataShow(DB&);
void ShowRecommendBook(DB&);

int main(){
    DB DATA;
    sqlite3 *db;
    sqlite3_open("bookdata.db",&db);
    
    if(SQLf::CheckTable(db)){
        cout << "data base is exist" << endl;
        SQLf::ReadDataBase(db,DATA);
    }
    else{
        cout << "data base is not exist" << endl;
        cout << "create new page_data base" << endl;
        SQLf::CreateTable(db);
    }
    
    SelectMode(DATA);
    SQLf::WriteDataBase(db,DATA);
    sqlite3_close(db);
    
    return 0;
}

void SelectMode(DB &DATA){
    int mode;
    
    while(1){
        cout << "select mode" << endl;
        cout << "1:insert data" << endl;
        cout << "2:remove data" << endl;
        cout << "3:update data" << endl;
        cout << "4:show data" << endl;
        cout << "5:show recommend book" << endl;
        cout << "6:exit" << endl;
        
        cin >> mode;
        
        if(mode == 1)
            DataInsert(DATA);
        else if(mode == 2)
            DataRemove(DATA);
        else if(mode == 3)
            DataUpdate(DATA);
        else if(mode == 4)
            DataShow(DATA);
        else if(mode == 5)
            ShowRecommendBook(DATA);
        else if(mode == 6){
            system("clear");
            cout << "System exit" << endl;
            return;
        }
        else
            cout << "number is not right" << endl << endl;
    }
}

void DataShow(DB &DATA){
    int mode;
    string in;
    
    system("clear");
    while(1){
        cout << "[data show mode]" << endl;
        cout << "1:user list" << endl;
        cout << "2:friend list" << endl;
        cout << "3:book list" << endl;
        cout << "4:review list" << endl;
        cout << "5:document title list" << endl;
        cout << "6:document detail list" << endl;
        cout << "7:exit" << endl;
        
        cin >> mode;
        system("clear");
        switch(mode){
            case 1:
                cout << "[user list]" << endl;
                DATA.ShowUser();
                break;
                
            case 2:
                cout << "[input user name]" << endl;
                cin >> in;
                DATA.ShowUserFriend(in);
                break;
                
            case 3:
                cout << "[book list]" << endl;
                DATA.ShowBook();
                break;
                
            case 4:
                cout << "[input user name]" << endl;
                cin >> in;
                DATA.ShowUserReview(in);
                break;
                
            case 5:
                cout << "[document title list]" << endl;
                DATA.ShowDocumentTitle();
                break;
                
            case 6:
                cout << "[input document title]" << endl;
                cin >> in;
                DATA.ShowDocumentDetail(in);
                break;
                
            case 7:
                system("clear");
                cout << "data show mode exit" << endl << endl;
                return;
                
            default:
                cout << "number is not right" << endl << endl;
                break;
                
        }
    }
}

void ShowRecommendBook(DB &DATA){
    string line, name;
    vector<string> word_list;
    string::size_type pos = 0;
    
    cout << "input your name" << endl;
    cin >> name;
    
    if(!DATA.CheckUser(name)){
        cout << name << " is not found user list" << endl;
        return;
    }
    
    cin.ignore();
    cout << "input search word" << endl;
    getline(cin,line);
    
    for(string::size_type i = 0;i < line.size();i++){
        if(line[i] == ' '){
            InsertString(word_list, line.substr(pos, i - pos));
            pos = i + 1;
        }
        
        if(i == line.size() - 1)
            InsertString(word_list, line.substr(pos, line.size() - pos));
    }
    
    DATA.GetRecBookList(name, word_list);
}

void DataUpdate(DB &DATA){
    int mode;
    string in;
    string uname,book_title;
    double score;
    
    system("clear");
    while(1){
        cout << "[update data mode]" << endl;
        cout << "1:add book review" << endl;
        cout << "2:delete book review" << endl;
        cout << "3:show review list" << endl;
        cout << "4:insert word or link to document" << endl;
        cout << "5:delete word or link to document" << endl;
        cout << "6:show document detail" << endl;
        cout << "7:exit" << endl;
        
        cin >> mode;
        system("clear");
        switch(mode){
            case 1:
                cout << endl << "[add book review]" << endl;
                cout << "input user name, book title, review score(0.0 ~ 5.0)" << endl;
                cin >> uname >> book_title >> score;
                if(0 <= score && score <= 5)
                    DATA.AddReview(uname,book_title,score);
                else
                    cout << "input score is not right" << endl;
                break;
                
            case 2:
                cout << endl << "[delete book review]" << endl;
                cout << "input user name, book title" << endl;
                cin >> uname >> book_title;
                DATA.DeleteReview(uname,book_title);
                break;
                
            case 3:
                cout << endl << "[show user review list]" << endl;
                cout << "input user name" << endl;
                cin >> uname;
                DATA.ShowUserReview(uname);
                break;
                
            case 4:
                cout << endl << "[insert new word or link]" << endl;
                cout << "input document title" << endl;
                cin >> in;
                DATA.InsertWL(in);
                break;
                
            case 5:
                cout << endl << "[delete new word or link]" << endl;
                cout << "input document title" << endl;
                cin >> in;
                DATA.DeleteWL(in);
                break;
                
            case 6:
                cout << endl << "[show document detail]" << endl;
                cout << "input document title" << endl;
                cin >> in;
                DATA.ShowDocumentDetail(in);
                break;
                
            case 7:
                system("clear");
                cout << "data update mode exit" << endl << endl;
                return;
                
            default:
                cout << "number is not right" << endl << endl;
                break;
        }
    }
}

void DataRemove(DB &DATA){
    int mode;
    string in1,in2;
    
    system("clear");
    while(1){
        cout << "[remove mode]" << endl;
        cout << "1:delete user" << endl;
        cout << "2:delete friend" << endl;
        cout << "3:delete book" << endl;
        cout << "4:delete document" << endl;
        cout << "5:exit" << endl;
        
        cin >> mode;
        
        system("clear");
        switch(mode){
            case 1:
                cout << "[delete user mode]" << endl;
                cout << "input delete user name" << endl;
                cin >> in1;
                DATA.RemoveUser(in1);
                break;
                
            case 2:
                cout << "[delete friend mode]" << endl;
                cout << "input user name and delete friend name" << endl;
                cin >> in1 >> in2;
                DATA.RemoveFriend(in1, in2);
                break;
                
            case 3:
                cout << "[delete book mode]" << endl;
                cout << "input delete book name" << endl;
                cin >> in1;
                DATA.RemoveBook(in1);
                break;
                
            case 4:
                cout << "[delete document mode]" << endl;
                cout << "input delete document name" << endl;
                cin >> in1;
                DATA.RemoveUser(in1);
                break;
                
            case 5:
                system("clear");
                cout << "data remove mode exit" << endl;
                return;
                
            default:
                cout << "number is not right" << endl;
                break;
        }
    }
}

void DataInsert(DB &DATA){
    int mode;
    string in1,in2;
    
    system("clear");
    while(1){
        cout << "[select mode]" << endl;
        cout << "1:new user" << endl;
        cout << "2:new friend" << endl;
        cout << "3:new book" << endl;
        cout << "4:new document" << endl;
        cout << "5:exit" << endl;
        
        cin >> mode;
        
        system("clear");
        switch(mode){
            case 1:
                cout << "[insert new user mode]" << endl;
                cout << "input new user name" << endl;
                cin >> in1;
                DATA.InsertUser(in1);
                break;
                
            case 2:
                cout << "[set new friend mode]" << endl;
                cout << "input user name and friend name" << endl;
                cin >> in1 >> in2;
                DATA.SetFriend(in1, in2);
                break;
                
            case 3:
                cout << "[insert new book mode]" << endl;
                cout << "input new book name" << endl;
                cin >> in1;
                DATA.InsertBook(in1);
                break;
                
            case 4:
                cout << "[insert new document mode]" << endl;
                cout << "input new document name" << endl;
                cin >> in1;
                DATA.InsertDocument(in1);
                break;
                
            case 5:
                system("clear");
                cout << "data insert mode exit" << endl << endl;
                return;
                
            default:
                cout << "number is not right" << endl << endl;
                break;
        }
    }
}

