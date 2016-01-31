#include "msgshowdlg.h"
#include "ui_msgshowdlg.h"
#include "cxmlmodel.h"
#include "../util.h"
#include <QDir>
#include <QFileDialog>

MsgShowDlg::MsgShowDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MsgShowDlg)
{
    ui->setupUi(this);
    m_pXmlModel1 = NULL;
    m_pXmlModel2 = NULL;

}

MsgShowDlg::~MsgShowDlg()
{
    if(m_pXmlModel1)
      delete m_pXmlModel1;
    if(m_pXmlModel2)
      delete m_pXmlModel2;
    delete ui;
}

void MsgShowDlg::SetMode(bool nShowOnly)
{
    m_bShowOnly = nShowOnly;
    if(m_bShowOnly)
    {
        ui->btnOpenImage->setVisible(false);
        ui->btnOpenImage_2->setVisible(false);
        ui->btnDeleteImage->setVisible(false);
        ui->btnDeleteImage2->setVisible(false);
        ui->messageTitle->setReadOnly(true);
        ui->comboBox->setVisible(false);
        ui->labelMsgType->setVisible(false);
    }else
    {
        LoadXmlModelType();
        ui->comboBox->setCurrentIndex(0);
    }
}

void MsgShowDlg::on_rbtnPublicXml_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MsgShowDlg::on_rbtnEncryptXml_clicked()
{
    if(m_bShowOnly)
    {
        if(!m_pXmlModel2)
        {
            if(m_MessageBlock.CheckPassword(ui->password->text().toStdString()))
            {
                ui->labelError->setVisible(false);
                std::string strXml;
                m_MessageBlock.DecodeXml(strXml);
                m_pXmlModel2 = new CXmlModel(ui->scrollArea2);
                if(!m_pXmlModel2->LoadXmlModel(QString::fromStdString(strXml)))
                {
                    delete m_pXmlModel2;
                    m_pXmlModel2 = NULL;
                    MessageBox(NULL,strXml.c_str(),"err",MB_OK);
                }
            }else
            {
                ui->labelError->setVisible(true);
                MessageBox(NULL,"password incorrent","error",MB_OK);

                delete m_pXmlModel2;
                m_pXmlModel2 = NULL;
            }
        }
    }else
    {
        ui->labelError->setVisible(false);
    }
    ui->stackedWidget->setCurrentIndex(2);
}

