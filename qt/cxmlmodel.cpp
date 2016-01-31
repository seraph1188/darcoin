
#include <QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomAttr>
#include <QtDebug>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QComboBox>
#include "cxmlmodel.h"
#include "../util.h"



CXmlModel::CXmlModel(QWidget *pParent)
{
    m_bLoadSuccess = false;
    m_pParent = pParent;
}

CXmlModel::~CXmlModel()
{
    ClearModel();
}

void CXmlModel::CreateWidget(QString strClass,QString strName,QString strText,QRect rt,QString strMask)
{
    if(strClass=="QLabel")
    {
        QLabel *plabel = new QLabel(m_pParent);
        plabel->setObjectName(strName);
        plabel->setGeometry(rt);
        plabel->setText(strText);
        plabel->setVisible(true);
        m_wList.push_back(plabel);
    }
    else if(strClass=="QLineEdit")
    {
        QLineEdit *pedit = new QLineEdit(m_pParent);
        pedit->setObjectName(strName);
        pedit->setGeometry(rt);
        if(!strMask.isEmpty())
            pedit->setInputMask(strMask);
        pedit->setText(strText);
        pedit->setVisible(true);
        m_wList.push_back(pedit);
    }else if(strClass=="QTextEdit")
    {
        QTextEdit *pedit = new QTextEdit(m_pParent);
        pedit->setObjectName(strName);
        pedit->setGeometry(rt);
        pedit->setPlainText(strText);
        pedit->setVisible(true);
        m_wList.push_back(pedit);
    }else if(strClass=="QDateEdit")
    {
        QDateEdit *pedit = new QDateEdit(m_pParent);
        pedit->setObjectName(strName);
        pedit->setGeometry(rt);
        if(strText.isEmpty())
            pedit->setDate(QDate::currentDate());
        else
            pedit->setDate(QDate::fromString(strText,"yyyy-MM-dd"));
        pedit->setVisible(true);
        m_wList.push_back(pedit);
    }else if(strClass=="QComboBox")
    {
        QComboBox *pedit = new QComboBox(m_pParent);
        pedit->setObjectName(strName);
        pedit->setGeometry(rt);
        QString strCurrent;
        strCurrent = strText.left(strText.indexOf("||"));
        strText = strText.mid(strText.indexOf("||")+2);
        QStringList strList = strText.split("|");
        foreach(QString s,strList)
        {
            if(s.isEmpty()) continue;
            pedit->addItem(s);
        }
        pedit->setCurrentText(strCurrent);
        pedit->setVisible(true);
        m_wList.push_back(pedit);
    }
}

QString CXmlModel::GetWidgetText(QString strClass,QString strName)
{
    foreach (QWidget *pW, m_wList) {
        if(pW->objectName()==strName)
        {
            if(strClass=="QLineEdit")
            {
                QLineEdit *pedit = (QLineEdit *)pW;
                return pedit->text();
            }else if(strClass=="QLabel")
            {
                QLabel *plabel = (QLabel *)pW;
                return plabel->text();
            }else if(strClass=="QTextEdit")
            {
                QTextEdit *pedit =  (QTextEdit *)pW;
                return pedit->toPlainText();
            }else if(strClass=="QDateEdit")
            {
                QDateEdit *pedit =  (QDateEdit *)pW;
                return pedit->date().toString("yyyy-MM-dd");

            }else if(strClass=="QComboBox")
            {
                QComboBox *pedit = (QComboBox *)pW;
                return pedit->currentText();
            }else
                return "";

        }
    }
    return QString();
}

bool CXmlModel::LoadXmlModel(QString strXml)
{

    ClearModel();
    if(strXml.size()<10) return false;

    QDomDocument    document;
    QString         strError;
    int             errLin = 0, errCol = 0;
    if( !document.setContent(strXml, false, &strError, &errLin, &errCol)) {
        LogPrintf("parse file failed at line %d column %d, error: %s !\n", errLin, errCol, strError.toStdString().c_str());
        return false;
    }

    if( document.isNull() ) {
        LogPrintf( "document is null !\n" );
        return false;
    }


    QDomElement root;
    QDomElement widget;
    root = document.documentElement();
    if(root.tagName()!="ui")
        return false;
    //查找窗体组件
    QDomElement mainform = root.firstChildElement("widget");
    //查找第二级组件
    if(mainform.isNull())
    {
        return false;
    }

    QString strClass,strName,strText,strMask;
    QRect rt;

    QDomElement mainattr = mainform.firstChildElement("property");
    while(!mainattr.isNull())
    {
        if(mainattr.attribute("name")=="geometry")
        {
            QDomElement ert = mainattr.firstChildElement("rect");

            QDomElement nv;
            nv = ert.firstChildElement("x");
            rt.setLeft(nv.text().toInt());
            nv = ert.firstChildElement("y");
            rt.setTop(nv.text().toInt());
            nv = ert.firstChildElement("width");
            rt.setWidth(nv.text().toInt());
            nv = ert.firstChildElement("height");
            rt.setHeight(nv.text().toInt());
            m_pParent->setGeometry(rt);
        }
        mainattr = mainattr.nextSiblingElement("property");
    }

    widget = mainform.firstChildElement("widget");

    while(!widget.isNull())
    {
        strName = widget.attribute("name");
        strClass =  widget.attribute("class");
        strText.clear();
        strMask.clear();

        QDomElement att = widget.firstChildElement();

        while(!att.isNull())
        {
            if(att.tagName()=="item")
            {

                QDomElement nv = att.firstChildElement("property");
                if(nv.attribute("name")=="text")
                    strText += "|"+nv.firstChildElement("string").text();

            }else if(att.tagName()=="property")
            {
                if(att.attribute("name")=="text")
                {
                    QDomElement nv = att.firstChildElement();
                    strText = nv.text();
                }else if(att.attribute("name")=="geometry")
                {
                    QDomElement ert = att.firstChildElement("rect");

                    QDomElement nv;
                    nv = ert.firstChildElement("x");
                    rt.setLeft(nv.text().toInt());
                    nv = ert.firstChildElement("y");
                    rt.setTop(nv.text().toInt());
                    nv = ert.firstChildElement("width");
                    rt.setWidth(nv.text().toInt());
                    nv = ert.firstChildElement("height");
                    rt.setHeight(nv.text().toInt());

                }else if(att.attribute("name")=="dateTime")
                {
                    QDomElement sub = att.firstChildElement("datetime");
                    int day,month,year;
                    year = sub.firstChildElement("year").text().toInt();
                    month = sub.firstChildElement("month").text().toInt();
                    day = sub.firstChildElement("day").text().toInt();
                    strText.sprintf("%04d-%02d-%02d",year,month,day);
                }else if(att.attribute("name")=="currentText")
                {
                    QDomElement nv = att.firstChildElement();
                    strText = nv.text()+"|";
                }else if(att.attribute("name")=="inputMask")
                {
                    QDomElement nv = att.firstChildElement();
                    strMask = nv.text();
                }
            }
            att = att.nextSiblingElement();
        }
        CreateWidget(strClass,strName,strText,rt,strMask);

        widget = widget.nextSiblingElement("widget");
    }
    m_bLoadSuccess = true;
    return true;
}





