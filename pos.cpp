#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <random>
using namespace std;

class Product {
private:
    int productID;
    string description;
    float price;
    int quantity;
public:
    Product(int id, string desc, float pr, int qty) : productID(id), description(desc), price(pr), quantity(qty) {}

    int getProductID() const { return productID; }
    string getDescription() const { return description; }
    float getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void updateStock(int qty) {
        quantity += qty;
    }
};

class Customer {
protected:
    string name;
    string address;
    string contact;
    string type;
    string paymentType;
public:
    Customer(string n, string addr, string cont, string typ, string payType)
        : name(n), address(addr), contact(cont), type(typ), paymentType(payType) {}

    virtual ~Customer() {} // Virtual destructor

    virtual void calcDiscount(double& totalAmount) = 0;
    virtual void calcInstallment(double totalAmount, int months) const = 0;

    string getName() const { return name; }
    string getAddress() const { return address; }
    string getContact() const { return contact; }
    string getType() const { return type; }
    string getPaymentType() const { return paymentType; }
};

class RegularCustomer : public Customer {
public:
    RegularCustomer(string n, string addr, string cont, string payType)
        : Customer(n, addr, cont, "Regular", payType) {}

    void calcDiscount(double& totalAmount) override {
        if (totalAmount > 1000) {
            totalAmount *= 0.88; // 12% discount
        }
        if (paymentType == "Cash") {
            totalAmount *= 0.95; // additional 5% discount for cash
        }
    }

    void calcInstallment(double totalAmount, int months) const override {
        if (paymentType == "Installment") {
            double downPayment = totalAmount * 0.25;
            double balance = totalAmount - downPayment;
            double monthlyPayment = balance / months;
            cout << "Down Payment: " << downPayment << endl;
            cout << "Monthly Payment for " << months << " months: " << monthlyPayment << endl;
        }
    }
};

class RetailCustomer : public Customer {
public:
    RetailCustomer(string n, string addr, string cont, string payType)
        : Customer(n, addr, cont, "Retail", payType) {}

    void calcDiscount(double& totalAmount) override {
        if (totalAmount > 1000) {
            totalAmount *= 0.88; // 12% discount
        }
    }

    void calcInstallment(double totalAmount, int months) const override {
        if (paymentType == "Installment") {
            double downPayment = totalAmount * 0.30;
            double balance = totalAmount - downPayment;
            double monthlyInterest = 0.025 * balance;
            double monthlyPayment = (balance + (monthlyInterest * months)) / months;
            cout << "Down Payment: " << downPayment << endl;
            cout << "Monthly Payment for " << months << " months (including interest): " << monthlyPayment << endl;
        }
    }
};

class Transaction {
private:
    int transactionID;
    Customer* customer;
    vector<Product> products;
    string date;
    double totalAmount;

    int generateUniqueTransactionID() {
        static int lastTransactionID = 0;
        ifstream inFile("lastTransactionID.txt");
        if (inFile.is_open()) {
            inFile >> lastTransactionID;
            inFile.close();
        }
        lastTransactionID++;
        ofstream outFile("lastTransactionID.txt");
        if (outFile.is_open()) {
            outFile << lastTransactionID;
            outFile.close();
        }
        return lastTransactionID;
    }

public:
    Transaction(Customer* cust, const vector<Product>& prods)
        : customer(cust), products(prods) {
        transactionID = generateUniqueTransactionID();
        totalAmount = calculateTotal();
        applyDiscount();
        time_t now = time(0);
        date = ctime(&now);
        date.pop_back(); // remove newline character from the end of the date string
    }

    double calculateTotal() const {
        double total = 0.0;
        for (const auto& prod : products) {
            total += prod.getPrice() * prod.getQuantity();
        }
        return total;
    }

    void applyDiscount() {
        customer->calcDiscount(totalAmount);
    }

    int getTransactionID() const { return transactionID; }
    string getDate() const { return date; }
    Customer* getCustomer() const { return customer; }
    const vector<Product>& getProducts() const { return products; }
    double getTotalAmount() const { return totalAmount; }

