#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("图书管理系统");
    dbCreat();
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(2);
    ui->tabWidget->removeTab(3);
    ui->tabWidget->removeTab(4);
    user_type=VISITOR;
    id="visitor";
    UiUpdate();
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

//数据库连接
bool MainWindow::dbConnect()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("library.db");
    if(!db.open())
    {
        qDebug() << "Database Error!";
        return false;
    }
    return true;
}

//数据库创建
bool MainWindow::dbCreat()
{
    if(dbConnect()){
        BkModel=new QSqlTableModel(this);
        BkModel->setTable("book");
        BkModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        BkModel->setSort(BkId,Qt::AscendingOrder);
        BkModel->setHeaderData(BkId, Qt::Horizontal, "ISBN");
        BkModel->setHeaderData(BkName, Qt::Horizontal, "书名");
        BkModel->setHeaderData(BkAuthor,Qt::Horizontal,"作者");
        BkModel->setHeaderData(BkPub, Qt::Horizontal, "出版社");
        BkModel->setHeaderData(BkPubDate, Qt::Horizontal, "出版日期");
        BkModel->setHeaderData(BkAvail, Qt::Horizontal, "是否可借");
        BkModel->setHeaderData(BkStore, Qt::Horizontal, "库存");
        BkModel->setHeaderData(BkTotal, Qt::Horizontal, "总数");
        BkModel->select();
        ui->tableView->setModel(BkModel);

        UsrModel=new QSqlTableModel(this);
        UsrModel->setTable("user");
        UsrModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        UsrModel->setSort(UsrId,Qt::AscendingOrder);
        UsrModel->setHeaderData(UsrId, Qt::Horizontal, "ID");
        UsrModel->setHeaderData(UsrPwd, Qt::Horizontal, "密码");
        UsrModel->setHeaderData(UsrEmail, Qt::Horizontal, "邮箱");
        UsrModel->setHeaderData(UsrLoan, Qt::Horizontal, "已借数量");
        UsrModel->setHeaderData(UsrMax, Qt::Horizontal, "可借数量");
        UsrModel->select();
        ui->tableView_2->setModel(UsrModel);

        LoanModel=new QSqlTableModel(this);
        LoanModel->setTable("loan");
        LoanModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        LoanModel->setSort(LUsrId,Qt::AscendingOrder);
        LoanModel->setHeaderData(LUsrId,Qt::Horizontal,"用户ID");
        LoanModel->setHeaderData(LBkId,Qt::Horizontal,"ISBN");
        LoanModel->setHeaderData(LDate,Qt::Horizontal,"借阅日期");
        LoanModel->setHeaderData(LBackDate,Qt::Horizontal,"应还日期");
        LoanModel->select();
        ui->tableView_4->setModel(LoanModel);

        OperModel= new QSqlTableModel(this);
        OperModel->setTable("manager_operation");
        OperModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        OperModel->sort(Op_date,Qt::DescendingOrder);
        OperModel->setHeaderData(M_id,Qt::Horizontal,"管理员ID");
        OperModel->setHeaderData(Oper,Qt::Horizontal,"操作");
        OperModel->setHeaderData(Bk_id,Qt::Horizontal,"书号");
        OperModel->setHeaderData(Op_num,Qt::Horizontal,"改动数量");
        OperModel->setHeaderData(Op_date,Qt::Horizontal,"改动时间");
        OperModel->select();
        ui->tableView_5->setModel(OperModel);

           qDebug() << "Database Create Sucessfully!";
           return true;
       }
       else{
           qDebug() << "Database Create Failed!";
           return false;
       }

}

//用户登录
void MainWindow::UsrLogin()
{
    QSqlQuery query;
        query.exec("select pwd from user where id = '"
                    +ui->lineEdit_4->text()+"'");
        if(!query.isActive())
        {
            return;
        }

        if(query.next()){
            QString password = query.value(0).toString();
            if(QString::compare(password,ui->lineEdit_5->text())!=0)
            {
                QMessageBox::critical(NULL, "Error", "密码错误",
                                      QMessageBox::Yes);
                return;
            }
        }
        else
        {
            QMessageBox::critical(NULL, "Error","用户名不存在",QMessageBox::Yes);
            return;
        }
        id=ui->lineEdit_4->text();

        //更新界面
        user_type=USER;
        UiUpdate();
        QMessageBox::about(0,"Login","登陆成功！");

}

