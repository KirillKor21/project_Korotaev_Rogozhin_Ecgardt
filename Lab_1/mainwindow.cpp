#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "cripto.h"
#include <iostream>
#include <windows.h>


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent) ,  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
//

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::check_pin_code() {
    //qDebug() << pin_code_input->text();

    QByteArray pin_hash_code_current = QCryptographicHash::hash(QByteArray(pin_code_input->text().toUtf8()), QCryptographicHash::Md5).toHex(); // Хэшируем по Sha256

    if (pin_hash_code_current == pin_hash_code_orig) {
        this->key = pin_hash_code_current; //назначаем ключом хэш значение от пароля
        delete[] window_pin;

        this->show_main_window(); //запускаем основное окно
        this->show();
        return 0;

    }
    else {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Ошибка ввода пин-кода"));
        return 0;
    }
}

void MainWindow::show_pin_check_window() {

    window_pin = new QWidget; //Создание виджета как окна для ввода пин кода
    //window_pin->setBaseSize(250,100);
    window_pin->setFixedSize(250,100);
    QVBoxLayout *layout_pin = new QVBoxLayout;


    QLabel *pin_code_label = new QLabel("Пин код"); //Лейбл для пинкода
    pin_code_input = new QLineEdit(); // Инпут для пинкода
    pin_code_input->setEchoMode(QLineEdit::Password);

    QPushButton *check_pin_btn = new QPushButton("Вход"); //Кнопка для проверки
    connect(check_pin_btn, SIGNAL(clicked()), this, SLOT(check_pin_code())); //При нажатии на кнопку отправляемся в функцию проверки


    //Добавляем в окно лейбл кнопку и инпут
    layout_pin->addWidget(pin_code_label);
    layout_pin->addWidget(pin_code_input);
    layout_pin->addWidget(check_pin_btn);
    window_pin->setLayout(layout_pin);

    window_pin->show();
}

void MainWindow::show_main_window() {

    encrypt_file.setFileName(encrypt_file_name);

    cripto cript;


    //Проверка на существование файла с полностью зашифрованными файлами
    if (!encrypt_file.exists()) {
        cript.encrypt_file(encrypt_file_name, (unsigned char *)key.data()); //Шифрование файла с открытыми данными (Результат - созданный файл с зашифрованными данными)
    }

    //вызываем функцию расшифрови, отправляет имя файла и ключ(.data преобразует в char)
    QString encrypt_data = cript.decript_file(encrypt_file_name, (unsigned char *)key.data());
    //приходит файл с расшифрованными названиями сайтов но с защифрованными связками логин+пароль

    doc = QJsonDocument::fromJson(QByteArray(encrypt_data.toUtf8()), &docError);

    json = doc.object();

    credentials = json.value("credentials");

    for (int i = 0; credentials[i].isObject(); i++) {
        QJsonObject obj_temp = credentials[i].toObject();
        QVector<QString> vec_temp = {obj_temp["site"].toString(), obj_temp["data"].toString()};
        data_vector.push_back(vec_temp);
    }

    //qDebug() << data_vector;

    //Отслеживание нажатие определенной кнопки
    mapper_log = new QSignalMapper(this);
    mapper_pas = new QSignalMapper(this);
    mapper_del = new QSignalMapper(this);


    QObject::connect(mapper_log,SIGNAL(mappedInt(int)),this,SLOT(show_data_log(int))); //сигнал на кнопку логина
    QObject::connect(mapper_pas,SIGNAL(mappedInt(int)),this,SLOT(show_data_pas(int))); //сигнал на кнопку пароля
    QObject::connect(mapper_del,SIGNAL(mappedInt(int)),this,SLOT(delete_wig(int))); //сигнал на кнопку пароля

    //Добавление виджетов в общий лист виджетов
    for (int i = 0; i < data_vector.length(); i++) {

        QWidget *wig = create_widget(data_vector[i][0], i); // создание виджета
        list_of_widgets.push_back(wig);
    }


    for (int var = 0; var < list_of_widgets.length(); ++var) {
        ui->verticalLayout->addWidget(list_of_widgets[var]);
    }

}

