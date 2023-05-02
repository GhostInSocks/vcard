#include <iostream>
#include "vcard.h"
#include "qrcode.h"

using namespace std;

void addPerson(Vcard* card) {
    string prefix, given_name, middle_name, family_name, suffix;

    cout << "Napisite prefiksi cez vejico, ce imate, npr. Dr., PhD.: ";
    getline(cin, prefix);
    cout << "Napisite ime: ";
    getline(cin, given_name);
    cout << "Napisite srednje ime, ce imate: ";
    getline(cin, middle_name);
    cout << "Napisite priimek: ";
    getline(cin, family_name);
    cout << "Napisite sufiksi cez vejico, ce imate, npr. Jr., Esq.: ";
    getline(cin, suffix);
    card->addPerson(prefix, given_name, middle_name, family_name, suffix);
}

bool addBday(Vcard* card) {
    string bday;
    bool ok;

    cout << "Napisite datum rojstnega dneva (dd.mm.yyyy): ";
    getline(cin, bday);
    ok = card->addBday(bday);
    if (!ok) {
        cout << "Napacni vnos!" << endl;
        addBday(card);
    }

    return ok;
}

bool addZip(Vcard* card, string street_house, string city, string state,
            string country) {
    string zip;
    bool ok;

    cout << "Napisite postno stevilko: ";
    getline(cin, zip);
    ok = card->addAddress(street_house, city, state, zip, country);
    if (!ok) {
        cout << "Napacna postna stevilka!\n";
        addZip(card, street_house, city, state, country);
    }

    return ok;
}

void addAddress(Vcard* card) {
    string street_house, city, state, country;

    cout << "Napisite ulico in hisno stevilko: ";
    getline(cin, street_house);
    cout << "Napisite mesto: ";
    getline(cin, city);
    cout << "Napisite regijo: ";
    getline(cin, state);
    cout << "Napisite drzavo: ";
    getline(cin, country);
    addZip(card, street_house, city, state, country);
}

bool addHomePhone(Vcard* card) {
    string phone;
    bool ok;

    cout << "Napisite osebno telefonsko stevilko, npr. +386 40 123 456: ";
    getline(cin, phone);
    ok = card->addHomePhone(phone);
    if (!ok) {
        cout << "Napacni vnos!" << endl;
        addHomePhone(card);
    }
    return ok;
}

bool addWorkPhone(Vcard* card) {
    string phone;
    bool ok;

    cout << "Napisite delovno telefonsko stevilko, npr. +386 40 123 456 ext.1234: ";
    getline(cin, phone);
    ok = card->addWorkPhone(phone);
    if (!ok) {
        cout << "Napacni vnos!" << endl;
        addHomePhone(card);
    }

    return ok;
}

void addBusiness(Vcard* card) {
    string organization, function;

    cout << "Napisite naziv podjetja: ";
    getline(cin, organization);
    cout << "Napisite naziv delovnega mesta: ";
    getline(cin, function);
    card->addBussiness(organization, function);
}

void addNote(Vcard* card) {
    string note;

    cout << "Napisite komentar: ";
    getline(cin, note);
    card->addNote(note);
}

bool addEmail(Vcard* card) {
    string mail;
    bool ok;

    cout << "Napisite email, npr. johndoe@hotmail.com: ";
    getline(cin, mail);
    ok = card->addEmail(mail);
    if(!ok) {
        cout << "Napacen vnos!" << endl;
        addEmail(card);
    }

    return ok;
}

bool addUrl(Vcard* card) {
    string link;
    bool ok;

    cout << "Napisite url, npr. http://www.johndoe.com: ";
    getline(cin, link);
    ok = card->addUrl(link);
    if (!ok) {
        cout << "Napacen vnos!" << endl;
        addUrl(card);
    }

    return ok;
}

bool addPhoto(Vcard* card) {
    string photo;
    bool ok;

    cout << "Dodajte foto, napisite url ali pot do datoteke: ";
    getline(cin, photo);
    ok = card->addPhoto(photo);
    if (!ok) {
        cout << "Napacni url ali datoteka ni najdena!" << endl;
        addPhoto(card);
    }

    return ok;
}

bool saveCard(Vcard* card) {
    string file_name;
    bool ok;

    cout << "Napisite polno ime datoteke, npr. C:/my_vcard.vcf ali pustite prazno: ";
    getline(cin, file_name);
    ok = card->saveVCF(file_name);
    if (!ok) {
        cout << "Datoteka ze obstaja ali napacno ime datoteke!" << endl;
        saveCard(card);
    }

    return ok;
}

bool savePNG(QRCode* png, Vcard* card) {
    string file_name;
    bool ok;

    cout << "Napisite polno ime slike, npr. C:/my_qr.png ali pustite prazno: ";
    getline(cin, file_name);
    ok = png->savePNG(card->createVCF(true), 300, file_name);
    if (!ok) {
        cout << "Datoteka ze obstaja ali napacno ime datoteke!" << endl;
        savePNG(png, card);
    }

    return ok;
}

int main(){
    Vcard myvcard;
    QRCode myQR;

    addPerson(&myvcard);
    addBday(&myvcard);
    addAddress(&myvcard);
    addHomePhone(&myvcard);
    addWorkPhone(&myvcard);
    addBusiness(&myvcard);
    addNote(&myvcard);
    addEmail(&myvcard);
    addUrl(&myvcard);
    addPhoto(&myvcard);
    saveCard(&myvcard);
    savePNG(&myQR, &myvcard);
    myQR.showImage(myvcard.createVCF(true), 600);

    return 0;
}
