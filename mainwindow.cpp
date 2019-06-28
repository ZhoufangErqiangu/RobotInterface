#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FRRJIFInit();
}

MainWindow::~MainWindow()
{
    FRRJIFClose();
    delete ui;
}

void MainWindow::FRRJIFInit()
{
    QVariantList params;

    FRRJIF=new QAxObject(this);
    FRRJIF->setControl("FRRJIF.Core");
    DataTable=new QAxObject(this);
    DataTable=FRRJIF->querySubObject("DataTable");
    DataNumReg=new QAxObject(this);
    params.clear();
    params<<1<<17<<17; //Specify DataType(NUMREG_INT=0 for integer case, NUMREG_REAL=1 for float case) , the first and end index of numeric registers.
    DataNumReg=DataTable->querySubObject("AddNumReg(FRIF_DATA_TYPE, int, int)", params);
    params.clear();
    params<<2<<1<<2<<2; //Specify DataType(POSREG=2) , motion group number, the first and end index of position registers.
    DataPosReg=DataTable->querySubObject("AddPosReg(FRIF_DATA_TYPE, int, int, int)",params);
    FRRJIFConnected=FRRJIF->dynamicCall("Connect(QString)","192.168.0.22").toBool();
}

void MainWindow::FRRJIFClose()
{
    delete DataNumReg;
    delete DataPosReg;
    delete DataTable;
    if (FRRJIFConnected)
    {
        if (FRRJIF->dynamicCall("Disconnect()").toBool())
        {
            ui->label->setText("disconnect ok");
        }
        else
        {
            ui->label->setText("disconnect fail");
        }
    }
    delete FRRJIF;
}

void MainWindow::on_pushButton_clicked()
{
    int i=0;
    QVariantList params;
    int NumRegStartIndex;
    int NumRegEndIndex;
    double NumReg[10];
    QVariant vdata;
    QString sdata;
    bool bpick=false;
    QString sfilepath;
    QDateTime latestdatetime(QDate(1970,1,1),QTime(00,00));
    int latestfileindex=0;
    QString sbasedir="d:/data/";

    NumReg[0]=0;
    NumReg[1]=0;
    NumReg[2]=0;
    NumReg[3]=0;
    NumReg[4]=0;
    NumReg[5]=0;
    NumReg[6]=0;
    NumReg[7]=0;
    NumReg[8]=0;
    NumReg[9]=0;

    QDir dpath;
    dpath.setPath(sbasedir);
    QStringList filter;
    filter.clear();
    filter<<"*.ini";
    dpath.setFilter(QDir::Files|QDir::NoSymLinks);
    dpath.setNameFilters(filter);
    for (i=0;i<static_cast<int>(dpath.count());i++)
    {
        sfilepath=dpath[i];
        QFileInfo finfo(sbasedir+sfilepath);
        QDateTime filetime;
        filetime=finfo.lastModified();
        ui->label_3->setText(finfo.lastModified().toString());
        if (filetime>latestdatetime)
        {
            latestdatetime=finfo.lastModified();
            latestfileindex=i;
        }
    }
    sfilepath=dpath[latestfileindex];
    ui->label_3->setText(sfilepath);

    QSettings *iniRead= new QSettings(sbasedir+sfilepath,QSettings::IniFormat);
    for (i=1;i<10;i++)
    {
        iniRead->beginGroup("Object" + QString::number(i));
        sdata=iniRead->value("Pick").toString();
        if (sdata=="True")
        {
            bpick=true;
            NumReg[0]=iniRead->value("Target X").toDouble();
            NumReg[1]=iniRead->value("Target Y").toDouble();
            NumReg[2]=iniRead->value("Target Z").toDouble();
            NumReg[3]=iniRead->value("Target W").toDouble();
            NumReg[4]=iniRead->value("Target P").toDouble();
            NumReg[5]=iniRead->value("Target R").toDouble();
            NumReg[6]=1.0;
            break;
        }
        else
        {
            bpick=false;
            NumReg[6]=0.0;
        }
        iniRead->endGroup();
    }
    delete iniRead;

    if (bpick)
    {
        if (FRRJIFConnected)
        {
            if (DataTable->dynamicCall("Refresh()").toBool())
            {
                ui->label->setText("data table refresh ok");
            }
            else
            {
                ui->label->setText("data table refresh fail");
            }

            NumRegStartIndex=DataNumReg->property("StartIndex").toInt();
            NumRegEndIndex=DataNumReg->property("EndIndex").toInt();
            //vdata=NumReg[];

            params.clear();
            params<<NumRegStartIndex<<NumReg[6]<<NumRegEndIndex-NumRegStartIndex+1;
            if (DataNumReg->dynamicCall("SetValuesReal(int, double&, int)",params).toBool())
            {
                ui->label_2->setText("set num ok");
            }
            else
            {
                ui->label_2->setText("set num fail");
            }

            params.clear();
            params<<2<<NumReg[0]<<NumReg[1]<<NumReg[2]<<NumReg[3]<<NumReg[4]<<NumReg[5]<<0<<0<<0<<0<<0<<0<<0<<0<<0<<0<<0<<0; //long Index, float X, float Y, float Z, float W, float P, float R, float E1, float E2, float E3, short C1, short C2, short C3, short C4, short C5, short C6, short C7, short UF, short UT
            if (DataPosReg->dynamicCall("SetValueXyzwpr2(int, double, double, double, double, double, double, double, double, double, int, int, int, int, int, int, int, int, int)", params).toBool())
            {
                ui->label_2->setText("set pos reg ok");
            }
            else
            {
                ui->label_2->setText("set pos reg fail");
            }

        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    QString sDoc;
    QFile *fDoc;

    fDoc=new QFile("d:/doc2.html");
    fDoc->open(QIODevice::WriteOnly|QIODevice::Append);
    QTextStream tsDoc(fDoc);

    sDoc=DataPosReg->generateDocumentation();
    ui->textBrowser->setHtml(sDoc);
    tsDoc<<sDoc<<endl;

    tsDoc.flush();
    fDoc->close();
}