//Функция создания виджетов
QWidget* MainWindow::create_widget(QString site, int id) {
    QHBoxLayout *layout = new QHBoxLayout;

    //Создание форм
    QLineEdit *site_name = new QLineEdit();
    QLineEdit *login = new QLineEdit();
    QLineEdit *password = new QLineEdit();

    site_name->setReadOnly(true);
    site_name->setText(site);
    //Логин
    login->setEchoMode(QLineEdit::Password);
    login->setReadOnly(true);
    login->setText("1234567890");

    //Пароль
    password->setEchoMode(QLineEdit::Password);
    password->setReadOnly(true);
    password->setText("1234567890");

    //Удаление рамок
    site_name->setStyleSheet("border: none;");
    login->setStyleSheet("border: none;");
    password->setStyleSheet("border: none;");

    //Кнопки копирования данных
    QPushButton *coppy_login_btn = new QPushButton("copy");
    QPushButton *coppy_pass_btn = new QPushButton("copy");
    QPushButton *delete_wig_btn = new QPushButton("x");


    //Размеры кнопок
    coppy_login_btn->setFixedSize(50,20);
    coppy_pass_btn->setFixedSize(50,20);
    delete_wig_btn->setStyleSheet("background-color: #AAADAD; margin: 18px;  margin-right: 0; margin-top: 0;padding-bottom: 3px; height: 12px; width: 12px");

    //Сигналы на передачу данных от кнопок
    mapper_log->setMapping(coppy_login_btn,id); //Вместе с кнопкой передаем номер цикла - это будет id
    connect(coppy_login_btn,SIGNAL(clicked()),mapper_log,SLOT(map())); //слот на передачу при нажатии

    mapper_pas->setMapping(coppy_pass_btn,id); //Вместе с кнопкой передаем номер цикла - это будет id
    connect(coppy_pass_btn,SIGNAL(clicked()),mapper_pas,SLOT(map())); //слот на передачу при нажатии

    mapper_del->setMapping(delete_wig_btn,id); //Вместе с кнопкой передаем номер цикла - это будет id
    connect(delete_wig_btn,SIGNAL(clicked()),mapper_del,SLOT(map())); //слот на передачу при нажатии


    QSpacerItem *spacer = new QSpacerItem(40, 0); //Spacer

    //добавление в layout
    layout->addWidget(site_name);
    layout->addWidget(login);
    layout->addWidget(coppy_login_btn);
    layout->addSpacerItem(spacer);
    layout->addWidget(password);
    layout->addWidget(coppy_pass_btn);
    layout->addWidget(delete_wig_btn);

    QWidget *wig = new QWidget(); // создание виджета

    //wig->installEventFilter(this);

    wig->setObjectName(QString(site)); // устанавливаем имя виджета для дальнейшего поиска
    wig->setLayout(layout); // добавление в виджет
    wig->setStyleSheet("background-color: #e1e7e7;");
    //wig->setBaseSize(455,60);
    //wig->setFixedSize(455,60);


    return wig;

}

