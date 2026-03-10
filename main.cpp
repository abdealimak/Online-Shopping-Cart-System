#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

/*
=========================================================
    ABSTRACT BASE CLASS : Product

    Serves as the blueprint for all products in the store.
    Stores common attributes (id, name, price) as protected
    so derived classes can access them directly.

    Pure virtual functions getGST(), getCategory(), and
    displayDetails() force every derived class to provide
    its own implementation — this is how runtime polymorphism
    is achieved in this program.
=========================================================
*/
class Product {
protected:
    int    id;
    char   name[40];
    double price;

public:
    Product() : id(0), price(0.0) { name[0] = '\0'; }

    Product(int id, const char* n, double price) {
        this->id    = id;
        this->price = price;
        strncpy(this->name, n, 39);
        this->name[39] = '\0';
    }

    int         getId()    const { return id;    }
    const char* getName()  const { return name;  }
    double      getPrice() const { return price; }

    virtual double      getGST()         const = 0;
    virtual const char* getCategory()    const = 0;
    virtual void        displayDetails() const = 0;

    virtual ~Product() {}
};

/*
=========================================================
    DERIVED CLASS : Electronics

    Inherits from Product and represents electronic items
    such as laptops, smartphones, and monitors.
    GST is calculated at 18% of the base price, which is
    the standard GST slab for electronics in India.
=========================================================
*/
class Electronics : public Product {
public:
    Electronics(int id, const char* n, double price)
        : Product(id, n, price) {}

    double getGST() const override {
        return price * 0.18;
    }

    const char* getCategory() const override {
        return "Electronics";
    }

    void displayDetails() const override {
        printf("  | %3d | %-20s | %-11s | %10.2f |\n",
            id, name, "Electronics", price);
    }
};

/*
=========================================================
    DERIVED CLASS : Accessory

    Inherits from Product and represents peripheral items
    such as keyboards, mice, and headphones.
    Also taxed at 18% GST, same as Electronics.
=========================================================
*/
class Accessory : public Product {
public:
    Accessory(int id, const char* n, double price)
        : Product(id, n, price) {}

    double getGST() const override {
        return price * 0.18;
    }

    const char* getCategory() const override {
        return "Accessory";
    }

    void displayDetails() const override {
        printf("  | %3d | %-20s | %-11s | %10.2f |\n",
            id, name, "Accessory", price);
    }
};

/*
=========================================================
    CLASS : CartItem

    Pairs a Product pointer with a quantity. The pointer
    is polymorphic — it can point to either an Electronics
    or Accessory object, so getGST() resolves correctly
    at runtime via virtual dispatch.

    getSubtotal() computes (base price + GST) x quantity,
    giving the full amount owed for that line item.
=========================================================
*/
class CartItem {
private:
    Product* product;
    int      qty;

public:
    CartItem() : product(nullptr), qty(0) {}

    CartItem(Product* p, int q) : product(p), qty(q) {}

    Product* getProduct()  const { return product; }
    int      getQty()      const { return qty; }

    double getSubtotal() const {
        return (product->getPrice() + product->getGST()) * qty;
    }

    void addOne()    { qty++; }
    void removeOne() { if (qty > 0) qty--; }
};

/*
=========================================================
    CLASS : ShoppingCart

    Manages a fixed-size array of CartItem objects.
    Handles all cart operations: adding, removing, viewing,
    and checking out.

    addProduct() checks if the product already exists in
    the cart using findIndex(). If found, it increments
    the quantity instead of adding a duplicate row.

    removeProduct() decrements quantity by one. If quantity
    reaches zero, it shifts the array left to close the gap,
    keeping the cart array compact.

    checkout() displays the cart, asks for an optional
    discount percentage, computes the final bill, and
    saves the order to orders.txt using file handling.
=========================================================
*/
class ShoppingCart {
private:
    static const int MAX = 20;
    CartItem items[MAX];
    int      count;

    int findIndex(int pid) const {
        for (int i = 0; i < count; i++)
            if (items[i].getProduct()->getId() == pid) return i;
        return -1;
    }

public:
    ShoppingCart() : count(0) {}