QString CXmlModel::SaveXmlModel()
{
    QDomDocument    document;
    QString         strError;
    int             errLin = 0, errCol = 0;
    if( !document.setContent(m_strXml, false, &strError, &errLin, &errCol)) {
        LogPrintf("parse file failed at line %d column %d, error: %s !\n", errLin, errCol, strError.toStdString().c_str());
        return QString("N/A");
    }

    if( document.isNull() ) {
        LogPrintf( "document is null !\n" );
        return QString("N/A");
    }

    QDomElement root;
    QDomElement widget;
    root = document.documentElement();
    if(root.tagName()!="ui")
        return QString("N/A");
    //查找窗体组件
    QDomElement mainform = root.firstChildElement("widget");
    //查找第二级组件
    if(!mainform.isNull())
         widget = mainform.firstChildElement("widget");

    QString strClass,strName,strText;
    QRect rt;
    while(!widget.isNull())
    {
        strName = widget.attribute("name");
        strClass =  widget.attribute("class");
        strText = GetWidgetText(strClass,strName);
        if(strClass=="QLabel")
        {
            widget = widget.nextSiblingElement("widget");
            continue;
        }
        bool bFindProperty = false ;
        QDomElement att = widget.firstChildElement("property");
        if((strClass=="QLineEdit")||(strClass=="QTextEdit"))
        {
            while(!att.isNull())
            {

                if(att.attribute("name")=="text")
                {
                    QDomElement nv = att.firstChildElement();
                    strText = GetWidgetText(strClass,strName);
                    nv.firstChild().setNodeValue(strText);
                    bFindProperty = true;

                }
                att = att.nextSiblingElement("property");
            }
            if(!bFindProperty)
            {
                QDomElement newnode = document.createElement("property");
                newnode.setAttribute("name","text");
                widget.appendChild(newnode);
                QDomElement node = document.createElement("string");
                newnode.appendChild(node);
                QDomText dt = document.createTextNode(strText);
                node.appendChild(dt);
            }
        }else if(strClass=="QDateEdit")
        {
            while(!att.isNull())
            {

                if(att.attribute("name")=="dateTime")
                {
                    QDomElement nv = att.firstChildElement("datetime");
                    if(!nv.isNull())
                    {
                        strText = GetWidgetText(strClass,strName);
                        QDate dt = QDate::fromString(strText,"yyyy-MM-dd");
                        QDomNode sub;
                        sub = nv.firstChildElement("year");
                        if(!sub.isNull())
                            sub.firstChild().setNodeValue(QString::number(dt.year()));
                        sub = nv.firstChildElement("month");
                        if(!sub.isNull())
                            sub.firstChild().setNodeValue(QString::number(dt.month()));
                        sub = nv.firstChildElement("day");
                        if(!sub.isNull())
                            sub.firstChild().setNodeValue(QString::number(dt.day()));
                    }
                }
                att = att.nextSiblingElement("property");
            }
        }else if(strClass=="QComboBox")
        {
            while(!att.isNull())
            {
                if(att.attribute("name")=="currentText")
                {

                    QDomElement nv = att.firstChildElement();
                    strText = GetWidgetText(strClass,strName);
                    nv.firstChild().setNodeValue(strText);
                }
                att = att.nextSiblingElement("property");
            }

        }

        widget = widget.nextSiblingElement("widget");
    }

    return document.toString();
}



void CXmlModel::ClearModel()
{
    foreach (QWidget *pWidget, m_wList) {

        delete pWidget;
    }
    m_wList.clear();
}


bool CXmlModel::LoadXmlModelFromFile(QString strFile)
{
    QFile file(strFile);
    if( !file.open(QFile::ReadOnly | QFile::Text) ) {
        LogPrintf("**************xml file open failed\n");
        return false;
    }

    QTextStream  tfile(&file);
    tfile.setCodec("utf-8");
    m_strXml = tfile.readAll();
    file.close();
    return LoadXmlModel(m_strXml);
}

bool CXmlModel::SaveXmlModelToFile(QString strFile)
{

    QFile sfile(strFile);
    if(!sfile.open(QFile::WriteOnly))
    {
        return false;
    }

    QTextStream sstream(&sfile);
    sstream.setCodec("utf-8");
    sstream << SaveXmlModel();
    sfile.close();
    return true;
}