void MsgShowDlg::on_rbtnPublicImage_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MsgShowDlg::on_rbtnEncryptImage_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MsgShowDlg::on_comboBox_currentIndexChanged(int index)
{
    bool bShowXml = !(index==0);
    ui->rbtnPublicXml->setEnabled(bShowXml);
    ui->rbtnEncryptXml->setEnabled(bShowXml);
    ui->rbtnPublicImage->setEnabled(bShowXml);
    ui->rbtnEncryptImage->setEnabled(bShowXml);
    if(bShowXml)
    {
        QString strFileName = "./model/"+ui->comboBox->itemText(index)+".A.xml";

        if(m_pXmlModel1)
        {
            delete m_pXmlModel1;
            m_pXmlModel1 = NULL;
        }

        m_pXmlModel1 = new CXmlModel(ui->scrollArea);
        if(!m_pXmlModel1->LoadXmlModelFromFile(strFileName))
             MessageBox(NULL,"xml 1 parse error","err",MB_OK);
        ui->scrollArea->repaint();
        strFileName = "./model/"+ui->comboBox->itemText(index)+".B.xml";

        if(m_pXmlModel2)
        {
            delete m_pXmlModel2;
            m_pXmlModel2 = NULL;
        }
        m_pXmlModel2 = new CXmlModel(ui->scrollArea2);
        if(!m_pXmlModel2->LoadXmlModelFromFile(strFileName))
        {
            MessageBox(NULL,"xml 2 parse error","err",MB_OK);
        }

        ui->rbtnPublicXml->setChecked(true);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
        ui->stackedWidget->setCurrentIndex(0);

}

void MsgShowDlg::LoadXmlModelType()
{
    QDir dmodel("./model/");
    QFileInfoList finfolist = dmodel.entryInfoList(QDir::Files);
    foreach(QFileInfo finfo,finfolist)
    {
        if(finfo.completeSuffix()!="A.xml")
            continue;
        ui->comboBox->addItem(finfo.baseName());
    }
    ui->comboBox->setCurrentIndex(0);
    on_comboBox_currentIndexChanged(0);
}


void MsgShowDlg::on_listWidget_currentTextChanged(const QString &currentText)
{
    if(!m_bShowOnly)
    {
        if(currentText.isEmpty())
        {
            ui->ImageView->clear();
            return;
        }
        QImage myImage;
        if(myImage.load(currentText))
        {
            ui->ImageView->setPixmap(QPixmap::fromImage(myImage));
        }
    }else
    {
        int nIndex = ui->listWidget->currentRow();
        if(nIndex>=0)
        {
            QImage myImage;
            CImageFile *pImgFile = &m_MessageBlock.ImageA[nIndex];
            if(myImage.loadFromData((unsigned char *)pImgFile->ImageData.data(),pImgFile->ImageData.size()))
            {
                ui->ImageView->setPixmap(QPixmap::fromImage(myImage));
            }
        }

    }
}

void MsgShowDlg::on_listWidget2_currentTextChanged(const QString &currentText)
{
    if(!m_bShowOnly)
    {
        if(currentText.isEmpty())
        {
            ui->ImageView2->clear();
            return;
        }
        QImage myImage;
        if(myImage.load(currentText))
        {
            ui->ImageView2->setPixmap(QPixmap::fromImage(myImage));
        }
    }else
    {

        int nIndex = ui->listWidget2->currentRow();
        if(nIndex>=0)
        {
            if(m_MessageBlock.CheckPassword(ui->password->text().toStdString()))
            {
                QImage myImage;
                std::vector<char> data;
                m_MessageBlock.DecodeImage(nIndex,data);
                if(myImage.loadFromData((const unsigned char *)data.data(),data.size()))
                {
                    ui->ImageView2->clear();
                    ui->ImageView2->setPixmap(QPixmap::fromImage(myImage));
                }else
                {
                    ui->ImageView2->clear();
                    ui->ImageView2->setText(tr("密码不正确，或未输入密码！"));
                }
            }else
            {
                ui->ImageView2->clear();
                ui->ImageView2->setText(tr("密码不正确，或未输入密码！"));
            }
        }
    }
}

void MsgShowDlg::on_btnOpenImage_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Images (*.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        foreach (QString strfile, fileNames) {
            if(ui->listWidget->count()>4)
                break;
            ui->listWidget->addItem(strfile);
        }
        if(ui->listWidget->count()>0)
            ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
}

void MsgShowDlg::on_btnDeleteImage_clicked()
{
    int nRow = ui->listWidget->currentRow();
    if(nRow>=0)
    {
        QListWidgetItem *pItem = ui->listWidget->takeItem(nRow);
        delete pItem;

    }
}

void MsgShowDlg::on_btnOpenImage_2_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Images (*.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        foreach (QString strfile, fileNames) {
            if(ui->listWidget2->count()>4)
                break;
            ui->listWidget2->addItem(strfile);
        }
        if(ui->listWidget2->count()>0)
            ui->listWidget2->setCurrentRow(ui->listWidget2->count()-1);
    }
}

void MsgShowDlg::on_btnDeleteImage2_clicked()
{

    int nRow = ui->listWidget2->currentRow();
    if(nRow>=0)
    {
        QListWidgetItem *pItem = ui->listWidget2->takeItem(nRow);
        delete pItem;
    }
}

QString  MsgShowDlg::GetMessageTitle()
{
    return ui->messageTitle->text();
}

void  MsgShowDlg::SetMessageTitle(QString strTitle)
{
    ui->messageTitle->setText(strTitle);
}

