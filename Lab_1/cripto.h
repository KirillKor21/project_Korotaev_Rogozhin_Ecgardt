#include <openssl/evp.h>
#include "QDebug"
#include "QByteArray"
#include "QBuffer"
#include "QFile"
#include "QJsonParseError"
#include "QJsonObject"
#include "QJsonArray"


class cripto
{
public:
    cripto();
    ~cripto();
    QJsonDocument doc;
    QJsonObject json;
    QJsonParseError docError;
    QJsonValue credentials;

    const QString open_file_name = "D:/sec_auto/201-331_Korotaev/Lab_1/open_data.json";

    QByteArray cript_log_and_pass(QString log_pass_str);

    unsigned char *iv = (unsigned char *)"0123456789012345";

    QString decript_file(QString file, unsigned char *key);
    int encrypt_file(QString file, unsigned char *key);
    int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *iv, unsigned char *decryptext);
    int cript_error(void);
    int encrypt(unsigned char *text, int text_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext);

    QByteArray chipr_log_and_pass(QString log, QString pass, unsigned char *key);
    QByteArray chipr_data(QString data, unsigned char *key);

};

