#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonParseError>
#include <QFile>
#include <QMainWindow>
#include <QJsonArray>
#include "QJsonDocument"
#include "QJsonObject"
#include "QSignalMapper"
#include "QClipboard"
#include "QListWidgetItem"
#include "QVBoxLayout"
#include "QLineEdit"
#include "QLabel"
#include "QPushButton"
#include "QMessageBox"
#include "QList"
#include "QCryptographicHash"
#include <QSpacerItem>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    const QByteArray pin_hash_code_orig = "cdaeb1282d614772beb1e74c192bebda";

    //окно для пин кода
    QWidget *window_pin;
    QByteArray key;
    QLineEdit *pin_code_input;

    //Шифрование:Расшифровка
    const QString encrypt_file_name = "D:/sec_auto/201-331_Korotaev/Lab_1/data.json";
    const QString open_file_name = "D:/sec_auto/201-331_Korotaev/Lab_1/open_data.json";
    QFile encrypt_file;
    QString decript_pass_or_log (QString log_or_pas, int id);


    //основное окно
    QSignalMapper *mapper_log;//отслеживание кнопки копирования логина
    QSignalMapper *mapper_pas;//отслеживание кнопки копирования пароля
    QSignalMapper *mapper_del;//отслеживание кнопки удаления виджета
    QJsonDocument doc;
    QJsonObject json;
    QJsonParseError docError;
    QJsonValue credentials;
    QList<QWidget *> list_of_widgets;
    QVector<QVector<QString>> data_vector;


    //Окно добавления
    QWidget *add_data_window;
    QLineEdit * add_site_input;
    QLineEdit * add_login_input;
    QLineEdit * add_password_input;

    bool eventFilter(QObject *watched, QEvent *event);

    void show_pin_check_window();



private slots:
    void on_search_line_textChanged(const QString &arg1);
    void show_main_window();
    QWidget* create_widget(QString site, int id); //Создание виджета

    int check_pin_code();

    int show_data_log(int id);
    int show_data_pas(int id);
    int delete_wig(int id);


    void on_open_add_window_btn_clicked();
    int add_data_fun();
    int add_data_in_open_file(QString site, QString login, QString password);
    int remove_obj(QString site_name);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
