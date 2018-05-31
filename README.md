# 图书管理系统实验报告

[TOC]

## 开发环境

- C++ MSVC2017_64
- QT 5.10.1
- Windows 10
- Qt Creator 4.5.1
- SQLITE 3

## 设计思路

### 需求

1. 存储图书信息、采购和淘汰情况、租借情况
2. 实现图书采购、淘汰、租借功能
3. 实现图书信息、采购和淘汰、库存和租借情况查询
4. 实现图书的采购、库存、淘汰、租借情况等统计

```sequence
title: 实现思想原理图
participant 数据库
participant 数据
participant 用户
participant 界面
participant C++
participant QT

数据库->数据: 删除、查询、修改
数据->数据库: 存储
界面->数据: 传递
QT->界面: Desinger设计
数据->界面: 显示
用户->界面: 控制
界面->用户: 反馈
QT->C++: 编写
C++-->数据库: SQL语句
```

### 数据库结构

#### BOOK表

|  字段名  |  类型   |   介绍   |   备注   |
| :------: | :-----: | :------: | :------: |
|    id    |  text   | ISBN书号 |   主键   |
|   name   |  text   |   书名   |          |
|  author  |  text   |   作者   |          |
|   pub    |  text   |  出版社  |          |
| pub_date |  text   | 出版日期 | yyyy-MM  |
|  avail   |  bool   | 是否可借 | 是1, 否0 |
|  store   | integer |   库存   |          |
|  total   | integer |   总数   |          |

#### LOAN表

|  字段名   | 类型 |   介绍   |    备注    |
| :-------: | :--: | :------: | :--------: |
|  user_id  | text |   用户   |    外键    |
|  book_id  | text |   书籍   |    外键    |
| loan_date | text | 借阅日期 | yyyy-MM-dd |
| back_date | text | 应还日期 | yyyy-MM-dd |

#### MANAGER表

| 字段名 | 类型 |  介绍  | 备注 |
| :----: | :--: | :----: | :--: |
|   id   | text | 管理员 | 主键 |
|  pwd   | text |  密码  |      |

#### USER表

| 字段名 |  类型   |   介绍   | 备注 |
| :----: | :-----: | :------: | :--: |
|   id   |  text   |   用户   | 主键 |
|  pwd   |  text   |   密码   |      |
| email  |  text   |   邮箱   |      |
|  loan  | integer |  借阅数  |      |
|  max   | integer | 可借阅数 |      |

#### MANAGER_OPERATION表

|   字段名   | 类型 |   介绍   |         备注         |
| :--------: | :--: | :------: | :------------------: |
| manager_id | text |  管理员  |         外键         |
| operation  | bool |   操作   | 新增书籍1，删除书籍0 |
|  book_id   | text |   书号   |         外键         |
|    num     | int  | 操作数量 |                      |
|    date    | text | 操作时间 |      yyyy-MM-dd      |

### 业务逻辑

```sequence
title: 业务逻辑图
participant 用户
participant 游客
participant 管理员

游客->用户: 登入
用户-->游客: 注销

游客->管理员: 管理员登入
管理员-->游客: 注销

note over 用户: 借阅、还书、查询、续借、个人信息
note over 游客: 查询
note over 管理员: 图书入库、图书删除、用户信息管理、管理操作查询


```

## 项目实现

### 数据库搭建

```sql lite
CREATE TABLE book(
id text primary key,
name text,
author text,
pub text,
pub_date text,
avail blob,
store integer,
total integer);

```

```sql lite
CREATE TABLE user(
id text primary key,
pwd text,
email text,
loan integer,
max integer);
```

```sql lite
CREATE TABLE manager(
id text primary key,
pwd text);
```

```sql lite
CREATE TABLE loan(
user_id text,
book_id text,
loan_date text,
back_date text,
constraint u_id_cons foreign key (user_id) references user(id),
constraint b_id_cons foreign key (book_id) references book(id));
```

```sql lite
CREATE TABLE manager_operation(
manager_id text,
operation bool,
book_id text,
num int,
date text,
foreign key(manager_id) references manager(id));
```

### 在QT中配置数据库

QT提供了一套很强大的API，用来支持数据库与UI界面的通信。

使用`QSqlTableModel`类，可基本上实现数据库的所有功能。

例如，创建数据库视图:

```c++
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
```

这样，就可以在UI中显示数据库。

### 代码及功能实现

#### 文件结构

mylibrary

- mylibrary.pro项目文件

- 头文件

  insert.h //图书录入

  mainwindow.h //主窗口

  register.h //注册界面

- 源文件

  main.cpp

  insert.cpp

  mainwindow.cpp

  register.cpp

- 界面文件

  insert.ui

  mainwindow.ui

  register.ui

#### 具体实现

##### 用户、管理员登入

用户登入分为普通登入和管理员登入，区别在于UI中的管理员登入选项是否选择（用户账号和管理员账号是分开的）。

用户登入:

```c++
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
```

管理员登入: 

```c++
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
```

用户登入在UI主界面，这样设计会使得排版自然。



用户登入GIF:

![](https://raw.githubusercontent.com/Roarcannotprogramming/Image_sources/master/user_login.gif)

管理员登入GIF:

![](https://raw.githubusercontent.com/Roarcannotprogramming/Image_sources/master/manager_login.gif)



##### 用户注册

注册为单独的界面。

管理员账号为数据库创建时就创建，故不增加新增管理员操作。

用户注册: 

```c++
void Register::UsrRegister()
{
    if(ui->lineEdit->text().isEmpty()||ui->lineEdit_2->text().isEmpty()
            ||ui->lineEdit_3->text().isEmpty())
    {
        QMessageBox::critical(NULL,"Error","信息填写不完整！",QMessageBox::Yes);
        return;
    }

    if(ui->lineEdit_2->text().compare(ui->lineEdit_3->text())!=0)
    {
        QMessageBox::critical(NULL,"Error","输入密码不一致！",QMessageBox::Yes);
        return;
    }

    QSqlQuery query;
    query.exec("select id from user where id = '"+ui->lineEdit->text()+"'");
    if(query.next())
    {
        QMessageBox::critical(NULL,"Error","该用户名已被注册！",QMessageBox::Yes);
        return;
    }

    query.exec("insert into user values('"+ui->lineEdit->text()+"','"+ui->lineEdit_2
               ->text()+"','"+ui->lineEdit_4->text()+"',0,8)");
    if(query.isActive())
    {
        close();
        QMessageBox::about(0,"Register","注册成功！");
    }
    else
    {
        QMessageBox::critical(NULL,"Error","注册失败！",QMessageBox::Yes);
        return;
    }
}
```

用户注册GIF:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/user_register.gif?raw=true)

重复注册GIF:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/user_register_failed.gif?raw=true)



##### 书目查找

支持书名和作者查找。

本着学习知识和简便性的原则，使用QT提供的API来实现。实现非常简单。

```c++
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
```

书名查找GIF:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/search_book.gif?raw=true)

作者查找GIF:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/search_book_author.gif?raw=true)



##### 借书、续借与还书

借阅书籍的时候需要维护用户、借阅、书目表，所以代码比较复杂。

还要判断是否可借等。

```c++
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
```

```c++
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
```

```c++
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
```

借阅、续借与还书GIF:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/lend_book.gif?raw=true)



##### 管理员操作

管理员功能分为图书管理、用户管理。

每次管理都会生成相应的操作记录，以方便审计。

图书管理可以新增或删除书目。

也可以删除用户。

```c++
//插入书目
void Insert::BkInsert()
{
    if(ui->lineEdit->text().isEmpty()||ui->lineEdit_2->text().isEmpty()||
            ui->lineEdit_3->text().isEmpty()||ui->lineEdit_4->text().isEmpty()
            ||ui->lineEdit_5->text().isEmpty()||ui->spinBox->text()==0)
    {
        QMessageBox::critical(NULL,"Error","信息不完整！",QMessageBox::Yes);
        return;
    }

    QSqlQuery query;
    query.exec("select id from book where id='"+ui->lineEdit->text()+"'");
    if(query.next())
    {
        QMessageBox::critical(NULL,"Error","重复录入！",QMessageBox::Yes);
        return;
    }

    query.exec("insert into book values('"+
               ui->lineEdit->text()+"','"+
               ui->lineEdit_2->text()+"','"+
               ui->lineEdit_3->text()+"','"+
               ui->lineEdit_5->text()+"','"+
               ui->lineEdit_4->text()+"',1,'"+
               ui->spinBox->text()+"','"+ui->spinBox->text()+"')");
    if(query.isActive())
    {
        QMessageBox::about(0,"Insert","已录入！");

        QDate d = QDate::currentDate();
        QString date = d.toString("yyyy-MM-dd");
        query.exec("insert into manager_operation values('a_temp_id',1,'" +
                   ui->lineEdit->text() + "','" +
                   ui->spinBox->text()  + "','" +
                   date                 +"')"
                   );
    }
    else
    {
        QMessageBox::critical(NULL,"Error","失败！",QMessageBox::Yes);
        return;
    }

}
```

```c++
//删除书目
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
                   book_id              + "'," +
                   oper_num             + ",'" +
                   date                 +"')"
                   );
        OperModel->select();
    }
}
```

```c++
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
```

插入书目GIF、查看操作管理:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/insert_book.gif?raw=true)

删除书目GIF:

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/delete_book.gif?raw=true)

删除用户GIF：

![](https://github.com/Roarcannotprogramming/Image_sources/blob/master/delete_user.gif?raw=true)

## 项目总结

项目开始于2018.04.21，截至目前共耗时41天，工作时间约为40小时左右（包括学习技术时间）。

Qt的确是一款非常好用的UI设计软件，Qt以C++为基础语言，在其基础上加入大量UI相关的库，而且具有可视化编程的特点（PyQt使得Qt同时兼容python编程，更加使得Qt受到欢迎）。

Qt对于数据库的支持程度远超我的估计。起初认为能够兼容sql语言就很好，学习Qt后发现Qt同时提供了很多API方便编写程序，极大地提高了效率。

功能上，项目分为基础功能（需求部分），和附加功能（合理增加功能）。将用户主要分为普通用户和管理员，管理员可以执行用户的所有操作并增加管理功能，用户只能进行借阅和查找。此外，没有登入的用户默认为游客，游客只能查看有哪些书目，不能借阅。

结构上，项目分为查找、借阅、管理，对应SQL中的SELECT、UPDATE、INSERT、DELETE等操作。

附: 

​	github： [github](https://github.com/Roarcannotprogramming/database_homework)