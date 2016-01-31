#ifndef CXMLMODEL_H
#define CXMLMODEL_H
#include <QWidget>

class CXmlModel
{
public:
    CXmlModel(QWidget *pParent);
    ~CXmlModel();
protected:
    QList<QWidget*> m_wList;
    QWidget *m_pParent;
    QString  m_strXml;
    QString  m_strModelFile;
    bool     m_bLoadSuccess;
    QString GetWidgetText(QString strClass,QString strName);
    void CreateWidget(QString strClass,QString strName,QString strText,QRect rt,QString strMask);
    void ClearModel();
public:
    bool LoadXmlModel(QString strModel);
    QString SaveXmlModel();
    bool LoadXmlModelFromFile(QString strFile);
    bool SaveXmlModelToFile(QString strFile);
};

#endif // CXMLMODEL_H
