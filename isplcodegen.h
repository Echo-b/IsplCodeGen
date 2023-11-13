#ifndef ISPLCODEGEN_H
#define ISPLCODEGEN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class IsplCodeGen; }
QT_END_NAMESPACE

class IsplCodeGen : public QMainWindow
{
    Q_OBJECT

public:
    IsplCodeGen(QWidget *parent = nullptr);
    void initItemsStatus();
    void resetItemsStatus();
    QString getCurAgentCode();
    void saveParams(QString AgentCode,QString AgentName);
    void checkAndUpdateIsplCode();
    QString CopyAgent();
    QString AssignCombineAgentCode(QString replaceName1,QString replaceName2);
    ~IsplCodeGen();

private:
    Ui::IsplCodeGen *ui;

private slots://槽函数定义
    void newFile(); // 新建文件
    void exportFile(); // 保存文件
    void AgentCodeGen();
    void EditIsplCode();
    void EvaluationCodeGen();
    void InitStatesCodeGen();
    void GroupsCodeGen();
    void FormulaeCodeGen();
    void setComboxStatus();
    void saveAgentCode();
    void genCodeWithTemplate();
    void clearIsplCode();

private:
    QString currentFile;
    QString curAgentCode;
    QString curAgentName;
    QString preAgentCode;
    QString preAgentName;
    int cnt ;
    QString assignFileName;
};


#endif // ISPLCODEGEN_H
