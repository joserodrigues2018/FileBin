#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QList>

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cstring>

using namespace std;

typedef struct {
  string conditions;
  string name;
  int tam = 0;
  string descr;
}CamposXml;


//Prototipo das Funções

string TratarTexto(string text);
QString LerFileBin(QString nomearq);
int LerFileXml(char* nomefile, string * vtLinhasXml);
void SplitString(string strFull, string *vetor_split);
void TratarEstrutura(string *vtLinhasXml,CamposXml *listaCampos,string *vtCondicao, int *qtReg);
void AplicarRegra( CamposXml *listaCampos,string *vtCondicao, int qtReg,QString binario,string *vtResult);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionClose_triggered()
{
    close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString filebin = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "C:\\",
                "All Files (*.bf)");

    QMessageBox::information(this,tr("File Open"),filebin);

    ui->lblFileName->setText(filebin);

    QString retBin = LerFileBin(filebin);

    char filexml []="C:\\Qt\\xml_test.xml";

    string vtLinhasXml[20];

    LerFileXml(filexml,vtLinhasXml);

    CamposXml listaCampos[20];
    string vtCondicao[1];
    int qtReg[1];
    string vtResult[3];

    TratarEstrutura(vtLinhasXml,listaCampos,vtCondicao,qtReg);

    AplicarRegra(listaCampos,vtCondicao,qtReg[0],retBin,vtResult);

   qDebug() << "field_flag    : " << vtResult[0].c_str();
   qDebug() << "optional_flag : " << vtResult[1].c_str();
   qDebug() << "last_field    : " << vtResult[2].c_str();

   ui -> txtFieldFlag     ->setText(vtResult[0].c_str());

   if(vtResult[1] == "")
   {
       ui -> txtOptionalField ->setText(vtResult[1].c_str());
       ui -> txtOptionalField -> setEnabled(false);
   }
   else{
       ui -> txtOptionalField -> setEnabled(true);
       ui -> txtOptionalField ->setText(vtResult[1].c_str());
   }

   ui -> txtLastField     -> setText(vtResult[2].c_str());
}

