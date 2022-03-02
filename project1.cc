//// Copyright 2022 Wook-shin Han
//
//int main(int argc, char** argv) {
//	/* fill this */
//	return 0;
//}


#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// trim from left
inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v") {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v") {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string &trim(std::string &s, const char *t = " \t\n\r\f\v") {
    return ltrim(rtrim(s, t), t);
}

class Customer {
public:
    enum SEX { MALE, FEMALE };

    static SEX convertToSex(const string &str) {
        if (str == "m")
            return SEX::MALE;
        else if (str == "f")
            return SEX::FEMALE;
        else
            throw std::invalid_argument("not Sex String Value");
    }

private:
    string uname;
    string passwd;
    string lname;
    string fname;
    string addr;
    int zone;
    SEX sex;
    int age;
    int limit;
    float balance;
    string creaditCard;
    string email;
    bool active;

public:
    static const int PROPERTY_COUNT = 13;

    bool isLivingIn(int zone) { return this->zone == zone; }

    bool isActive() { return this->active; }

    string getLname() { return this->lname; }

    Customer(string uname, string passwd, string lname, string fname, string addr,
             int zone, SEX sex, int age, int limit, float balance,
             string creaditCard, string email, bool active) {
        this->uname = uname;
        this->passwd = passwd;
        this->lname = lname;
        this->fname = fname;
        this->addr = addr;
        this->zone = zone;
        this->sex = sex;
        this->age = age;
        this->limit = limit;
        this->balance = balance;
        this->creaditCard = creaditCard;
        this->email = email;
        this->active = active;
    }
};

class Zonecost {
private:
    int zoneId;
    string zoneDesc;
    float price;

public:
    static const int PROPERTY_COUNT = 3;

    bool isZoneDescSameWith(const string &zoneDesc) {
        return this->zoneDesc == zoneDesc;
    }

    int getZoneId() const { return this->zoneId; }

    Zonecost(int zoneId, string zoneDesc, float price) {
        this->zoneId = zoneId;
        this->zoneDesc = zoneDesc;
        this->price = price;
    }
};

class LineItem {
private:
    string uname;
    string date;
    string time;
    string barcode;
    int quantity;
    float price;

public:
    string getUname() { return this->uname; }

    string getBarcode() { return this->barcode; }

    static const int PROPERTY_COUNT = 6;

    LineItem(string uname, string date, string time, string barcode, int quantity,
             float price) {
        this->uname = uname;
        this->date = date;
        this->time = time;
        this->barcode = barcode;
        this->quantity = quantity;
        this->price = price;
    }
};

class ProductA {
private:
    string barcode;
    float price;
    string prodDesc;
    string mfg;
    string supplier;
    bool taxable;
    string category;
    float salePercent;

public:
    static const int PROPERTY_COUNT = 8;

    string getBarcode() { return this->barcode; }

    string getProdDesc() { return this->prodDesc; }

    ProductA(string barcode, float price, string prodDesc, string mfg,
             string supplier, bool taxable, string category, float salePercent) {
        this->barcode = barcode;
        this->price = price;
        this->prodDesc = prodDesc;
        this->mfg = mfg;
        this->supplier = supplier;
        this->taxable = taxable;
        this->category = category;
        this->salePercent = salePercent;
    }
};

list<Zonecost> getZonecostList(ifstream &zonecostFile);

list<Customer> getCustomerList(ifstream &customerFile);

list<LineItem> getLineItemList(ifstream &lineItemFile);

list<ProductA> getProductAList(ifstream &productAFile);

void query1(char *const *argv);

void query2(char *const *argv);

map<string, list<string>> getBarcodeToUserListMap(list<LineItem> &lineItemList);

list<string> getResultBarcodeList(
        map<string, list<string>> &barcodeToPurchaseCountMap);

void printResultBarcodeList(list<ProductA> &productAList,
                            list<string> &resultBarcodeList);

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "q1") == 0) {
        query1(argv);
    } else if (strcmp(argv[1], "q2") == 0) {
        query2(argv);
    } else {
        throw invalid_argument("There is no function with that query");
    }

    return 0;
}

// Query #1:
// List all the last names (LNAME) of the active customers that live in Toronto.
//
// You will be provided with a customer and zonecost tables in separate files.
// So your program should be executed as :
//
//<your_binary> q1 <customer.file> <zonecost.file>
//
// ex) ./a.out q1 customer.txt zonecost.txt (on your local directory)
//
//
void query1(char *const *argv) {
    const string specificZoneDesc = "Toronto";
    ifstream customerFile(argv[2]);
    ifstream zonecostFile(argv[3]);

    list<Zonecost> zonecostList = getZonecostList(zonecostFile);
    list<Customer> customerList = getCustomerList(customerFile);

    auto zonecostIter = find_if(
            begin(zonecostList), end(zonecostList),
            [&](Zonecost p) { return p.isZoneDescSameWith(specificZoneDesc); });
    if (zonecostIter == end(zonecostList)) {
        throw invalid_argument("There is no zone with that zoneDesc");
    }

    auto customerIter = customerList.begin();
    for (customerIter = customerList.begin(); customerIter != customerList.end();
         customerIter++) {
        if (customerIter->isLivingIn(zonecostIter->getZoneId()) &&
            customerIter->isActive()) {
            cout << customerIter->getLname() << endl;
        }
    }
}

