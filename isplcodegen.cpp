#include "isplcodegen.h"
#include "ui_isplcodegen.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QTextStream>
#include <QFile>
#include <QDebug>

IsplCodeGen::IsplCodeGen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IsplCodeGen)
{
    ui->setupUi(this);
    initItemsStatus();
    connect(ui->actionNew, &QAction::triggered, this, &IsplCodeGen::newFile);
    connect(ui->actionExport,&QAction::triggered,this,&IsplCodeGen::exportFile);
    connect(ui->AgentCodeGenBtn,SIGNAL(clicked(bool)),this,SLOT(AgentCodeGen()));
    connect(ui->EvaluationCodeGenBtn,SIGNAL(clicked(bool)),this,SLOT(EvaluationCodeGen()));
    connect(ui->InitStatesCodeGenBtn,SIGNAL(clicked(bool)),this,SLOT(InitStatesCodeGen()));
    connect(ui->GroupsCodeGenBtn,SIGNAL(clicked(bool)),this,SLOT(GroupsCodeGen()));
    connect(ui->FormulateCodeGenBtn,SIGNAL(clicked(bool)),this,SLOT(FormulaeCodeGen()));
    connect(ui->plainTextEditPreview,&QTextEdit::textChanged,this,&IsplCodeGen::EditIsplCode);
    connect(ui->AgentMode_comboBox,&QComboBox::currentTextChanged,this,&IsplCodeGen::setComboxStatus);
    connect(ui->copyCurAgentBtn,SIGNAL(clicked(bool)),this,SLOT(saveAgentCode()));
    connect(ui->GenWithTemplateBtn,SIGNAL(clicked(bool)),this,SLOT(genCodeWithTemplate()));
    connect(ui->clearIsplCodeBtn,SIGNAL(clicked(bool)),this,SLOT(clearIsplCode()));
}

void IsplCodeGen::initItemsStatus()
{
    ui->AgentName_lineEdit->setEnabled(false);
    ui->AgentCodeGenBtn->setText("GENERATE Environment");
    this->cnt = 0;
    ui->SensingVarNum_lineEdit->setText("1");
}

void IsplCodeGen::resetItemsStatus()
{
    ui->AgentName_lineEdit->clear();
    ui->Protocol_plainTextEdit->clear();
    ui->Var_plainTextEdit->clear();
    ui->Evolution_plainTextEdit->clear();
    ui->action_plainTextEdit->clear();
    ui->SensingVarNum_lineEdit->setText("1");
    this->cnt = 0;
}

QString IsplCodeGen::getCurAgentCode()
{
    // 获取lineEdit和comboBox的值
    QString agentName = ui->AgentName_lineEdit->text();
    QString protocol = ui->Protocol_plainTextEdit->toPlainText();
    QString var = ui->Var_plainTextEdit->toPlainText();
    QString action=ui->action_plainTextEdit->toPlainText();
    QString evolution = ui->Evolution_plainTextEdit->toPlainText();
    QString agentMode = ui->AgentMode_comboBox->currentText();


    // 生成ISPL的Agent部分的代码
    QString isplCode;
    if (agentMode == "Normal Agent")
    {
        isplCode = "Agent " + agentName + "\n"; // 写入Agent名称
        isplCode += "  Vars:\n" + var + "\n"; // 写入变量
    }
    else
    {
        isplCode = "Agent Environment\n"; // 写入Agent名称
        isplCode += "  Obsvars:\n" + var + "\n"; // 写入环境
    }
    isplCode+="  Actions={"+action+"}\n";
    isplCode += "  Protocol:\n"; // 写入协议开始标志
    isplCode += "    " + protocol + "\n"; // 写入协议内容
    isplCode += "  end Protocol\n"; // 写入协议结束标志
    isplCode += "  Evolution:\n"; // 写入演化开始标志
    isplCode += "    " +evolution + "\n"; // 写入演化内容
    isplCode += "  end Evolution\n"; // 写入演化结束标志
    isplCode += "end Agent\n"; // 写入Agent结束标志
    saveParams(isplCode,agentName);
    if(ui->SensingVarNum_lineEdit->text().toInt() == 2){
        int index = isplCode.indexOf(agentName);
        QString newAgentName = agentName + QString::number(++this->cnt);
        QString copyAgentCode = isplCode;
        copyAgentCode.replace(index,agentName.size(),newAgentName);
        isplCode += copyAgentCode;
        isplCode += AssignCombineAgentCode(agentName,newAgentName);
    }
    return isplCode;
}

