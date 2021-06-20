#include "RecordManager.h"

vector<Tuple> RecordManager :: SelectWithIndex( Table &t ,File * file , string Index_name , Where where){
    vector <Tuple> ret;
    vector <Search_Info> Fetch = im.Search_In_Index( Index_name, where );
    if( Fetch.size() == 0 ) return ret;
    int size = t.GetLength();
    for( int i = 0 ; i < Fetch.size() ; i ++ ){
        Tuple tp;
        Block * tmp = bm.GetBlockByNum(file , Fetch[i].Block_Offset);
        char * data = tmp->GetContent();
        data += Fetch[i].Offset_in_Block;
        if( *data == 0 ){
            for( int i = 0; i < t.attr_.num ; i ++  ){
                tp.addData( ConverseIntoData( data ,t.attr_.type[i] ) );
                data += ( t.attr_.type[i] < 1 ) ?  4 : t.attr_.type[i];
            }
            ret.push_back(tp);
        }
    }
    return ret;
}
Data ConverseIntoData( char * data, int type ){ 
    Data ret;
    ret.type = type;
    if( type == -1 ){
        ret.datai = *((int*) data);
    }else if ( type == 0 ){
        ret.dataf = *((float*)data);
    }else{
        string tmp(data, type);
        ret.datas = tmp;
    }
    return ret;
}
bool  RecordManager :: RecordConditionFit(Tuple &t , vector < int > &AttributeName ,vector<Where> &where){
    vector < Data > D = t.getData();
    bool ret = true;
    for(int i = 0 ; i < where.size() ; i ++ ){
        bool res;
        switch (where[i].relation_character){
            case LESS:
                if( where[i].data.type == -1 )
                    res = D[AttributeName[i]].datai < where[i].data.datai;
                else if ( where[i].data.type == 0 )
                    res = D[AttributeName[i]].dataf < where[i].data.dataf;
                else res = D[AttributeName[i]].datas < where[i].data.datas;
            break;
            case LESS_OR_EQUAL:
                if( where[i].data.type == -1 )
                    res = D[AttributeName[i]].datai <= where[i].data.datai;
                else if ( where[i].data.type == 0 )
                    res = D[AttributeName[i]].dataf <= where[i].data.dataf;
                else res = D[AttributeName[i]].datas <= where[i].data.datas;
            break;
            case EQUAL:
                if( where[i].data.type == -1 )
                    res = D[AttributeName[i]].datai == where[i].data.datai;
                else if ( where[i].data.type == 0 )
                    res = D[AttributeName[i]].dataf == where[i].data.dataf;
                else res = D[AttributeName[i]].datas == where[i].data.datas;
            break;
            case GREATER_OR_EQUAL:
                if( where[i].data.type == -1 )
                    res = D[AttributeName[i]].datai >= where[i].data.datai;
                else if ( where[i].data.type == 0 )
                    res = D[AttributeName[i]].dataf >= where[i].data.dataf;
                else res = D[AttributeName[i]].datas >= where[i].data.datas;
            break;
            case GREATER:
                if( where[i].data.type == -1 )
                    res = D[AttributeName[i]].datai > where[i].data.datai;
                else if ( where[i].data.type == 0 )
                    res = D[AttributeName[i]].dataf > where[i].data.dataf;
                else res = D[AttributeName[i]].datas > where[i].data.datas;
            break;
            case NOT_EQUAL:
                if( where[i].data.type == -1 )
                    res = D[AttributeName[i]].datai != where[i].data.datai;
                else if ( where[i].data.type == 0 )
                    res = D[AttributeName[i]].dataf != where[i].data.dataf;
                else res = D[AttributeName[i]].datas != where[i].data.datas;
            break;
        }
        ret = ret & res;
    } 
    return ret;
}
vector<Tuple> RecordManager :: SelectRecord( Table &t, vector < string > &AttributeName ,vector<Where> &where ){
    vector <Tuple> ret;
    Attribute attr = t.attr_;
    vector < int > AttrIndex = t.ConvertIntoIndex( AttributeName );
    File * file = bm.GetFile( t.getTitle(), 0 );
    bool do_index;
    for( int i = 0;i < AttributeName.size() ; i ++ ){
        int index = t.AttrName2Index[AttributeName[i]];
        if( attr.has_index[index] ){
            ret = SelectWithIndex( t , file ,attr.index_name[index], where[i] );
            AttributeName.erase(AttributeName.begin()+i);
            where.erase(where.begin()+i);
            break;
        }
    }
    for( int i = 0 ; i < ret.size() ; i ++ ){
        if( !RecordConditionFit( ret[i] , AttrIndex , where ) ){
            ret.erase(ret.begin() + i);
        }
    }
    return ret;
}
Tuple ConverseIntoTuple( char * data , Attribute * attr ){
    Tuple ret;
    for(int i = 0 ; i < attr->num ; i ++ ){
        ret.addData( ConverseIntoData( data , attr->type[i] ) );
    }
    return ret;
}
string Data2String( Data d ){
    if ( d.type < 0){
        string ret( (char*) (&d.datai) , 4);
        return ret;
    }else if( d.type == 0 ){
        string ret( (char*) (&d.dataf) , 4);
        ret;
    }else {
        string ret = d.datas;
        ret.resize( d.type );
        return ret;
    }
}
int RecordManager :: DeleteRecord(  Table & t , vector<string> target_attr, vector<Where> where ){
    string Index_name ;
    vector <int > AttrIndex = t.ConvertIntoIndex( target_attr );
    int SearchIndex = -1;
    for(int i = 0 ; i < AttrIndex.size() ; i ++ ){
        if ( t.attr_.has_index[AttrIndex[i]] ) {
            Index_name = t.attr_.index_name[AttrIndex[i]];
            SearchIndex = i;
            break;
        }
    }
    File * file = bm.GetFile( t.getTitle() , 0 ); 
    int cnt = 0;
    if( SearchIndex != -1 ){
        vector <Search_Info> Fetch = im.Search_In_Index( Index_name, where[SearchIndex] );
        if ( Fetch.size() ){
            for( int i = 0 ;i < Fetch.size() ; i++ ){
                Block * tmp = bm.GetBlockByNum( file , Fetch[i].Block_Offset );
                Tuple tempTuple = ConverseIntoTuple(tmp->GetContent() + 1, &t.attr_ );
                if ( RecordConditionFit( tempTuple , AttrIndex , where ) ){
                    for( int i = 0 ; i < t.attr_.num; i ++ ){
                        if( t.attr_.has_index[i] ){
                            string tmp = Data2String( tempTuple.getData()[i] );
                            im.Delete_From_Index( t.attr_.index_name[i], tmp );
                        }
                    }
                    char DeletedFlag = 1;
                    tmp->write( Fetch[i].Offset_in_Block , &DeletedFlag , 1 );
                    RecordFreeList node = new FreeListNode;
                    file->AppendFreeList( Fetch[i].Block_Offset , Fetch[i].Offset_in_Block );
                    cnt ++ ;
                }
            }
        }
    }else{
        for( Block * tmp = bm.GetBlockHead(file) ; !tmp->IsEnd() ; tmp = bm.GetNextBlock(file , tmp ) ){
            for(int i = 0 ; i + t.GetLength() < BLOCK_SIZE ; i += t.GetLength()+1 ){
                Tuple tempTuple = ConverseIntoTuple( tmp->GetContent() + 1 , & t.attr_ );
                if ( RecordConditionFit( tempTuple , AttrIndex , where ) ){
                    char DeletedFlag = 1;
                    tmp->write( i , &DeletedFlag , 1 );
                    RecordFreeList node = new FreeListNode;
                    file->AppendFreeList( tmp->GetBlockOffsetNum() , i );
                    cnt ++ ;
                }
            }
        } 
    }
    return cnt;
}

