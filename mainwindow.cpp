#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    t=new Text;//唯一的主数据结构
    label=NULL;
    labelnum=0;
    state=state_initial;



    scrollArea=new QScrollArea(this);
    setCentralWidget(scrollArea);
    screen= new QWidget;


    cursor=new QLabel("▲",screen);
    QFont ft;
    ft.setPointSize(10);
    cursor->setFont(ft);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    cursor->setPalette(pa);

    //测试用 虚拟光标
   // cursor_2=new QLabel("▲",screen);
    //cursor_2->setFont(ft);

    t->chunk=false;

    //定时刷新图形化界面
    QTimer* time=new QTimer(this);
    QObject::connect(time,SIGNAL(timeout()),this,SLOT(graphic_datastruct()));
    time->start(100);

    this->setFocus();
    //setAttribute(Qt::WA_InputMethodEnabled, true);




}

MainWindow::~MainWindow()
{
    delete ui;
    delete screen;
    delete scrollArea;
}
//完成新建文件
void MainWindow::on_actionNew_triggered()
{
    if(state==state_initial||state==state_saved){
        t->Clear();
        d_new * new_d = new d_new;
        QObject::connect(new_d,SIGNAL(sendfilename(QString)),this,SLOT(receiveNewfilename(QString)));
        new_d->show();
    }
    else{
        QMessageBox::about(this,tr("提示信息"),tr("编辑区有未保存内容，请先保存"));
    }

}
void MainWindow::receiveNewfilename(QString filename){
    t->New_File(filename.toStdString());
   // graphic_datastruct();
    state=state_saved;
    QMessageBox::about(this,tr("提示信息"),tr("新建成功"));
}
//完成打开文件
void MainWindow::on_actionOpen_triggered()//点击打开文件
{
    if(state==state_editing){
        QMessageBox::about(this,tr("提示信息"),tr("编辑区有未保存内容，请先保存"));
    }
    else{
        t->Clear();
        DialogOpen * open=new DialogOpen;
        QObject::connect(open,SIGNAL(sendfilename(QString)),this,SLOT(receivefilename(QString)));
        open->show();
    }

}

void MainWindow::receivefilename(QString filename)//打开指定文件 slot function
{
    if(t->Text_Set(filename.toStdString())==0){
        QMessageBox::about(this,tr("提示信息"),tr("该文件不存在，已帮您新建"));
    }
 //   graphic_datastruct();
    state=state_saved;
}
//完成保存
void MainWindow::on_actionSave_triggered()

{

    if(state==state_editing){
        d_save * save_d =new d_save;
        save_d->lineedit_settext(QString::fromStdString(t->filename));
        QObject::connect(save_d,SIGNAL(sendfilename(QString)),this,SLOT(receiveSavefilename(QString)));
        save_d->show();
    }
}
void MainWindow::receiveSavefilename(QString filename){
    t->Save_File(filename.toStdString());
    state=state_saved;
    QMessageBox::about(this,tr("提示信息"),tr("保存成功"));
}
//完成退出操作
void MainWindow::on_actionexit_triggered()
{
    if(state==state_editing){
        QMessageBox::about(this,tr("提示信息"),tr("编辑区有未保存内容，请先保存"));
    }
    else{
        exit(0);
    }
}
//完成查找操作
void MainWindow::on_actionfind_triggered()
{
    d_find* find_d = new d_find;
    QObject::connect(find_d,SIGNAL(send_find_message(QString)),this,SLOT(f_find(QString)));
    find_d->show();
}
void MainWindow::f_find(QString s){
    t->Index_at_Cursor(s.toStdString());
}
//完成替换操作
void MainWindow::on_actionreplace_triggered()
{
    d_replace * replace = new d_replace;
    QObject::connect(replace,SIGNAL(send_replace_message(QString ,QString )),this,SLOT(f_replace(QString,QString)));
    QObject::connect(replace,SIGNAL(send_find_message(QString)),this,SLOT(f_find(QString)));
    replace->show();
}

void MainWindow::f_replace(QString s1, QString s2){
    if(t->Replace(s1.toStdString(),s2.toStdString(),t->cursor.position,t->cursor.line)==1){
        state=state_editing;
    }else{
        QMessageBox::about(this,tr("提示信息"),tr("没有可要替换的内容"));
    }
}