int MainWindow::show_data_log(int id) {

    QClipboard* pcb = QApplication::clipboard();
    //Копируем в буфер обмена

    cripto str_chipr;
    QString pass = decript_pass_or_log("login",id);

    QString chipr_pass = QString(str_chipr.chipr_data(pass,(unsigned char *)key.data()));

    pcb->setText(chipr_pass, QClipboard::Clipboard);
    //Создание процесса
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdLine[] = L"C:/Users/pro10/source/repos/buffer/x64/Debug/buffer.exe";

    if(CreateProcess(cmdLine, NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
        std::cout << "Process id created !" << std::endl;
        std::cout << "Pid = " << std::dec << pi.dwProcessId << std::endl;
    }
    else {
        std::cout << "Process is not created! unlucky :( "<< std::endl;
    }

    return 0;

//    QClipboard* pcb = QApplication::clipboard();
//    //Копируем в буфер обмена
//    pcb->setText(decript_pass_or_log("login",id), QClipboard::Clipboard);

//    return 0;

}


int MainWindow::show_data_pas(int id) {

    QClipboard* pcb = QApplication::clipboard();
    //Копируем в буфер обмена

    cripto str_chipr;
    QString pass = decript_pass_or_log("password",id);

    QString chipr_pass = QString(str_chipr.chipr_data(pass,(unsigned char *)key.data()));

    pcb->setText(chipr_pass, QClipboard::Clipboard);
    //Создание процесса
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdLine[] = L"C:/Users/pro10/source/repos/buffer/x64/Debug/buffer.exe";

    if(CreateProcess(cmdLine, NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
        std::cout << "Process id created !" << std::endl;
        std::cout << "Pid = " << std::dec << pi.dwProcessId << std::endl;
    }
    else {
        std::cout << "Process is not created! unlucky :( "<< std::endl;
    }


    //    QClipboard* pcb = QApplication::clipboard();
    //    //Копируем в буфер обмена
    //    pcb->setText(decript_pass_or_log("login",id), QClipboard::Clipboard);

    //    return 0;

    return 0;
}

int MainWindow::delete_wig(int id) {

    QString name_del_site = data_vector[id][0]; // Фиксируем имя удаляемого объекта

    for (int i = 0; i < list_of_widgets.length(); ++i) {
        // Сапостовляем название виджета в листе виджетов с именем удаляемого объекта
        if(list_of_widgets[i]->objectName() == name_del_site) {
            //Удаляем виджет из лайаута из листа и из памяти
            ui->verticalLayout->removeWidget(list_of_widgets[i]);
            delete[] list_of_widgets[i];
            list_of_widgets.remove(i);
            remove_obj(name_del_site);
        }
    }

    return 0;
}


QString MainWindow::decript_pass_or_log (QString log_or_pas, int id) {

    QString log_and_pass_chipr = data_vector[id][1]; //Забираем из вектора строчку с зашифрованным логином и паролем

    QByteArray log_and_pass_chipr_bit = QByteArray::fromBase64(log_and_pass_chipr.toUtf8()); //Переводим в байты

    cripto decript_log_and_pass;

    unsigned char decrypt[256] = {0};
    int decrypt_len;


    //Расшифровываем строчку
    decrypt_len = decript_log_and_pass.decrypt((unsigned char*)log_and_pass_chipr_bit.data(),log_and_pass_chipr_bit.length(),(unsigned char *)key.data(),decript_log_and_pass.iv,decrypt);

    //Расшифрованные данные переводим в байты
    QByteArray decrypt_bit =  QByteArray::fromRawData((const char*)decrypt, decrypt_len);
    //qDebug() << decrypt_bit.data();

    //Создаем json документ
    QJsonDocument pas_log_doc = QJsonDocument::fromJson(decrypt_bit, &docError);
    QJsonObject pas_log_json = pas_log_doc.object();


    return (pas_log_json[log_or_pas].toString());
}


void MainWindow::on_search_line_textChanged(const QString &arg1)
{
    //Делаем все эллементы видимыми
    for (int var = 0; var < list_of_widgets.length(); var++) {
        list_of_widgets[var]->show();
    }

    //Скрываем эллементы, в которых не содержится введенной подстроки
    for (int var = 0; var < list_of_widgets.length(); var++) {
        if(!list_of_widgets[var]->objectName().contains(arg1)) {
            list_of_widgets[var]->hide();
        }
    }

    //qDebug() << arg1;
}


void MainWindow::on_open_add_window_btn_clicked()
{
    this->setEnabled(false);//Скрываем главное окно

    add_data_window = new QWidget; //Создание виджета как окна для добавление данных

    add_data_window->installEventFilter(this); //Устанавливаем отлавливание событий

    add_data_window->setFixedSize(300,200);
    //add_data_window->setStyleSheet(" height:25px; width:100px; ");

    QVBoxLayout *layout_add_window = new QVBoxLayout;


    //Создание лейблов
    QLabel *add_site_label = new QLabel("site");
    QLabel *add_login_label = new QLabel("login");
    QLabel *add_password_label = new QLabel("password");

    //Создание полей вводу
    add_site_input = new QLineEdit();
    add_login_input = new QLineEdit();
    add_password_input = new QLineEdit();

    //устанвока звездочек на поля ввода
    add_login_input->setEchoMode(QLineEdit::Password);
    add_password_input->setEchoMode(QLineEdit::Password);

    //Создание горизонтальных лайаутов
    QHBoxLayout *layout_for_site = new QHBoxLayout;
    QHBoxLayout *layout_for_login = new QHBoxLayout;
    QHBoxLayout *layout_for_password = new QHBoxLayout;

    //Добавление лейблов и полей ввода в лайауты
    layout_for_site->addWidget(add_site_label);
    layout_for_site->setSpacing(38);
    layout_for_site->addWidget(add_site_input);

    layout_for_login->addWidget(add_login_label);
    layout_for_login->setSpacing(30);
    layout_for_login->addWidget(add_login_input);

    layout_for_password->addWidget(add_password_label);
    //layout_for_password->setSpacing(20);
    layout_for_password->addWidget(add_password_input);

    //создание кнопки для добавления
    QPushButton *add_data_btn_ = new QPushButton("Добавить"); //Кнопка для проверки
    connect(add_data_btn_, SIGNAL(clicked()), this, SLOT(add_data_fun())); //При нажатии на кнопку отправляемся в функцию проверки

    //Заполнение основного лайаута
    layout_add_window->addLayout(layout_for_site);
    layout_add_window->addLayout(layout_for_login);
    layout_add_window->addLayout(layout_for_password);
    layout_add_window->addWidget(add_data_btn_);

    //Добавление всей информации в окно
    add_data_window->setLayout(layout_add_window);

    add_data_window->show();
}

int MainWindow::add_data_fun() {

    //Проверка на ввод нулевых данных
    if(add_site_input->text().trimmed() == nullptr || add_login_input->text().trimmed() == nullptr || add_password_input->text().trimmed() == nullptr) {

        QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Введены некорректные данные"));

        //Закрываем и удаляем окно
        add_data_window->close();

        return 0;

    }

    cripto cript_add_data;

    //Забираем информацию из форм ввода и шифруем связку логин + пароль
    QByteArray cripted_add_log_pass = cript_add_data.chipr_log_and_pass(add_login_input->text(),add_password_input->text(),(unsigned char *)key.data());

    //qDebug() << cripted_add_log_pass;

    //Новый JSON объект хранит в себе название сайта и связку зашифрованную логин + пароль
    QJsonObject new_aut_data;
    new_aut_data["site"] = add_site_input->text();
    new_aut_data["data"] = cripted_add_log_pass.data();

    //Добавляем объект в общий вектор
    QVector<QString> vec_temp = {new_aut_data["site"].toString(), new_aut_data["data"].toString()};

    data_vector.push_back(vec_temp);

    //Создаем новый виджет
    QWidget *new_wig = create_widget(new_aut_data["site"].toString(),data_vector.length() - 1); // Так как он добавлен в вектор последним, то его id будет номер последнего эллемента

    // Добавляем новый виджет в общий лист виджетов
    list_of_widgets.push_back(new_wig);

    //Добавляем новый виджет в главное окно
    ui->verticalLayout->addWidget(new_wig);

    //Добавление данных в открытый файл
    add_data_in_open_file(add_site_input->text(), add_login_input->text(), add_password_input->text());

    //Закрываем и удаляем окно
    add_data_window->close();

    return 0;

}

int MainWindow::add_data_in_open_file(QString site, QString login, QString password) {
    QJsonDocument open_file_json_doc;

    //Считываем данные из открытого файла
    QFile open_file;

    open_file.setFileName(open_file_name);

    if (open_file.open(QIODevice::ReadOnly|QFile::Text))
    {
       open_file_json_doc = QJsonDocument::fromJson(QByteArray(open_file.readAll())); //читаются данные из json
    }
    else
    {
        qDebug() << "Ошибка в открытие файла " + open_file_name;
        return 0;
    }
    open_file.close();

    QJsonObject open_file_json = open_file_json_doc.object();
    QJsonValue open_file_credentials = open_file_json.value("credentials");

    //Создание переменных для нового файла
    QJsonObject new_open_file_object;
    QJsonArray new_open_file_arr;
    QJsonObject new_open_file_inner_object;

    //Добавление аут данных
    new_open_file_inner_object.insert("site", site);
    new_open_file_inner_object.insert("login", login);
    new_open_file_inner_object.insert("password", password);

    //Копирование старых данных
    for (int i = 0; open_file_credentials[i].isObject(); i++) {
        new_open_file_arr.push_back(open_file_credentials[i]);
    }

    //Добавляем новыые данные
    new_open_file_arr.push_back(new_open_file_inner_object);

    new_open_file_object.insert("credentials", new_open_file_arr);

    //Создаем новый документ
    QJsonDocument open_file_json_doc_1(new_open_file_object);

    //Добавляем перезаписанные данные в файл
    QFile new_file(open_file_name);

    if (new_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&new_file);
        out << open_file_json_doc_1.toJson(QJsonDocument::Indented);
        new_file.close();
        //qDebug() << "Новый файл с добавленными данными успешно сохранен";
    } else {
        //qDebug() << "Не удалось сохранить новый файл с добавленными данными";
    }


    QByteArray encrypt_file_name_bit = encrypt_file_name.toUtf8();

    //Удаляем зашифрованный файл (чтобы при следующем запуске программа создала новый из измененного)
    remove((const char*)encrypt_file_name_bit);

    return 0;
}

int MainWindow::remove_obj(QString name_del_site) {
    QJsonDocument open_file_json_doc;

    //Считываем данные из открытого файла
    QFile open_file;

    open_file.setFileName(open_file_name);

    if (open_file.open(QIODevice::ReadOnly|QFile::Text))
    {
       open_file_json_doc = QJsonDocument::fromJson(QByteArray(open_file.readAll())); //читаются данные из json
    }
    else
    {
        qDebug() << "Ошибка в открытие файла " + open_file_name;
        return 0;
    }
    open_file.close();

    QJsonObject open_file_json = open_file_json_doc.object();
    // Получение массива объектов учетных данных
    QJsonArray cred_arr = open_file_json["credentials"].toArray();


    //удаляем из общего массива данные
    for(int i = 0; i < cred_arr.size(); ++i) {
        QJsonObject obj = cred_arr[i].toObject();
        if(obj["site"].toString() == name_del_site) {
            cred_arr.removeAt(i);
            break;
        }
    }

    //создаем новый json документ
    QJsonObject new_root_obj;
    new_root_obj["credentials"] = cred_arr;
    QJsonDocument new_doc(new_root_obj);


    //Сохраняем в файл
    QFile new_file(open_file_name);

    if (new_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&new_file);
        out << new_doc.toJson(QJsonDocument::Indented);
        new_file.close();
        //qDebug() << "Новый файл с удаленными данными успешно сохранен";
    } else {
        //qDebug() << "Не удалось сохранить новый файл с удаленными данными";
    }

    QByteArray encrypt_file_name_bit = encrypt_file_name.toUtf8();

    //Удаляем зашифрованный файл (чтобы при следующем запуске программа создала новый из измененного)
    remove((const char*)encrypt_file_name_bit);

    return 0;

}



// событие на закрытие окна добавления данных
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {

    if (event->type() == QEvent::Close) {

        //Удаляем окно добавления
        delete[] add_data_window;

        //Делаем вновь активным главное окно
        this->setEnabled(true);

        return true;
    }
    else {
        QObject::eventFilter(watched, event);
        return false;
    }
}