    void display() const {
        cout << "Transaction ID: " << getTransactionID() << endl;
        cout << "Date: " << getDate() << endl;
        cout << "Customer: " << customer->getName() << endl;
        cout << "Address: " << customer->getAddress() << endl;
        cout << "Contact: " << customer->getContact() << endl;
        cout << "Items: " << endl;
        for (const auto& prod : products) {
            cout << "  - " << prod.getDescription() << " (x" << prod.getQuantity() << ") @ " << prod.getPrice() << endl;
        }
        cout << "Total Amount: " << getTotalAmount() << endl;
        if (customer->getPaymentType() == "Installment") {
            int months;
            cout << "Enter number of months for installment (3, 6, 9): ";
            cin >> months;
            customer->calcInstallment(totalAmount, months);
        } else {
            cout << "Amount Due: " << getTotalAmount() << endl;
        }
    }
};

class User {
private:
    string username;
    string password;
    string role;
public:
    User(string usr, string pass, string rl)
        : username(usr), password(pass), role(rl) {}

    bool login(const string& usr, const string& pass) const {
        return (username == usr && password == pass);
    }

    string getRole() const { return role; }
};

void saveTransaction(const Transaction& trans) {
    ofstream salesFile("SalesTran.txt", ios::app);
    salesFile << "Transaction ID: " << trans.getTransactionID() << endl;
    salesFile << "Date: " << trans.getDate() << endl;
    salesFile << "Customer: " << trans.getCustomer()->getName() << endl;
    salesFile << "Address: " << trans.getCustomer()->getAddress() << endl;
    salesFile << "Contact: " << trans.getCustomer()->getContact() << endl;
    for (const auto& prod : trans.getProducts()) {
        salesFile << "  - " << prod.getDescription() << " (x" << prod.getQuantity() << ") @ " << prod.getPrice() << endl;
    }
    salesFile << "Total Amount: " << trans.getTotalAmount() << endl;
    salesFile << "----------------------------------------" << endl;
    salesFile.close();

    ofstream salesIDFile("SalesID.txt", ios::app);
    for (const auto& prod : trans.getProducts()) {
        salesIDFile << "Transaction ID: " << trans.getTransactionID() << " - Product ID: " << prod.getProductID() << " - " << prod.getDescription() << " (x" << prod.getQuantity() << ") @ " << prod.getPrice() << endl;
    }
    salesIDFile.close();
}

void saveCustomerInfo(const Customer* cust) {
    ofstream custFile("CustInfo.txt", ios::app);
    custFile << "Name: " << cust->getName() << endl;
    custFile << "Address: " << cust->getAddress() << endl;
    custFile << "Contact: " << cust->getContact() << endl;
    custFile << "Type: " << cust->getType() << endl;
    custFile << "----------------------------------------" << endl;
    custFile.close();

    if (cust->getType() == "Retail") {
        ofstream retailCustFile("CustRetail.txt", ios::app);
        retailCustFile << "Name: " << cust->getName() << endl;
        retailCustFile << "Address: " << cust->getAddress() << endl;
        retailCustFile << "Contact: " << cust->getContact() << endl;
        retailCustFile << "----------------------------------------" << endl;
        retailCustFile.close();
    } else {
        ofstream regCustFile("CustReg.txt", ios::app);
        regCustFile << "Name: " << cust->getName() << endl;
        regCustFile << "Address: " << cust->getAddress() << endl;
        regCustFile << "Contact: " << cust->getContact() << endl;
        regCustFile << "----------------------------------------" << endl;
        regCustFile.close();
    }
}