void RecordManager :: DropTable( string TableName){
    File * file = bm.GetFile(TableName , 0);
    bm.ClearTable( file );
}
bool RecordManager :: CheckUnique( Table & t , Tuple & tuple ){
    bool ret = true;
    for( int i = 0 ; i < t.attr_.num ; i ++ ){
        if( t.attr_.unique[i] || i == t.attr_.primary_key ){
            map<string , unordered_set<string >> :: iterator SUIT = t.Unique.find(t.attr_.name[i]);
            if( SUIT != t.Unique.end() ){
                unordered_set < string > & ValueSet = SUIT->second ;
                string data = Data2String( tuple.getData()[i] );
                unordered_set < string >  ::iterator SETITOR = ValueSet.find( data );
                if( SETITOR == ValueSet.end() ){
                    ValueSet.insert( data );
                }else {
                    ret = false;
                }
            }else{
                unordered_set < string > ValueSet;
                ValueSet.insert( Data2String(tuple.getData()[i]) );
                t.Unique.insert(make_pair(t.attr_.name[i] ,ValueSet ));
            }
        }
    }
    return ret;
}
int RecordManager :: InsertRecord( Table &t , Tuple& tuple ){
    File * file = bm.GetFile( t.getTitle(),0 );
    string data = '\0';
    for( int i = 0 ; i < tuple.getData().size(); i++ ){
        data += Data2String(tuple.getData()[i]);
    }
    int OffsetNum , offset ;
    if( file->freelist == NULL ){
        for( Block * tmp = bm.GetBlockHead(file ); !tmp->IsEnd() ; tmp = bm.GetNextBlock(file , tmp ) ){
            if( tmp->GetUsingSize() + t.GetLength() + 1 > BLOCK_SIZE && tmp->GetUsingSize() != BLOCK_SIZE ){
                tmp->SetUsingSize(BLOCK_SIZE);
            }else{
                offset = tmp->GetUsingSize();
                OffsetNum = tmp->GetBlockOffsetNum();
                tmp->write( offset , data.c_str() , data.size() );
                tmp->SetUsingSize( offset + data.size() ); 
            }
        }
    }else{
        RecordFreeList now = file->freelist;
        file->freelist = file->freelist->next;
        now->next = NULL ;
        OffsetNum = now->BlockNum;
        offset = now->offset;
        delete now;
        Block * tmp = bm.GetBlockByNum(file , OffsetNum);
        tmp->write( offset , data.c_str() , data.size() ); 
    }
    for( int i = 0 ; i < t.attr_.num; i ++ ){
        if( t.attr_.has_index[i] ){
            string tmp = Data2String( tuple.getData()[i] );
            string KeyType;
            if( t.attr_.type[i] == -1) KeyType = "int";
            else if ( t.attr_.type[i] == 0 ) KeyType = "float";
            else KeyType = "string"; 
            im.Insert_Into_Index( t.attr_.index_name[i] ,tmp,  KeyType , OffsetNum , offset );
        }
    }
}
void RecordManager :: CreateIndex( Table &t , string AttrName , string index_name  ){
    int index = t.AttrName2Index[AttrName];
    int KeySize = t.attr_.type[index] < 1 ? 4 : t.attr_.type[index];
    string KeyType;
    if( t.attr_.type[index] == -1) KeyType = "int";
    else if ( t.attr_.type[index] == 0 ) KeyType = "float";
    else KeyType = "string"; 
    im.Create_Index( index_name ,KeySize , KeyType);
    File * file = bm.GetFile( t.getTitle() , 0);
    int offset = 1;
    for(int i = 0 ; i < index ; i ++ ){
        offset += t.attr_.type[i] < 1 ? 4 : t.attr_.type[i];
    } 
    int KeySize = t.attr_.type[index] < 1 ? 4 : t.attr_.type[index];
    int RecordSize = t.GetLength();
    for( Block * tmp = bm.GetBlockHead(file) ;tmp ; tmp = bm.GetNextBlock( file , tmp)){
        char* data = tmp->GetContent();
        for(int i = 0 ; i + RecordSize < BLOCK_SIZE ; i += RecordSize + 1){
            if( *(data+i) == 1 ) continue;
            string KeyValue( data + i + 1 , KeySize);
            im.Insert_Into_Index( index_name , KeyValue , KeyType , tmp->GetBlockOffsetNum() , i);
        }
    }
}
ostream & operator << ( ostream & out , Data tmp ){
    if( tmp.type == -1 ) out << tmp.datai;
    else if( tmp.type == 0 ) out << tmp.dataf;
    else out << tmp.datas;
    return out;
}
void RecordManager :: ShowTuple( vector <Tuple> tuples , Table & t){
    for(int i = 0 ; i < t.attr_.num ; i ++ ){
        int length = t.attr_.type[i] < 1 ? 10 : t.attr_.type[i] ;
        cout << "|" << *right << setw(length) << t.attr_.name[i];
    }
    cout << "|" << endl;
    for(int i = 0; i < tuples.size() ; i++ ){
        for(int j = 0 ; j < tuples[i].getData().size() ; j ++ ){
            int length = t.attr_.type[i] < 1 ? 10 : t.attr_.type[i];
            cout << "|" << *right << setw(length) << tuples[i].getData()[j]; 
        }
        cout << "|" << endl;
    }
}