void AplicarRegra( CamposXml *listaCampos,string *vtCondicao, int qtRegLista,QString binario, string *vtResult)
{
    string field_flag;
    string optional_field;
    string last_field;
    int tam_comp = 0;

    for( int ct=0; ct<qtRegLista; ct++)
    {
        if(vtCondicao[0] == "$campo1=1")
        {
            if(listaCampos[ct].name=="campo1")
            {
                //field_flag
                qDebug() << ct  << listaCampos[ct].name.c_str();
                qDebug() << ct  << listaCampos[ct].tam;
                qDebug() << ct  << listaCampos[ct].descr.c_str();
                field_flag=binario.mid(0,listaCampos[ct].tam).toStdString();
                qDebug() << listaCampos[ct].descr.c_str() << " : " << field_flag.c_str();

                tam_comp = listaCampos[ct].tam;

                vtResult[0] = field_flag;
            }
            else if(listaCampos[ct].name=="campo2")
            {
                //optional_field pelo field_flag
                // ja peguei do campo anterior

                if(atoi(field_flag.c_str()) == 1) //field_flag =1
                {
                    qDebug() << ct  << listaCampos[ct].name.c_str();
                    qDebug() << ct  << listaCampos[ct].tam;
                    qDebug() << ct  << listaCampos[ct].descr.c_str();
                    optional_field=binario.mid(1,listaCampos[ct].tam).toStdString();
                    qDebug() << listaCampos[ct].descr.c_str() << ":" << optional_field.c_str();
                    vtResult[1] = optional_field;
                }
                else if (atoi(field_flag.c_str()) == 0)
                {
                    qDebug() << ct  << listaCampos[ct].name.c_str();
                    qDebug() << ct  << listaCampos[ct].tam;
                    qDebug() << ct  << listaCampos[ct].descr.c_str();
                    optional_field=binario.mid(1,listaCampos[ct].tam).toStdString();
                    //qDebug() << listaCampos[ct].descr.c_str() << ":" << optional_field.c_str();
                    tam_comp = tam_comp+listaCampos[ct].tam;
                    vtResult[1] = "";
                }
                else{

                }
            }
            else if(listaCampos[ct].name=="campo3")
            {
                // last_field ==>> campo3

                qDebug() << ct  << listaCampos[ct].name.c_str();
                qDebug() << ct  << listaCampos[ct].tam;
                qDebug() << ct  << listaCampos[ct].descr.c_str();
                last_field=binario.mid(optional_field.length(),listaCampos[ct].tam).toStdString();
                qDebug() <<listaCampos[ct].descr.c_str()<<" : " <<last_field.c_str();
                vtResult[2] = last_field;
            }
            else{

            }
        }
        else // Condição ==>> $campo1=0
        {
            if(listaCampos[ct].name=="campo1")
            {
                //field_flag
                qDebug() << ct  << listaCampos[ct].name.c_str();
                qDebug() << ct  << listaCampos[ct].tam;
                qDebug() << ct  << listaCampos[ct].descr.c_str();
                field_flag=binario.mid(0,listaCampos[ct].tam).toStdString();
                qDebug() << listaCampos[ct].descr.c_str() << " : " << field_flag.c_str();

                tam_comp = listaCampos[ct].tam;
                vtResult[0] = field_flag;
            }
            else if(listaCampos[ct].name=="campo2")
            {
                //optional_field pelo field_flag
                // ja peguei do campo anterior

                if(atoi(field_flag.c_str()) == 1) //field_flag =1
                {
                    qDebug() << ct  << listaCampos[ct].name.c_str();
                    qDebug() << ct  << listaCampos[ct].tam;
                    qDebug() << ct  << listaCampos[ct].descr.c_str();
                    optional_field=binario.mid(1,listaCampos[ct].tam).toStdString();
                    //qDebug() << listaCampos[ct].descr.c_str() << ":" << optional_field.c_str();
                    vtResult[1] = "";

                }
                else if (atoi(field_flag.c_str()) == 0)
                {
                    qDebug() << ct  << listaCampos[ct].name.c_str();
                    qDebug() << ct  << listaCampos[ct].tam;
                    qDebug() << ct  << listaCampos[ct].descr.c_str();
                    optional_field=binario.mid(1,listaCampos[ct].tam).toStdString();
                    qDebug() << listaCampos[ct].descr.c_str() << ":" << optional_field.c_str();
                    tam_comp = tam_comp+listaCampos[ct].tam;
                    vtResult[1] = optional_field;
                }
                else{

                }
            }
            else if(listaCampos[ct].name=="campo3")
            {
                // last_field ==>> campo3

                qDebug() << ct  << listaCampos[ct].name.c_str();
                qDebug() << ct  << listaCampos[ct].tam;
                qDebug() << ct  << listaCampos[ct].descr.c_str();
                last_field=binario.mid(tam_comp,listaCampos[ct].tam).toStdString();
                qDebug() <<listaCampos[ct].descr.c_str()<<" : " <<last_field.c_str();
                vtResult[2] = last_field;
            }
            else{

            }
        }
    }
}

