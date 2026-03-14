#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

// Base class — holds shared product data and forces child classes
// to implement getGST, getCategory, and display
class Product {
public:
    int id;
    char name[40];
    double price;

    Product() {
        id = 0;
        price = 0;
        name[0] = '\0';
    }

    virtual double getGST() = 0;
    virtual const char* getCategory() = 0;
    virtual void display() = 0;

    virtual ~Product() {}
};

// Electronics — sets its own data directly, GST is 18%
class Electronics : public Product {
public:
    Electronics(int pid, const char* pname, double pprice) {
        id = pid;
        price = pprice;
        strncpy(name, pname, 39);
        name[39] = '\0';
    }

    double getGST() { return price * 0.18; }
    const char* getCategory() { return "Electronics"; }

    void display() {
        printf("  | %3d | %-20s | %-11s | %10.2f |\n", id, name, "Electronics", price);
    }
};

// Accessory — same structure as Electronics, GST is also 18%
class Accessory : public Product {
public:
    Accessory(int pid, const char* pname, double pprice) {
        id = pid;
        price = pprice;
        strncpy(name, pname, 39);
        name[39] = '\0';
    }

    double getGST() { return price * 0.18; }
    const char* getCategory() { return "Accessory"; }

    void display() {
        printf("  | %3d | %-20s | %-11s | %10.2f |\n", id, name, "Accessory", price);
    }
};

// Pairs a product with a quantity, and calculates its subtotal, Each CartItem represents a row in the cart
class CartItem {
public:
    Product* product;
    int qty;

    CartItem() {
        product = nullptr;
        qty = 0;
    }

    CartItem(Product* p, int q) {
        product = p;
        qty = q;
    }

    double getSubtotal() {
        return (product->price + product->getGST()) * qty;
    }

    void addOne() { qty++; }
    void removeOne() { if (qty > 0) qty--; }
};

// Manages the cart — add, remove, view, checkout, and save orders
class ShoppingCart {
public:
    static const int MAX = 20;
    CartItem items[MAX];
    int count;

    ShoppingCart() { count = 0; }

    // Searches cart by product ID, returns index or -1 if not found
    int findIndex(int pid) {
        for (int i = 0; i < count; i++) {
            if (items[i].product->id == pid) return i;
        }
        return -1;
    }

    // Adds product to cart, or bumps quantity if already present
    void addProduct(Product* p) {
        int idx = findIndex(p->id);
        if (idx != -1) {
            items[idx].addOne();
        } else {
            if (count >= MAX) { printf("\n  Cart is full!\n"); return; }
            items[count] = CartItem(p, 1);
            count++;
        }
        printf("\n  >> \"%s\" added. Qty: %d\n", p->name, items[findIndex(p->id)].qty);
    }

    // Reduces quantity by one, removes item entirely if qty hits zero
    void removeProduct(int pid) {
        int idx = findIndex(pid);
        if (idx == -1) { printf("\n  Product ID %d not in cart.\n", pid); return; }

        items[idx].removeOne();
        printf("\n  >> \"%s\" qty: %d\n", items[idx].product->name, items[idx].qty);

        if (items[idx].qty == 0) {
            printf("     Removed from cart.\n");
            for (int i = idx; i < count - 1; i++) items[i] = items[i + 1];
            count--;
        }
    }

    // Prints all cart items with price, GST, and subtotal columns
    void viewCart() {
        if (count == 0) { printf("\n  Cart is empty.\n"); return; }

        printf("\n");
        printf("  +-----+----------------------+-----+------------+------------+------------+\n");
        printf("  |  ID | Product Name         | Qty |      Price |        GST |   Subtotal |\n");
        printf("  +-----+----------------------+-----+------------+------------+------------+\n");

        double totalBase = 0, totalGST = 0;
        for (int i = 0; i < count; i++) {
            double base = items[i].product->price * items[i].qty;
            double gst = items[i].product->getGST() * items[i].qty;
            totalBase += base;
            totalGST += gst;
            printf("  | %3d | %-20s | %3d | %10.2f | %10.2f | %10.2f |\n",
                items[i].product->id, items[i].product->name, items[i].qty, base, gst, base + gst);
        }

        printf("  +-----+----------------------+-----+------------+------------+------------+\n");
        printf("  |     | %-20s |     | %10.2f | %10.2f | %10.2f |\n",
            "TOTAL", totalBase, totalGST, totalBase + totalGST);
        printf("  +-----+----------------------+-----+------------+------------+------------+\n");
    }