list<Customer> getCustomerList(ifstream &customerFile) {
    list<Customer> customerList;
    if (customerFile.is_open()) {
        string line;

        unsigned long propertyRangeArray[Customer::PROPERTY_COUNT];
        unsigned long lineCount = 0;
        while (getline(customerFile, line)) {
            lineCount++;

            if (lineCount == 2) {
                istringstream lineStream(line);
                string token;

                int propertyRangeIndex = 0;
                while (getline(lineStream, token, ' ')) {
                    unsigned long size = token.size();
                    propertyRangeArray[propertyRangeIndex++] = size;
                }
            } else if (lineCount > 2) {
                string propertyArray[Customer::PROPERTY_COUNT];
                unsigned long lineCurrentIndex = 0;
                unsigned long propertyArrayIndex = 0;

                for (unsigned long propertyIndex : propertyRangeArray) {
                    propertyArray[propertyArrayIndex++] =
                            line.substr(lineCurrentIndex, propertyIndex);
                    lineCurrentIndex += (propertyIndex + 1);
                }

                string uname = trim(propertyArray[0]);
                string passwd = trim(propertyArray[1]);
                string lname = trim(propertyArray[2]);
                string fname = trim(propertyArray[3]);
                string addr = trim(propertyArray[4]);

                int zone = stoi(trim(propertyArray[5]));

                string sexString = trim(propertyArray[6]);

                int age = stoi(trim(propertyArray[7]));
                int limit = stoi(trim(propertyArray[8]));
                float balance = stof(trim(propertyArray[9]));
                string creaditCard = trim(propertyArray[10]);
                string email = trim(propertyArray[11]);
                bool active;
                istringstream(trim(propertyArray[12])) >> active;

                Customer customer(uname, passwd, lname, fname, addr, zone,
                                  Customer::convertToSex(sexString), age, limit,
                                  balance, creaditCard, email, active);
                customerList.push_back(customer);
            }
        }
        customerFile.close();
    }
    return customerList;
}

list<Zonecost> getZonecostList(ifstream &zonecostFile) {
    list<Zonecost> zonecostList;
    if (zonecostFile.is_open()) {
        string line;
        unsigned long propertyRangeArray[Zonecost::PROPERTY_COUNT];
        unsigned long lineCount = 0;

        while (getline(zonecostFile, line)) {
            lineCount++;
            if (lineCount == 2) {
                istringstream lineStream(line);
                string token;

                int propertyRangeIndex = 0;
                while (getline(lineStream, token, ' ')) {
                    unsigned long size = token.size();
                    propertyRangeArray[propertyRangeIndex++] = size;
                }
            } else if (lineCount > 2) {
                string propertyArray[Zonecost::PROPERTY_COUNT];
                unsigned long lineCurrentIndex = 0;
                unsigned long propertyArrayIndex = 0;

                for (unsigned long propertyIndex : propertyRangeArray) {
                    propertyArray[propertyArrayIndex++] =
                            line.substr(lineCurrentIndex, propertyIndex);
                    lineCurrentIndex += (propertyIndex + 1);
                }

                int zoneId = stoi(trim(propertyArray[0]));
                string zoneDesc = trim(propertyArray[1]);
                float price = stof(trim(propertyArray[2]));
                Zonecost zonecost(zoneId, zoneDesc, price);
                zonecostList.push_back(zonecost);
            }
        }
        zonecostFile.close();
    }
    return zonecostList;
}

list<LineItem> getLineItemList(ifstream &lineItemFile) {
    list<LineItem> lineItemList;
    if (lineItemFile.is_open()) {
        string line;

        unsigned long propertyRangeArray[LineItem::PROPERTY_COUNT];
        unsigned long lineCount = 0;
        while (getline(lineItemFile, line)) {
            lineCount++;

            if (lineCount == 2) {
                istringstream lineStream(line);
                string token;

                int propertyRangeIndex = 0;
                while (getline(lineStream, token, ' ')) {
                    unsigned long size = token.size();
                    propertyRangeArray[propertyRangeIndex++] = size;
                }
            } else if (lineCount > 2) {
                string propertyArray[LineItem::PROPERTY_COUNT];
                unsigned long lineCurrentIndex = 0;
                unsigned long propertyArrayIndex = 0;

                for (unsigned long propertyIndex : propertyRangeArray) {
                    propertyArray[propertyArrayIndex++] =
                            line.substr(lineCurrentIndex, propertyIndex);
                    lineCurrentIndex += (propertyIndex + 1);
                }

                string uname = trim(propertyArray[0]);
                string date = trim(propertyArray[1]);
                string time = trim(propertyArray[2]);
                string barcode = trim(propertyArray[3]);
                int quantity = stoi(trim(propertyArray[4]));
                float price = stof(trim(propertyArray[5]));

                LineItem lineItem(uname, date, time, barcode, quantity, price);
                lineItemList.push_back(lineItem);
            }
        }
        lineItemFile.close();
    }
    return lineItemList;
}