string TratarTexto(string text)
{
  string::iterator it;
  string only = "abcdefghijklmnopqrstuvxwyz_=$0123456789";
  size_t found = text.find_first_not_of(only);

  while (found!=string::npos)
  {
      it = text.begin()+found;

      text.erase (it);

     found = text.find_first_not_of(only);
  }

  return (text);
}
void SplitString(string strFull, string *ret_split)
{
    string parte_string;
    string str_parte;

    int ind_posicao=0;
    int ind_ultposicao=0;
    int ind_split = 0;
    int tam_str= 0;

    strFull = TratarTexto(strFull);

    size_t tam = strlen(strFull.c_str());

    while ((int)tam>ind_posicao){

        str_parte=strFull.substr(ind_posicao,1);

        ind_posicao++;

        if(str_parte == "="){

                if(ind_split == 0){

                    tam_str = strlen(strFull.substr(ind_split,ind_posicao-1).c_str());
                    ret_split[ind_split] = parte_string=strFull.substr(ind_split,tam_str);
                    ind_split++;
                }
                else{
                    tam_str = strlen(strFull.substr(ind_ultposicao,((ind_posicao-ind_ultposicao)-1)).c_str());
                    ret_split[ind_split] = parte_string=strFull.substr(ind_ultposicao,tam_str);
                    ind_split++;
                }

                ind_ultposicao = ind_posicao;
        }
        else if((int)tam == ind_posicao){
                tam_str = strlen(strFull.substr(ind_ultposicao,((ind_posicao-ind_ultposicao))).c_str());
                ret_split[ind_split] = parte_string=strFull.substr(ind_ultposicao,tam_str);
        }
        else{
        }
    }
}
void TratarEstrutura(string *vtLinhasXml,CamposXml *listaCampos,string *vtCondicao, int *qtReg)
{
    qDebug() <<"\nTratar Estrutura do Arquivo XML...";


    string leg_name = "name";
    string leg_size = "size";
    string leg_desc = "description";
    string leg_cond = "conditions";

    string comp1 = "<packets>";
    string comp2 = "</packets>";

    int contl =0;

    for ( int i=0; i<20; i++)
    {
       qDebug()<< vtLinhasXml[i].c_str() <<" - "<< strlen(vtLinhasXml[i].c_str());

       string ret_split[3];

       if(vtLinhasXml[i] != comp1 &&
              vtLinhasXml[i] != comp2 &&
              !(strlen(vtLinhasXml[i].c_str())<7))
       {
            qDebug()<< vtLinhasXml[i].c_str() <<" OK ";

            if(vtLinhasXml[i].substr(0,leg_cond.length())==leg_cond)
            {
                listaCampos[contl].name = leg_cond;
                listaCampos[contl].tam = 0;
                listaCampos[contl].descr = leg_cond;
                listaCampos[contl].conditions =
                        vtLinhasXml[i].substr(leg_cond.length()+2,9);
                contl++;

            }
            else
            {
                SplitString(vtLinhasXml[i],ret_split);

                if(ret_split[0] == leg_name)
                {
                    listaCampos[contl].name = ret_split[1];
                }
                else if(ret_split[0] == leg_size)
                {
                    listaCampos[contl].tam = atoi(ret_split[1].c_str());
                }
                else if(ret_split[0] == leg_desc)
                {
                 //   listaCampos[contl].name = ret_split[1];
                  //  listaCampos[contl].tam = atoi(ret_split[2].c_str());

                    //qDebug()<< "name        >>" <<listaCampos[contl].name.c_str();
                    //qDebug()<< "size        >>" <<listaCampos[contl].tam;

                    //contl++;
                    listaCampos[contl].descr = ret_split[1];

                    qDebug()<< "name        >>" <<listaCampos[contl].name.c_str();
                    qDebug()<< "size        >>" <<listaCampos[contl].tam;
                    qDebug()<< "description >>" <<listaCampos[contl].descr.c_str();

                    contl++;
                }
                else
                {

                }
            }
       }
    }

    // Lista todos os campos da estrutura do XML
    //
    for(int t=0; t<contl;t++)
    {
       if(listaCampos[t].conditions == "")
       {
            qDebug() << t  << listaCampos[t].name.c_str();
            qDebug() << t  << listaCampos[t].tam;
            qDebug() << t  << listaCampos[t].descr.c_str();
       }
       else
       {
           qDebug() << t  << listaCampos[t].name.c_str();
           qDebug() << t  << listaCampos[t].tam;
           qDebug() << t  << listaCampos[t].descr.c_str();
           qDebug() << t  <<listaCampos[t].conditions.c_str();

           vtCondicao[0] = listaCampos[t].conditions.c_str();
       }
    }
    qtReg[0] =  contl;
}

int LerFileXml(char* filename,string *vtlinhasXml)
{
    int ct = 0;
    ifstream arq;

    qDebug() << "Abrindo arquivo...";

    arq.open(filename,ios::in);

    if (!arq.is_open())
        return (0);

    string cabecalho;
    getline(arq,cabecalho);

    qDebug() << "linha==>>" << cabecalho.c_str();

    do
    {
        string   recvalor;
        string linha_texto;
        arq >> linha_texto;

        recvalor=linha_texto;
        vtlinhasXml[ct]=recvalor;

        ct++;

        if(!arq.fail()) {
            qDebug() << linha_texto.c_str();
        }

    } while(arq.good());

        if(arq.bad() || !arq.eof()) {
        qDebug() << "Erro fatal!";
        exit(1);
    }

    qDebug() << "Fechando o arquivo...";

    arq.close();

    return 0;
}

QString LerFileBin(QString nomearq)
{
    QFile binFile(nomearq);
    QString bin_valor;

    qDebug() << "Abrindo arquivo binario : "<< nomearq;

    if(!binFile.open(QFile::ReadOnly)){
            qDebug() << "Erro fatal!";
            bin_valor = "";
            return bin_valor;
    }

    QTextStream in(&binFile);
    bin_valor = in.readLine();

    qDebug() << bin_valor;
    qDebug() << "Fechando o arquivo binario...";

    binFile.close();

    return bin_valor;
}