    void addProduct(Product* p) {
        int idx = findIndex(p->getId());
        if (idx != -1) {
            items[idx].addOne();
        } else {
            if (count >= MAX) { printf("\n  Cart is full!\n"); return; }
            items[count++] = CartItem(p, 1);
        }
        printf("\n  >> \"%s\" added. Qty: %d\n",
            p->getName(), items[findIndex(p->getId())].getQty());
    }

    void removeProduct(int pid) {
        int idx = findIndex(pid);
        if (idx == -1) { printf("\n  Product ID %d not in cart.\n", pid); return; }

        items[idx].removeOne();
        printf("\n  >> \"%s\" qty: %d\n",
            items[idx].getProduct()->getName(), items[idx].getQty());

        if (items[idx].getQty() == 0) {
            printf("     Removed from cart.\n");
            for (int i = idx; i < count - 1; i++)
                items[i] = items[i + 1];
            count--;
        }
    }

    void viewCart() const {
        if (count == 0) { printf("\n  Cart is empty.\n"); return; }

        printf("\n");
        printf("  +-----+----------------------+-----+------------+------------+------------+\n");
        printf("  |  ID | Product Name         | Qty |      Price |        GST |   Subtotal |\n");
        printf("  +-----+----------------------+-----+------------+------------+------------+\n");

        double totalBase = 0, totalGST = 0;
        for (int i = 0; i < count; i++) {
            double base = items[i].getProduct()->getPrice() * items[i].getQty();
            double gst  = items[i].getProduct()->getGST()   * items[i].getQty();
            totalBase += base;
            totalGST  += gst;
            printf("  | %3d | %-20s | %3d | %10.2f | %10.2f | %10.2f |\n",
                items[i].getProduct()->getId(),
                items[i].getProduct()->getName(),
                items[i].getQty(),
                base, gst, base + gst);
        }

        printf("  +-----+----------------------+-----+------------+------------+------------+\n");
        printf("  |     | %-20s |     | %10.2f | %10.2f | %10.2f |\n",
            "TOTAL", totalBase, totalGST, totalBase + totalGST);
        printf("  +-----+----------------------+-----+------------+------------+------------+\n");
    }

    void checkout() {
        if (count == 0) { printf("\n  Cart is empty.\n"); return; }

        viewCart();

        double baseTotal = 0, gstTotal = 0;
        for (int i = 0; i < count; i++) {
            baseTotal += items[i].getProduct()->getPrice() * items[i].getQty();
            gstTotal  += items[i].getProduct()->getGST()   * items[i].getQty();
        }
        double subtotal = baseTotal + gstTotal;

        printf("\n  Enter discount %% (0 for none): ");
        double disc;
        cin >> disc;
        if (disc < 0 || disc > 100) {
            printf("  Invalid. Using 0%%.\n");
            disc = 0;
        }

        double discAmt = subtotal * (disc / 100.0);
        double total   = subtotal - discAmt;

        printf("\n");
        printf("  +-------------------------------------+\n");
        printf("  |  Base Total    :  Rs. %12.2f |\n", baseTotal);
        printf("  |  GST (18%%)     :  Rs. %12.2f |\n", gstTotal);
        printf("  |  Subtotal      :  Rs. %12.2f |\n", subtotal);
        if (disc > 0)
            printf("  |  Discount(%-.0f%%) :  Rs. %12.2f |\n", disc, discAmt);
        printf("  +-------------------------------------+\n");
        printf("  |  TOTAL PAYABLE :  Rs. %12.2f |\n", total);
        printf("  +-------------------------------------+\n");

        saveOrder(baseTotal, gstTotal, disc, discAmt, total);
        printf("\n  Order saved. Thank you for shopping!\n");
    }

    /*
        Appends the completed order to orders.txt.
        Each product line is saved as pipe-separated values.
        The bill summary (totals, discount, final amount)
        is written after the items, followed by "---" as
        a separator between orders.
    */
    void saveOrder(double base, double gst, double discPct,
                double discAmt, double total) const {
        ofstream file("orders.txt", ios::app);
        for (int i = 0; i < count; i++) {
            file << items[i].getProduct()->getId()       << "|"
                << items[i].getProduct()->getName()     << "|"
                << items[i].getProduct()->getCategory() << "|"
                << items[i].getQty()                    << "|"
                << items[i].getSubtotal()               << "\n";
        }
        file << "BaseTotal|"    << base    << "\n"
            << "GSTTotal|"     << gst     << "\n"
            << "Discount%|"    << discPct << "\n"
            << "DiscountAmt|"  << discAmt << "\n"
            << "TotalPayable|" << total   << "\n"
            << "---\n";
        file.close();
    }