list<ProductA> getProductAList(ifstream &productAFile) {
    list<ProductA> productAList;
    if (productAFile.is_open()) {
        string line;

        unsigned long propertyRangeArray[ProductA::PROPERTY_COUNT];
        unsigned long lineCount = 0;
        while (getline(productAFile, line)) {
            lineCount++;

            if (lineCount == 2) {
                istringstream lineStream(line);
                string token;

                int propertyRangeIndex = 0;
                while (getline(lineStream, token, ' ')) {
                    unsigned long size = token.size();
                    propertyRangeArray[propertyRangeIndex++] = size;
                }
            } else if (lineCount > 2) {
                string propertyArray[ProductA::PROPERTY_COUNT];
                unsigned long lineCurrentIndex = 0;
                unsigned long propertyArrayIndex = 0;

                for (unsigned long propertyIndex : propertyRangeArray) {
                    propertyArray[propertyArrayIndex++] =
                            line.substr(lineCurrentIndex, propertyIndex);
                    lineCurrentIndex += (propertyIndex + 1);
                }

                string barcode = trim(propertyArray[0]);
                float price = stof(trim(propertyArray[1]));
                string prodDesc = trim(propertyArray[2]);
                string mfg = trim(propertyArray[3]);
                string supplier = trim(propertyArray[4]);
                bool taxable;
                istringstream(trim(propertyArray[5])) >> taxable;
                string category = trim(propertyArray[6]);
                float salePercent = stof(trim(propertyArray[7]));

                ProductA productA(barcode, price, prodDesc, mfg, supplier, taxable,
                                  category, salePercent);
                productAList.push_back(productA);
            }
        }
        productAFile.close();
    }
    return productAList;
}

// Query #2:
//
// Output the BARCODE and the PRODDESC for each product that has been purchased
// by at least two customers.
//
// In this case you will need the lineitem and products tables, and your program
// should be executed as:
//
//<your_binary> q2 <lineitem.file> <products.file>
//
// ex) ./a.out q2 lineitem.txt products_a.txt (on your local directory)
void query2(char *const *argv) {
    ifstream lineItemFile(argv[2]);
    ifstream productAFile(argv[3]);

    list<LineItem> lineItemList = getLineItemList(lineItemFile);
    list<ProductA> productAList = getProductAList(productAFile);

    map<string, list<string>> barcodeToPurchaseCountMap =
            getBarcodeToUserListMap(lineItemList);
    list<string> resultBarcodeList =
            getResultBarcodeList(barcodeToPurchaseCountMap);
    printResultBarcodeList(productAList, resultBarcodeList);
}

void printResultBarcodeList(list<ProductA> &productAList,
                            list<string> &resultBarcodeList) {
    for (auto &productAIter : productAList) {
        if (find(resultBarcodeList.begin(), resultBarcodeList.end(),
                 productAIter.getBarcode()) != resultBarcodeList.end()) {
            cout << productAIter.getBarcode() << " " << productAIter.getProdDesc()
                 << endl;
        }
    }
}

list<string> getResultBarcodeList(
        map<string, list<string>> &barcodeToPurchaseCountMap) {
    list<string> resultBarcodeList;
    for (auto &iter : barcodeToPurchaseCountMap) {
        if (iter.second.size() >= 2) {
            resultBarcodeList.push_back(iter.first);
        }
    }
    return resultBarcodeList;
}

map<string, list<string>> getBarcodeToUserListMap(
        list<LineItem> &lineItemList) {
    map<string, list<string>> barcodeToPurchaseCountMap;
    for (auto &lineItemIter : lineItemList) {
        auto iterator = barcodeToPurchaseCountMap.find(lineItemIter.getBarcode());
        if (iterator == barcodeToPurchaseCountMap.end()) {
            list<string> unameList;
            unameList.push_back(lineItemIter.getUname());
            barcodeToPurchaseCountMap.insert({lineItemIter.getBarcode(), unameList});
        } else {
            list<string> unameList = iterator->second;

            auto unameIter =
                    find(unameList.begin(), unameList.end(), lineItemIter.getUname());
            if (unameIter == unameList.end()) {
                unameList.push_back(lineItemIter.getUname());
            }

            barcodeToPurchaseCountMap.erase(lineItemIter.getBarcode());
            barcodeToPurchaseCountMap.insert({lineItemIter.getBarcode(), unameList});
        }
    }
    return barcodeToPurchaseCountMap;
}