//管理员登录
void MainWindow::MngLogin()
{
    QSqlQuery query;
        query.exec("select pwd from manager where id = '"
                    +ui->lineEdit_4->text()+"'");
        if(!query.isActive())
        {
            return;
        }

        if(query.next()){
            QString password = query.value(0).toString();
            if(QString::compare(password,ui->lineEdit_5->text())!=0)
            {
                QMessageBox::critical(NULL, "Error", "密码错误",
                                      QMessageBox::Yes);
                return;
            }
        }
        else
        {
            QMessageBox::critical(NULL, "Error","管理员不存在",QMessageBox::Yes);
            return;
        }
        id=ui->lineEdit_4->text();

        //更新界面
        user_type=MANAGER;
        UiUpdate();
        QMessageBox::about(0,"Login","登陆成功！");

}


//更新界面
void MainWindow::UiUpdate()
{
    switch(user_type)
    {
    case USER:
    {
        ui->pushButton_2->show();
        ui->pushButton_9->hide();
        ui->pushButton_10->hide();
        ui->pushButton_11->show();
        ui->lineEdit_4->hide();
        ui->lineEdit_5->hide();
        ui->checkBox_3->hide();
        ui->tabWidget->addTab(ui->tab_3,"用户信息");
        ui->label->setText("用户："+id);
        ui->label_3->setText("用户："+id);
        break;
    }
    case MANAGER:
    {
        ui->pushButton_3->show();
        ui->pushButton_4->show();
        ui->pushButton_5->show();
        ui->pushButton_6->show();
        ui->pushButton_9->hide();
        ui->pushButton_10->hide();
        ui->pushButton_11->show();
        ui->pushButton_12->show();
        ui->pushButton_13->show();
        ui->lineEdit_4->hide();
        ui->lineEdit_5->hide();
        ui->checkBox_3->hide();
        ui->tabWidget->addTab(ui->tab_2,"用户管理");
        ui->tabWidget->addTab(ui->tab_4,"借阅信息");
        ui->tabWidget->addTab(ui->tab_5,"操作管理");
        ui->label->setText("管理员："+id);
        break;
    }
    case VISITOR:
    {
        ui->pushButton_2->hide();
        ui->pushButton_3->hide();
        ui->pushButton_4->hide();
        ui->pushButton_12->hide();
        ui->pushButton_13->hide();
        ui->pushButton_9->show();
        ui->pushButton_10->show();
        ui->pushButton_11->hide();
        ui->pushButton_5->hide();
        ui->pushButton_6->hide();
        ui->lineEdit_4->show();
        ui->lineEdit_5->show();
        ui->checkBox_3->show();
        ui->tabWidget->removeTab(3);
        ui->tabWidget->removeTab(2);
        ui->tabWidget->removeTab(1);
        ui->label->setText("游客");
        break;
    }
    }

}

//登录
void MainWindow::on_pushButton_9_clicked()
{
    if (ui->checkBox_3->checkState()==Qt::Checked)
    {
        MngLogin();
    }
    else
    {
        UsrLogin();
        //显示借阅信息
        UsrLoanModel=new QSqlTableModel(this);
        UsrLoanModel->setTable("loan");
        UsrLoanModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        UsrLoanModel->setSort(LUsrId,Qt::AscendingOrder);
        UsrLoanModel->setHeaderData(LUsrId,Qt::Horizontal,"用户ID");
        UsrLoanModel->setHeaderData(LBkId,Qt::Horizontal,"ISBN");
        UsrLoanModel->setHeaderData(LDate,Qt::Horizontal,"借阅日期");
        UsrLoanModel->setHeaderData(LBackDate,Qt::Horizontal,"应还日期");
        UsrLoanModel->setFilter("user_id='"+id+"'");
        UsrLoanModel->select();
        ui->tableView_3->setModel(UsrLoanModel);
    }

}