bool MsgShowDlg::GetMessageData(QByteArray &msgData)
{
   //文字信息
   if(ui->comboBox->currentIndex()==0)
   {
       m_MessageBlock.nType = 0;
       m_MessageBlock.strXmlPublic = ui->messageText->toPlainText().toStdString();
       m_MessageBlock.strXmlEncode.clear();
       m_MessageBlock.ImageA.clear();
       m_MessageBlock.ImageB.clear();
       CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
       ss << m_MessageBlock;
       msgData.resize(ss.size());
       memcpy(msgData.data(),&(*ss.begin()),ss.size());
       LogPrintf("***************send text\n");
   }else
   {
       //模板信息
       m_MessageBlock.ImageA.clear();
       m_MessageBlock.ImageB.clear();

       m_MessageBlock.nType = 1;
       m_MessageBlock.SetPassword(ui->password->text().toStdString());
       m_MessageBlock.strXmlPublic = m_pXmlModel1->SaveXmlModel().toStdString();
       std::string strEncodeXml = m_pXmlModel2->SaveXmlModel().toStdString();
       m_MessageBlock.EncodeXml(strEncodeXml);

       int nCount;
       int i;
       CImageFile imagefile;
       nCount = ui->listWidget->count();
       for(i=0;i<nCount;i++)
       {
           QListWidgetItem *pItem = ui->listWidget->item(i);
           if(pItem)
           {
               QFileInfo finfo(pItem->text());
               imagefile.strFileName = finfo.fileName().toStdString();
               int nSize = finfo.size();

               QFile file(pItem->text());
               if(!file.open(QFile::ReadOnly))
                   continue;
               imagefile.ImageData.resize(nSize);

               file.read(&(*imagefile.ImageData.begin()),nSize);
               file.close();
                m_MessageBlock.ImageA.push_back(imagefile);
           }
       }

       nCount = ui->listWidget2->count();

       for(i=0;i<nCount;i++)
       {
           QListWidgetItem *pItem = ui->listWidget2->item(i);
           if(pItem)
           {
               QFileInfo finfo(pItem->text());
               imagefile.strFileName = finfo.fileName().toStdString();
               int nSize = finfo.size();
                QFile file(pItem->text());
               if(!file.open(QFile::ReadOnly))
                   continue;
               imagefile.ImageData.resize(nSize);
               file.read(&(*imagefile.ImageData.begin()),nSize);
               file.close();

               m_MessageBlock.EncodeImage(imagefile.strFileName,imagefile.ImageData);
           }
       }
       CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
       ss << m_MessageBlock;

       msgData.resize(ss.size());
       memcpy(&(*msgData.begin()),&(*ss.begin()),ss.size());
       LogPrintf("***************send xml\n");

   }
   return true;
}

bool MsgShowDlg::SetMessageData(QByteArray &msgData)
{
    if(msgData.size()==0) return false;

    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss.reserve(msgData.size());
    ss.write(&(*msgData.begin()),msgData.size());
    ss >> m_MessageBlock;

    if(m_MessageBlock.strXmlPublic.size()>0)
    {
        if(m_MessageBlock.strXmlPublic.at(0)=='<')
            m_MessageBlock.nType = 1;
    }
    if(m_MessageBlock.nType==0)
    {
        ui->rbtnPublicImage->setVisible(false);
        ui->rbtnEncryptImage->setVisible(false);
        ui->rbtnPublicXml->setVisible(false);
        ui->rbtnEncryptXml->setVisible(false);
        ui->messageText->setPlainText(QString::fromStdString(m_MessageBlock.strXmlPublic));
        ui->stackedWidget->setCurrentIndex(0);
    }else
    {
        if(m_pXmlModel1)
        {
            delete m_pXmlModel1;
            m_pXmlModel1 = NULL;
        }
        m_pXmlModel1 = new CXmlModel(ui->scrollArea);
        m_pXmlModel1->LoadXmlModel(QString::fromStdString(m_MessageBlock.strXmlPublic));

        ui->listWidget->clear();
        int nCount;
        int i;
        nCount = m_MessageBlock.ImageA.size();

        for(i=0;i<nCount;i++)
        {
            ui->listWidget->addItem(QString::fromStdString(m_MessageBlock.ImageA[i].strFileName));
        }

        nCount = m_MessageBlock.ImageB.size();

        for(i=0;i<nCount;i++)
        {
            ui->listWidget2->addItem(QString::fromStdString(m_MessageBlock.ImageB[i].strFileName));
        }

        ui->rbtnPublicXml->setChecked(true);
        ui->stackedWidget->setCurrentIndex(1);

    }

    return true;
}

void MsgShowDlg::ClearMessage()
{
    ui->listWidget->clear();
    ui->listWidget2->clear();
    ui->ImageView->clear();
    ui->ImageView2->clear();
    ui->messageText->clear();
    ui->messageTitle->clear();
    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
}


void MsgShowDlg::on_listWidget2_doubleClicked(const QModelIndex &index)
{
    if(m_bShowOnly)
    {
        on_listWidget2_currentTextChanged(QString(""));
    }
}