//使用说明
void MainWindow::on_actioninstruction_triggered()
{
    QMessageBox::about(this,tr("使用说明"),tr("光标位移：ctrl/command + I/K/J/L 完成上下左右移动\nctrl + <／> 光标移动到本行的左端或右端\nHome:移动到本行开头\tEnd:移动到本行结尾\n\n打开：ctrl + O\t新建： ctrl + N\n保存：ctrl + S\t强制退出：ctrl + Q／点叉号\n复制：ctrl + C\t粘贴：ctrl + V\n剪切：ctrl + X\t查找：ctrl + F\n替换：ctrl + R"));
}


//定时刷新函数
void MainWindow::graphic_datastruct()//定时执行的槽函数，模拟线程，时刻刷新数据结构到界面
{

    this->setFocus();
    TextNode* text_node=t->headnode;


    int size=20;//字体大小
    float size_width=10;//字体的宽度
    QFont ft("PCMyungjo", size, 25);

    int max_length=t->Get_Max_Length();    //所有内容中最长的那一行有多长


    screen->resize((max_length+1)*size_width,(t->lines+1)*20);
  //释放上一轮的空间  更新label
    for(int i=0;i<labelnum;i++)
        delete label[i];

    QLabel** label_temp=new QLabel*[t->lines];
    labelnum=t->lines;
    label=label_temp;
 //字符串复制给Qlabel 数组循环
        for(int i=0;i<t->lines;i++){
            if(text_node->length!=0)
            {
                std::string str_temp;
                TextBlock* nexttextblock=text_node->first;

                for(int j=0,k=0;j<text_node->length;j++)
                {
                    str_temp+=nexttextblock->text[k];
                    if(k==99){
                        k=0;
                        nexttextblock=nexttextblock->next_block;
                    }else{
                        k++;
                    }
                }
                label[i]=new QLabel(QString::fromStdString(str_temp),screen);
                label[i]->setFont(ft);
                label[i]->move(0,20*i);
                text_node=text_node->nextnode;
            }
            else
            {
                label[i]=new QLabel("",screen);
                label[i]->setFont(ft);
                label[i]->move(0,20*i);
                text_node=text_node->nextnode;
            }
        }
        cursor->move((t->cursor.English*10)+(t->cursor.Chinese*20)-5,(t->cursor.line-1)*20+13);
        if(t->isCursorBehind()!=0){
            if(t->isCursorBehind()==1){
                highlightChunk(t->cursor_virtual,t->cursor);
            }
            else{
                highlightChunk(t->cursor,t->cursor_virtual);
            }
        }

        //到时候隐藏这句，不显示出来虚拟光标
      //  cursor_2->move(t->cursor_virtual.English*10+t->cursor_virtual.Chinese*20-5,(t->cursor_virtual.line-1)*20+13);

        for(int i=0;i<t->lines;i++)
            label[i]->show();

        if(t->chunk==true){
            ui->statusBar->showMessage("块状态");
        }else
           ui->statusBar->showMessage("非块状态");
        scrollArea->setWidget(screen);

}