void IsplCodeGen::saveParams(QString AgentCode,QString AgentName)
{
    this->preAgentCode = AgentCode;
    this->preAgentName = AgentName;
}

void IsplCodeGen::checkAndUpdateIsplCode()
{
    if(!ui->Evolution_plainTextEdit->toPlainText().isEmpty())
        this->EvaluationCodeGen();
    else if(!ui->InitStates_plainText->toPlainText().isEmpty())
        this->InitStatesCodeGen();
    else if(!ui->Groups_plainText->toPlainText().isEmpty())
        this->GroupsCodeGen();
    else if(!ui->Formulae_plaintext->toPlainText().isEmpty())
        this->FormulaeCodeGen();
}

QString IsplCodeGen::CopyAgent()
{
    QString currentAgnetCode = this->curAgentCode;
    QString lastAgentName = this->curAgentName;
    QString newAgentName = ui->AgentName_lineEdit->text().isEmpty() ? lastAgentName + QString::number(++this->cnt) : ui->AgentName_lineEdit->text();
    int index = currentAgnetCode.indexOf(lastAgentName);
    currentAgnetCode.replace(index,lastAgentName.size(),newAgentName);
    return currentAgnetCode;
}

IsplCodeGen::~IsplCodeGen()
{
    delete ui;
}