    // Shows bill, applies discount, prints final total, saves to file
    void checkout() {
        if (count == 0) { printf("\n  Cart is empty.\n"); return; }

        viewCart();

        double baseTotal = 0, gstTotal = 0;
        for (int i = 0; i < count; i++) {
            baseTotal += items[i].product->price * items[i].qty;
            gstTotal += items[i].product->getGST() * items[i].qty;
        }
        double subtotal = baseTotal + gstTotal;

        printf("\n  Enter discount %% (0 for none): ");
        double disc;
        cin >> disc;
        if (disc < 0 || disc > 100) { printf("  Invalid. Using 0%%.\n"); disc = 0; }

        double discAmt = subtotal * (disc / 100.0);
        double total = subtotal - discAmt;

        printf("\n");
        printf("  +-------------------------------------+\n");
        printf("  |  Base Total    :  Rs. %12.2f |\n", baseTotal);
        printf("  |  GST (18%%)     :  Rs. %12.2f |\n", gstTotal);
        printf("  |  Subtotal      :  Rs. %12.2f |\n", subtotal);
        if (disc > 0) printf("  |  Discount(%-.0f%%) :  Rs. %12.2f |\n", disc, discAmt);
        printf("  +-------------------------------------+\n");
        printf("  |  TOTAL PAYABLE :  Rs. %12.2f |\n", total);
        printf("  +-------------------------------------+\n");

        saveOrder(baseTotal, gstTotal, disc, discAmt, total);
        printf("\n  Order saved. Thank you for shopping!\n");
    }

    // Appends order items and bill summary to orders.txt
    void saveOrder(double base, double gst, double discPct, double discAmt, double total) {
        ofstream file("orders.txt", ios::app);
        for (int i = 0; i < count; i++) {
            file << items[i].product->id << "|"
                << items[i].product->name << "|"
                << items[i].product->getCategory() << "|"
                << items[i].qty << "|"
                << items[i].getSubtotal() << "\n";
        }
        file << "BaseTotal|" << base << "\n"
            << "GSTTotal|" << gst << "\n"
            << "Discount%|" << discPct << "\n"
            << "DiscountAmt|" << discAmt << "\n"
            << "TotalPayable|" << total << "\n"
            << "---\n";
        file.close();
    }

    bool isEmpty() { return count == 0; }
};

// Reads and prints all previously saved orders from orders.txt
void displaySavedOrders() {
    ifstream file("orders.txt");
    if (!file) { printf("\n  No saved orders found.\n"); return; }
    printf("\n===== SAVED ORDERS =====\n");
    string line;
    while (getline(file, line)) cout << "  " << line << "\n";
    file.close();
}

// Prints the product catalog as a formatted table
void showCatalog(Product* catalog[], int n) {
    printf("\n");
    printf("  +-----+----------------------+-------------+------------+\n");
    printf("  |  ID | Product Name         | Category    |      Price |\n");
    printf("  +-----+----------------------+-------------+------------+\n");
    for (int i = 0; i < n; i++) catalog[i]->display();
    printf("  +-----+----------------------+-------------+------------+\n");
    printf("  * Prices shown are base prices (18%% GST added at cart)\n");
}

int main() {
    const int N = 10;

    // Catalog of all available products
    Product* catalog[N] = {
        new Electronics(1, "Laptop", 55000.00),
        new Electronics(2, "Smartphone", 22000.00),
        new Electronics(7, "Monitor 24 inch", 18000.00),
        new Electronics(8, "Webcam HD", 3000.00),
        new Electronics(9, "SSD 1TB", 7500.00),
        new Accessory(3, "Wireless Headphones", 3500.00),
        new Accessory(4, "USB-C Hub", 1200.00),
        new Accessory(5, "Mechanical Keyboard", 4500.00),
        new Accessory(6, "Gaming Mouse", 2200.00),
        new Accessory(10, "Smart Watch", 9999.00),
    };

    ShoppingCart cart;
    int choice;

    // Main menu loop — runs until user chooses Exit
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
                int pid; cin >> pid;
                bool found = false;
                for (int i = 0; i < N; i++) {
                    if (catalog[i]->id == pid) {
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
                    int pid; cin >> pid;
                    cart.removeProduct(pid);
                }
                break;
            case 3: cart.viewCart(); break;
            case 4: cart.checkout(); break;
            case 5: displaySavedOrders(); break;
            case 6: printf("\n  Goodbye!\n\n"); break;
            default: printf("\n  Invalid choice.\n");
        }
    } while (choice != 6);

    // Free all dynamically allocated product memory
    for (int i = 0; i < N; i++) delete catalog[i];

    return 0;
}