//键盘热键
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(state!=state_initial){//有文本时允许的操作
        switch(event->key()){

        case Qt::Key_Escape://右删除 电脑上没delete用esc
            t->Delete_at_Cursor(Text::right);
            t->cursor_virtual=t->cursor;
            state=state_editing;
            break;
        case Qt::Key_Backspace://左删除

            if((t->cursor.line!=t->cursor_virtual.line)||(t->cursor.position!=t->cursor_virtual.position)){
                t->BlockDelete_at_Cursor();
            }else{
               t->Delete_at_Cursor(Text::left);
            }


            t->cursor_virtual=t->cursor;
            state=state_editing;
            break;
        case Qt::Key_Delete://右删除
            t->Delete_at_Cursor(Text::right);
            t->cursor_virtual=t->cursor;
            state=state_editing;
            break;
        case Qt::Key_Return://回车
            t->Insert_at_Cursor("\n");
            t->cursor_virtual=t->cursor;
            state=state_editing;
            break;
        case Qt::Key_Home:
            t->MoveCursor_to_start();
            break;
        case Qt::Key_End:
            t->MoveCursor_to_end();
            break;
        case Qt::Key_Left://左
            t->MoveCursor(Text::left);
            if(!t->chunk)
                t->cursor_virtual=t->cursor;
            break;
        case Qt::Key_Down://下
            t->MoveCursor(Text::down);
            if(!t->chunk)
                t->cursor_virtual=t->cursor;
            break;
        case Qt::Key_Right://右
            t->MoveCursor(Text::right);
            if(!t->chunk)
                t->cursor_virtual=t->cursor;
            break;
        case Qt::Key_Up://上
            t->MoveCursor(Text::up);
            if(!t->chunk)
                t->cursor_virtual=t->cursor;
            break;
        default://输入字符串
            QString s= event->text();
            if(s!=""){
                t->Insert_at_Cursor(s.toStdString());
                t->cursor_virtual=t->cursor;//应该先块删除再进行操作
                state=state_editing;
            }
            break;
        }

    }
    if(event->modifiers()==Qt::AltModifier){//alt 进入块状态
        if(t->chunk==false&&state!=state_initial){
            t->cursor_virtual=t->cursor;
            t->chunk=true;
        }
    }
    if(event->modifiers()==Qt::ControlModifier){//ctrl+快捷键

        switch(event->key()){

        case Qt::Key_S://保存
            this->on_actionSave_triggered();
            t->cursor_virtual=t->cursor;
            t->chunk=false;
            break;
        case Qt::Key_N://新建
            this->on_actionNew_triggered();
            break;
        case Qt::Key_F://查找
            if(state!=state_initial){
                this->on_actionfind_triggered();
                t->cursor_virtual=t->cursor;
                t->chunk=false;
            }
            break;
        case Qt::Key_R://替换
            if(state!=state_initial){
                this->on_actionreplace_triggered();
                t->cursor_virtual=t->cursor;
                t->chunk=false;
            }
            break;
        case Qt::Key_O://打开
            this->on_actionOpen_triggered();
            t->cursor_virtual=t->cursor;
            t->chunk=false;
            break;
        case Qt::Key_Q://退出
            this->on_actionexit_triggered();
            break;
        case Qt::Key_Comma://移动到本行开头
            t->cursor_virtual=t->cursor;
            t->chunk=false;
            t->MoveCursor_to_start();
            break;
        case Qt::Key_Period://移动到本行结尾
            t->cursor_virtual=t->cursor;
            t->chunk=false;
            t->MoveCursor_to_end();
            break;
        case Qt::Key_X://剪切
            t->Cut_at_Cursor();
            state=state_editing;
            t->cursor_virtual=t->cursor;
            t->chunk=false;
            break;
        case Qt::Key_C://复制
            t->BlokCopy_at_Cursor();
            break;
        case Qt::Key_V://粘贴
            t->Insert_at_Cursor(t->block.block_string);
            state=state_editing;
            t->cursor_virtual=t->cursor;
            t->chunk=false;
            break;
        }
    }

    switch(event->key()){//常规热键
    case Qt::Key_F1://使用说明
        this->on_actioninstruction_triggered();
        break;
    }
  //  graphic_datastruct();
}
//松开键盘
void MainWindow::keyReleaseEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Alt)//松开alt结束块状态
        t->chunk=false;
}
//真实光标是否在虚拟光标后面
int Text::isCursorBehind(){//真实光标在后返回1  真实光标在前返回-1  两者重叠返回0
    if(cursor.line>cursor_virtual.line)
        return 1;
    else if(cursor.line<cursor_virtual.line)
        return -1;
    else{
        if(cursor.position>cursor_virtual.position)
            return 1;
        else if(cursor.position<cursor_virtual.position)
            return -1;
        else
            return 0;
    }
}
//高亮块选中
void MainWindow::highlightChunk(Cursor cursor1,Cursor cursor2){//cursor1 在前 cursor2在后

    QString s;
    s=label[cursor1.line-1]->text();
    s.insert(cursor1.Chinese+cursor1.English,"<font color=red>");
    if(cursor1.line==cursor2.line){
        s.insert(cursor2.Chinese+cursor2.English+16,"</font>");
        label[cursor1.line-1]->setText(s);
    }else{
        s+="</font>";
        label[cursor1.line-1]->setText(s);
        for(int i=cursor1.line;i<cursor2.line-1;i++){
            s=label[i]->text();
            s="<font color=red>"+s;
            //s=+"</font>";
            label[i]->setText(s);
        }
        s=label[cursor2.line-1]->text();
        s="<font color=red>"+s;
        s.insert(cursor2.Chinese+cursor2.English+16,"</font>");
        label[cursor2.line-1]->setText(s);
    }
}