void IsplCodeGen::newFile() // 新建文件
{
    this->assignFileName = QFileDialog::getSaveFileName(this, "Save","newfile.ispl");
//    QDebug
//    ui->plainTextEditPreview->setPlainText(QString());
}
void IsplCodeGen::exportFile()
{
    QString fileName;
    //如何没有文件名就创建一个.
    if (this->assignFileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save","newfile.ispl");
//        currentFile = fileName;
    }
    else
        fileName = this->assignFileName;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {// 无法保存文件则报错
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream out(&file);
    checkAndUpdateIsplCode();
    QString text = ui->plainTextEditPreview->toPlainText();
    out << text;
    file.close();
    QMessageBox::information(this, "information", "save successful");
}

QString IsplCodeGen::AssignCombineAgentCode(QString replaceName1,QString replaceName2)
{
    QString combineAgent = "Agent combineAgent\n"
            "  Vars:\n"
            "    value: boolean;\n"
            "  end Vars\n"
            "  Actions = {Abnormal,Normal};\n"
            "  Protocol:\n"
            "    value=true : {Abnormal};\n"
            "    value=false : {Normal};\n"
            "  end Protocol\n"
            "  Evolution\n"
            "    value=true  if (agentName1.Action=abnormal) or (agentName2.Action=abnormal);\n"
            "    value=false if (agentName1.Action=normal) and (agentName2.Action=normal);\n"
            "  end Evolution\n"
        "end Agent\n";
    combineAgent.replace("combineAgent","Combine"+ replaceName1);
    combineAgent.replace("agentName1",replaceName1);
    combineAgent.replace("agentName2",replaceName2);
    return combineAgent;
}

void IsplCodeGen::AgentCodeGen()
{
    QString isplCode = getCurAgentCode();

    // 返回ISPL的Agent部分的代码
    this->currentFile+=isplCode;
    //A multi-agent system specified in ISPL is composed of an Environment agent and a set of normal agents.
    if(!ui->plainTextEditPreview->toPlainText().isEmpty() && ui->AgentMode_comboBox->currentText() == "Environment"){
        QMessageBox message(QMessageBox::NoIcon, "Information", "Do you want to save current ispl code?", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::No)
            ui->plainTextEditPreview->clear();
        else
            this->exportFile();
    }

    ui->plainTextEditPreview->setPlainText(currentFile);
    resetItemsStatus(); // reset the items status waitting for another agent create

}

void IsplCodeGen::EditIsplCode()
{
    QString newCode=ui->plainTextEditPreview->toPlainText();
    if(newCode!=currentFile)
    {
        currentFile=newCode;
    }
    return;
}

void IsplCodeGen::EvaluationCodeGen()
{
    QString evaluation = ui->Evaluation_plaintext->toPlainText();
    // 生成ISPL的Evaluation部分的代码
    QString isplCode = "Evaluation\n";
    isplCode += "  " + evaluation + "\n";
    isplCode += "end Evaluation\n";

    //修改代码以及清空输入框
    this->currentFile+=isplCode;
    ui->plainTextEditPreview->setPlainText(currentFile);
    ui->Evolution_plainTextEdit->clear();
}

void IsplCodeGen::InitStatesCodeGen()
{
    QString InitStates = ui->InitStates_plainText->toPlainText();
    // 生成ISPL的InitStates部分的代码
    QString isplCode = "InitStates\n";
    isplCode += "  " + InitStates + "\n";
    isplCode += "end InitStates\n";

    //修改代码以及清空输入框
    this->currentFile+=isplCode;
    ui->plainTextEditPreview->setPlainText(currentFile);
    ui->InitStates_plainText->clear();
}

void IsplCodeGen::GroupsCodeGen()
{
    QString Groups = ui->Groups_plainText->toPlainText();
    // 生成ISPL的Gropups部分的代码
    QString isplCode = "Groups\n";
    isplCode += "  " + Groups + "\n";
    isplCode += "end Groups\n";

    //修改代码以及清空输入框
    this->currentFile += isplCode;
    ui->plainTextEditPreview->setPlainText(currentFile);

    ui->Groups_plainText->clear();
}

void IsplCodeGen::FormulaeCodeGen()
{
    QString Formulae = ui->Formulae_plaintext->toPlainText();
    // 生成ISPL的Formulae部分的代码
    QString isplCode = "Formulae\n";
    isplCode += "  " + Formulae + "\n";
    isplCode += "end Formulae\n";

    //修改代码以及清空输入框
    if(!this->currentFile.isEmpty()){
        this->currentFile += isplCode;
        ui->plainTextEditPreview->setPlainText(currentFile);
    }else {
        QMessageBox::about(this, "about",  "please generate agent content");
        return;
    }

    ui->Formulae_plaintext->clear();

}

void IsplCodeGen::setComboxStatus()
{
    QString agentMode = ui->AgentMode_comboBox->currentText();
    if(agentMode == "Environment"){
        ui->AgentName_lineEdit->setEnabled(false);
        ui->VarsLabel->setText("Obsvars");
        ui->AgentCodeGenBtn->setText("GENERATE Environment");
    }
    else{
        ui->AgentName_lineEdit->setEnabled(true);
        ui->VarsLabel->setText("Vars   ");
        ui->AgentCodeGenBtn->setText("GENERATE AGENT TO MODEL");
    }
}

void IsplCodeGen::saveAgentCode()
{
    this->curAgentCode = this->preAgentCode;
    this->curAgentName = this->preAgentName;
}

void IsplCodeGen::genCodeWithTemplate()
{
    this->currentFile += CopyAgent();
    ui->plainTextEditPreview->setPlainText(currentFile);
}

void IsplCodeGen::clearIsplCode()
{
    QMessageBox message(QMessageBox::NoIcon, "Information", "Do you want to clear current ispl code?", QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
        ui->plainTextEditPreview->clear();
    else
        return;
}