void generateReports() {
    ifstream salesFile("SalesTran.txt");
    if (!salesFile.is_open()) {
        cout << "Failed to open SalesTran.txt\n";
        return;
    }

    cout << "\nSales Transactions Report:\n";
    cout << "----------------------------------------\n";
    string line;
    while (getline(salesFile, line)) {
        cout << line << endl;
    }
    salesFile.close();

    ifstream custInfoFile("CustInfo.txt");
    if (!custInfoFile.is_open()) {
        cout << "Failed to open CustInfo.txt\n";
        return;
    }

    cout << "\nCustomer Information Report:\n";
    cout << "----------------------------------------\n";
    while (getline(custInfoFile, line)) {
        cout << line << endl;
    }
    custInfoFile.close();

    ifstream salesIDFile("SalesID.txt");
    if (!salesIDFile.is_open()) {
        cout << "Failed to open SalesID.txt\n";
        return;
    }

    cout << "\nSales ID Report:\n";
    cout << "----------------------------------------\n";
    while (getline(salesIDFile, line)) {
        cout << line << endl;
    }
    salesIDFile.close();
}

int main() {
    User salesClerk("salesclerk", "password123", "salesclerk");
    User manager("manager", "password123", "manager");

    while (true) {
        string username, password;
        cout << "Login\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        User* loggedInUser = nullptr;
        if (salesClerk.login(username, password)) {
            loggedInUser = &salesClerk;
        } else if (manager.login(username, password)) {
            loggedInUser = &manager;
        } else {
            cout << "Invalid credentials\n";
            continue;
        }

        cout << "Welcome, " << loggedInUser->getRole() << "!\n";

        if (loggedInUser->getRole() == "manager") {
            char generateReport;
            cout << "Would you like to generate reports? [Y/N]: ";
            cin >> generateReport;
            if (generateReport == 'Y' || generateReport == 'y') {
                generateReports();
            }

            char createTransaction;
            cout << "Would you like to create a new transaction? [Y/N]: ";
            cin >> createTransaction;
            if (createTransaction == 'N' || createTransaction == 'n') {
                char logout;
                cout << "Would you like to log out? [Y/N]: ";
                cin >> logout;
                if (logout == 'Y' || logout == 'y') {
                    continue; // Logs out the manager and prompts for new login
                }
            }
        }

        if (loggedInUser->getRole() == "salesclerk") {
            char createTransaction;
            cout << "Would you like to create a new transaction? [Y/N]: ";
            cin >> createTransaction;
            if (createTransaction == 'N' || createTransaction == 'n') {
                cout << "Exiting program.\n";
                break; // Exit the program
            }
        }

        char addMore;
        do {
            string customerName, address, contactNumber, customerType, paymentType;
            int productId, quantity;
            float price;

            cout << "Enter customer details:" << endl;
            cout << "Customer Name: ";
            cin.ignore();
            getline(cin, customerName);
            cout << "Address: ";
            getline(cin, address);
            cout << "Contact Number: ";
            getline(cin, contactNumber);
            cout << "Customer Type (Regular/Retail): ";
            cin >> customerType;
            cout << "Payment Type (Cash/Installment): ";
            cin >> paymentType;

            Customer* customer;
            if (customerType == "Regular") {
                customer = new RegularCustomer(customerName, address, contactNumber, paymentType);
            } else {
                customer = new RetailCustomer(customerName, address, contactNumber, paymentType);
            }

            vector<Product> products;
            do {
                cout << "\nEnter product details:" << endl;
                cout << "Product ID: ";
                cin >> productId;
                cout << "Product Description: ";
                cin.ignore();
                string description;
                getline(cin, description);
                cout << "Item Price: ";
                cin >> price;
                cout << "Quantity: ";
                cin >> quantity;

                Product product(productId, description, price, quantity);
                products.push_back(product);

                cout << "Add more products? [Y/N]: ";
                cin >> addMore;
            } while (addMore == 'Y' || addMore == 'y');

            Transaction transaction(customer, products);
            transaction.display();
            saveTransaction(transaction);
            saveCustomerInfo(customer);

            cout << "Process another transaction? [Y/N]: ";
            cin >> addMore;

            delete customer;
        } while (addMore == 'Y' || addMore == 'y');
    }

    return 0;
}
