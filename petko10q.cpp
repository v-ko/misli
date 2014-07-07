/*  This file is part of Misli.

    Misli is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Misli is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Misli.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>

#include "petko10q.h"

char * q_get_text_between(char *source_string, char A, char B, int length=-1)
{
    QString txt=QString::fromUtf8(source_string);
    if(length!=-1){txt.truncate(length);} //only in the specified range
    int A_pos=-2,B_pos;

    //find first use of A - indexOf
    if(A!=0){A_pos=txt.indexOf(QChar(A));}
    //trim - remove
    if( (A_pos!=-1)&&(A_pos!=-2) ){txt.remove(0,A_pos+1);} //remove preceding string and the delimiting char
    if (A_pos==-1)return NULL;//if not found (if ==-2 => A==NULL =>dont trim)

    //find first use of B indexOf
    if(B!=0){B_pos=txt.indexOf(QChar(B));}
    else { return strdup(txt.toUtf8().data()); }
    //trim - truncate
    if(B_pos!=-1){txt.truncate(B_pos);}
    else return NULL; //if not found

    return strdup(txt.toUtf8().data());
}

char * q_get_text_between(const char *source_string, char A, char B, int length=-1)
{
    QString txt=QString::fromUtf8(source_string);
    if(length!=-1){txt.truncate(length);} //only in the specified range
    int A_pos=-2,B_pos;

    //find first use of A - indexOf
    if(A!=0){A_pos=txt.indexOf(QChar(A));}
    //trim - remove
    if( (A_pos!=-1)&&(A_pos!=-2) ){txt.remove(0,A_pos+1);} //remove preceding string and the delimiting char
    if (A_pos==-1)return NULL;//if not found (if ==-2 => A==NULL =>dont trim)

    //find first use of B indexOf
    if(B!=0){B_pos=txt.indexOf(QChar(B));}
    else { return strdup(txt.toUtf8().data()); }
    //trim - truncate
    if(B_pos!=-1){txt.truncate(B_pos);}
    else return NULL; //if not found

    return strdup(txt.toUtf8().data());
}

QString q_get_text_between(QString txt, char A, char B, int length)
{
    if(length!=-1){txt.truncate(length);} //only in the specified range
    int A_pos=-2,B_pos;

    //find first use of A - indexOf
    if(A!=0){A_pos=txt.indexOf(QChar(A));}
    //trim - remove
    if( (A_pos!=-1)&&(A_pos!=-2) ){txt.remove(0,A_pos+1);} //remove preceding string and the delimiting char
    if (A_pos==-1)return NULL;//if not found (if ==-2 => A==NULL =>dont trim)

    //find first use of B indexOf
    if(B!=0){B_pos=txt.indexOf(QChar(B));}
    else { return txt; }
    //trim - truncate
    if(B_pos!=-1){txt.truncate(B_pos);}
    else return QString(); //if not found

    return txt;
}

int q_get_value_for_key(QString string, QString key, QString& result) //string should be a list of INI type key-value pairs
{
    QStringList line;

    key+="="; //to fit the startsWith test more acurately

    line = string.split("\n",QString::SkipEmptyParts);

    for(int i=0;i<line.size();i++){
        if(line[i].startsWith(key)){

            result=q_get_text_between(line[i],'=',0);

            return 0;
        }
    }

    qDebug()<<"[q_get_value_for_key]Key not found: "<<key;
    return -1;
}

int q_get_value_for_key(QString string, QString key, float& result)
{
    QString txt;
    int err = q_get_value_for_key(string,key,txt);
    if (err<0) return err;
    result = txt.toFloat();
    return 0;
}

int q_get_value_for_key(QString string, QString key, int& result)
{
    QString txt;
    int err = q_get_value_for_key(string,key,txt);
    if (err<0) return err;
    result = txt.toInt();
    return 0;
}

int q_get_value_for_key(QString string, QString key, unsigned int& result)
{
    QString txt;
    int err = q_get_value_for_key(string,key,txt);
    if (err<0) return err;
    result = txt.toUInt();
    return 0;
}

int q_get_value_for_key(QString string, QString key, bool& result)
{
    QString txt;
    int err = q_get_value_for_key(string,key,txt);
    if (err<0) return err;
    int res = txt.toInt();
    if( (res==1) | (res==0) ){result = res;}
    else return -1;
    return 0;
}

int q_get_value_for_key(QString string, QString key, QStringList& result)
{
    QString txt;
    int err = q_get_value_for_key(string,key,txt);
    if (err<0) return err;
    result = txt.split(";",QString::SkipEmptyParts);
    return 0;
}

int q_get_groups(QString string, QStringList &names, QStringList &groups)
{
    QStringList line=string.split("\n",QString::SkipEmptyParts);
    QString group; //buffer
    names.clear();
    groups.clear();

    for(int i=0;i<line.size();i++){
        if(line[i].startsWith("[")){
            if(names.size()!=0){ //if there's a group started - complete it
                if(group.size()!=0) groups.push_back(group);
                group.clear();
            }
            names.push_back(q_get_text_between(line[i],'[',']'));//and start the new group
        }else {
            group+=line[i];
            group+="\n";
        }
    }
    if((group.size()!=0) && names.size()!=0) groups.push_back(group); //complete the last group if there's some text in the buffer and there's a group started

    if(groups.size()!=names.size()){
        return -1;
    }else {
        return 1;
    }
}