    bool isEmpty() const { return count == 0; }
};

/*
=========================================================
    FUNCTION : displaySavedOrders

    Reads orders.txt line by line and prints all previously
    saved orders to the console. If the file does not exist
    (no orders placed yet), it shows an appropriate message.
=========================================================
*/
void displaySavedOrders() {
    ifstream file("orders.txt");
    if (!file) { printf("\n  No saved orders found.\n"); return; }

    printf("\n===== SAVED ORDERS =====\n");
    string line;
    while (getline(file, line))
        cout << "  " << line << "\n";
    file.close();
}

/*
=========================================================
    FUNCTION : showCatalog

    Displays the full product catalog in a formatted table.
    Calls displayDetails() on each Product pointer — this
    triggers virtual dispatch so the correct derived class
    method runs for each item (Electronics or Accessory).
=========================================================
*/
void showCatalog(Product* catalog[], int n) {
    printf("\n");
    printf("  +-----+----------------------+-------------+------------+\n");
    printf("  |  ID | Product Name         | Category    |      Price |\n");
    printf("  +-----+----------------------+-------------+------------+\n");
    for (int i = 0; i < n; i++)
        catalog[i]->displayDetails();
    printf("  +-----+----------------------+-------------+------------+\n");
    printf("  * Prices shown are base prices (18%% GST added at cart)\n");
}

/*
=========================================================
    MAIN

    Entry point of the program. Sets up the product catalog
    as an array of polymorphic Product pointers, then runs
    a menu-driven loop until the user chooses to exit.

    On exit, all dynamically allocated Product objects are
    deleted to free memory.
=========================================================
*/
int main() {
    const int N = 10;

    Product* catalog[N] = {
        new Electronics(1, "Laptop",              55000.00),
        new Electronics(2, "Smartphone",          22000.00),
        new Electronics(7, "Monitor 24 inch",     18000.00),
        new Electronics(8, "Webcam HD",            3000.00),
        new Electronics(9, "SSD 1TB",              7500.00),
        new Accessory  (3, "Wireless Headphones",  3500.00),
        new Accessory  (4, "USB-C Hub",            1200.00),
        new Accessory  (5, "Mechanical Keyboard",  4500.00),
        new Accessory  (6, "Gaming Mouse",         2200.00),
        new Accessory  (10, "Smart Watch",         9999.00),
    };

    ShoppingCart cart;
    int choice;

    do {
        printf("\n===== ONLINE SHOPPING CART SYSTEM =====\n");
        printf("1. Add Product\n");
        printf("2. Remove Product\n");
        printf("3. View Cart\n");
        printf("4. Checkout\n");
        printf("5. View Saved Orders\n");
        printf("6. Exit\n");
        printf("Choice: ");
        cin >> choice;

        switch (choice) {
            case 1: {
                showCatalog(catalog, N);
                printf("  Enter Product ID: ");
                int pid;
                cin >> pid;
                bool found = false;
                for (int i = 0; i < N; i++) {
                    if (catalog[i]->getId() == pid) {
                        cart.addProduct(catalog[i]);
                        found = true;
                        break;
                    }
                }
                if (!found) printf("\n  Invalid Product ID.\n");
                break;
            }
            case 2:
                cart.viewCart();
                if (!cart.isEmpty()) {
                    printf("  Enter Product ID to remove: ");
                    int pid;
                    cin >> pid;
                    cart.removeProduct(pid);
                }
                break;
            case 3: cart.viewCart();      break;
            case 4: cart.checkout();      break;
            case 5: displaySavedOrders(); break;
            case 6: printf("\n  Goodbye!\n\n"); break;
            default: printf("\n  Invalid choice.\n");
        }

    } while (choice != 6);

    for (int i = 0; i < N; i++)
        delete catalog[i];

    return 0;
}