//注册
void MainWindow::on_pushButton_10_clicked()
{
    Register *reg=new Register(this);
    reg->exec();
    UsrModel->select();
}

//录入书籍
void MainWindow::on_pushButton_4_clicked()
{
    Insert *insert=new Insert(this);
    insert->exec();
    QSqlQuery query;
    query.exec("update manager_operation set manager_id = '" + id + "' where manager_id = 'a_temp_id'");
    BkModel->select();
    OperModel->select();
}

//注销
void MainWindow::on_pushButton_11_clicked()
{
    user_type=VISITOR;
    id="visitor";
    UiUpdate();
}

//提交
void MainWindow::on_pushButton_5_clicked()
{
    BkModel->database().transaction();
    if(BkModel->submitAll())
    {
        BkModel->database().commit();
    }
    else {
        BkModel->database().rollback();
        QMessageBox::warning(this,"BookModel",tr("数据库错误:%1")
                             .arg(BkModel->lastError().text()));
    }
}

//删除
void MainWindow::on_pushButton_3_clicked()
{
    int curRow=ui->tableView->currentIndex().row();
    QString book_id = BkModel->record(curRow).value(BkId).toString();
    int oper_num = BkModel->record(curRow).value(BkTotal).toInt();
    QSqlQuery query;
    QDate d = QDate::currentDate();
    QString date = d.toString("yyyy-MM-dd");

    BkModel->removeRow(curRow);
    int ok=QMessageBox::warning(this,"delete","确定要删除吗？",
                                QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::No)
    {
        BkModel->revertAll();
    }
    else {
        BkModel->submitAll();
        query.exec("insert into manager_operation values('"+
                   id                   + "'," +
                   "0"                  + ",'" +
                   book_id              + "','" +
                   oper_num             + "','" +
                   date                 +"')"
                   );
        OperModel->select();
    }
}

//撤销
void MainWindow::on_pushButton_6_clicked()
{
    BkModel->revertAll();
}

//删除用户
void MainWindow::on_pushButton_7_clicked()
{
    int curRow=ui->tableView_2->currentIndex().row();
    UsrModel->removeRow(curRow);
    int ok=QMessageBox::warning(this,"delete","确定要删除吗？",
                                QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::No)
    {
        UsrModel->revertAll();
    }
    else {
        UsrModel->submitAll();
    }
}

//提交用户修改
void MainWindow::on_pushButton_12_clicked()
{
    UsrModel->database().transaction();
    if(UsrModel->submitAll())
    {
        UsrModel->database().commit();
    }
    else {
        UsrModel->database().rollback();
        QMessageBox::warning(this,"UserModel",tr("数据库错误:'%1'")
                             .arg(UsrModel->lastError().text()));
    }
}

//撤销用户修改
void MainWindow::on_pushButton_13_clicked()
{
    UsrModel->revertAll();
}

//借阅
void MainWindow::on_pushButton_2_clicked()
{
    int curRow=ui->tableView->currentIndex().row();
    int cur_num = BkModel->record(curRow).value(BkStore).toInt();
    int cur_bk_brw = UsrModel->record(0).value(UsrLoan).toInt();
    int max_bk_brw = UsrModel->record(0).value(UsrMax).toInt();
    if(cur_bk_brw>= max_bk_brw){
        QMessageBox::critical(this,"Error!", "你的借书量达到上限！", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QString book=BkModel->record(curRow).value(BkId).toString();
    QDate d=QDate::currentDate();
    QString date=d.toString("yyyy-MM-dd");
    QDate bd=d.addMonths(1);
    QString back_date=bd.toString("yyyy-MM-dd");

    QSqlQuery query;
    query.exec("select * from loan where user_id = '" + id + "' and book_id ='" + book + "'");
    if(query.next()){
        QMessageBox::critical(this, "Error!", "你已经借阅过这本书了！",
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if(cur_num<=0){
        QMessageBox::critical(this,"Error!", "本数不足！",QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    int rowNum = LoanModel->rowCount();
    QSqlRecord record = LoanModel->record();
    record.setValue("user_id",id);
    record.setValue("book_id",book);
    record.setValue("loan_date",date);
    record.setValue("back_date",back_date);
    LoanModel->insertRecord(rowNum,record);
    int ok=QMessageBox::warning(this,"Question","确定要借阅吗？",
                                QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::No)
    {
        LoanModel->revertAll();
    }
    else {
        LoanModel->submitAll();
        UsrLoanModel->select();
        //int cur_num = BkModel->record(curRow).value(BkStore).toInt();
        cur_num -= 1;
        BkModel->setData(BkModel->index(curRow, BkStore), cur_num);
        BkModel->submitAll();
        BkModel->select();

        UsrModel->setFilter("id = '" + id + "'");

        UsrModel->setData(UsrModel->index(0,UsrLoan), cur_bk_brw+1);
        UsrModel->submitAll();
        UsrModel->setFilter("");
        UsrModel->select();
    }
}

//还书
void MainWindow::on_pushButton_15_clicked()
{
    int curRow=ui->tableView_3->currentIndex().row();
    QString ret_book_id = UsrLoanModel->record(curRow).value(LBkId).toString();
    /*QSqlQuery query;
    query.exec("update book set store = store + 1 ");*/
    BkModel->setFilter("id = '" + ret_book_id + "'");
    BkModel->select();
    if(BkModel->rowCount() == 1){
        int cur_num = BkModel->record(0).value(BkStore).toInt();
        BkModel->setData(BkModel->index(0, BkStore), cur_num+1);
        BkModel->submitAll();
        BkModel->setFilter("");
        BkModel->select();
    }



    UsrLoanModel->removeRow(curRow);
    int ok=QMessageBox::warning(this,"back","确定要还书吗？",
                                QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::No)
    {
        UsrLoanModel->revertAll();
    }
    else {
        UsrLoanModel->submitAll();
        LoanModel->select();

        UsrModel->setFilter("id = '" + id + "'");
        int cur_bk_brw = UsrModel->record(0).value(UsrLoan).toInt();
        UsrModel->setData(UsrModel->index(0,UsrLoan), cur_bk_brw-1);
        UsrModel->submitAll();
        UsrModel->setFilter("");
        UsrModel->select();
    }
}

//续借
void MainWindow::on_pushButton_14_clicked()
{
    int curRow=ui->tableView_3->currentIndex().row();
    QString bd=UsrLoanModel->record(curRow).value(LBackDate).toString();
    QDateTime back_date;
    back_date=QDateTime::fromString(bd,"yyyy-MM-dd");
    back_date=back_date.addDays(20);
    bd=back_date.toString("yyyy-MM-dd");
    UsrLoanModel->setData(UsrLoanModel->index(curRow,LBackDate),bd);
    int ok=QMessageBox::warning(this,"back","确定要续借吗？",
                                QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::No)
    {
        UsrLoanModel->revertAll();
    }
    else {
        UsrLoanModel->submitAll();
        LoanModel->select();
    }
}

//书名查询
void MainWindow::on_pushButton_clicked()
{
    BkModel->setFilter("name='"+ui->lineEdit->text()+"'");
    BkModel->select();
}

//作者查询
void MainWindow::on_pushButton_16_clicked()
{
    BkModel->setFilter("author='"+ui->lineEdit_2->text()+"'");
    BkModel->select();
}

//用户查询
void MainWindow::on_pushButton_8_clicked()
{
    UsrModel->setFilter("id='"+ui->lineEdit_3->text()+"'");
    UsrModel->select();
}

//返回主表
void MainWindow::on_pushButton_18_clicked()
{
   BkModel->setTable("book");
   BkModel->select();
}

//返回主表
void MainWindow::on_pushButton_17_clicked()
{
   UsrModel->setTable("user");
   UsrModel->select();
}

//123
QString MainWindow::get_id(){
    return id;
}
