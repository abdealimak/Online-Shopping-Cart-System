# 🛒 Online Shopping Cart System

A console-based **Online Shopping Cart System** built in **C++** that simulates a real-world e-commerce shopping experience — complete with a product catalog, cart management, GST calculation, discount handling, and persistent order saving via file handling.

---

## 📌 About the Project

This project models a simplified online store where users can browse a catalog of products, add or remove items from their cart, view itemized bills with GST applied, apply discount coupons at checkout, and save completed orders to a file. It is designed to demonstrate core **Object-Oriented Programming** principles in C++ through a practical, menu-driven application.

---

## ✨ Features

- 📦 **Product Catalog** — 10 pre-loaded products across two categories: *Electronics* and *Accessories*
- 🛍️ **Add to Cart** — Add any product by ID; duplicate entries increment quantity automatically
- ❌ **Remove from Cart** — Decrease quantity by one; item is removed when quantity reaches zero
- 🧾 **View Cart** — Displays a formatted table with base price, GST, and subtotal per item
- 💰 **Checkout** — Computes GST (18%), applies optional discount, and shows the final payable amount
- 💾 **Order History** — Saves every completed order to `orders.txt` and allows viewing past orders
- 🔁 **Menu-Driven Interface** — Simple numbered menu loop for smooth navigation

---

## 🗂️ Project Structure

```
OnlineShoppingCart/
├── 📁 Documentations
│   └── 📕 AbdealiMakda_OnlineShoppingCartSystem_CaseStudyReport.pdf
├── ⚙️ .gitignore
├── 📝 README.md
└── ⚡ main.cpp
```

### Class Overview

```
Product          (Abstract Base Class)
├── Electronics  (Derived Class — 18% GST)
└── Accessory    (Derived Class — 18% GST)

CartItem         (Associates a Product* with a quantity)
ShoppingCart     (Manages cart operations and checkout)
```

---

## 🧠 C++ & OOP Concepts Used

### 1. 🔷 Classes & Objects
Every entity — `Product`, `Electronics`, `Accessory`, `CartItem`, and `ShoppingCart` — is modelled as a class with data members and member functions.

### 2. 🔷 Abstraction
`Product` is an **abstract base class** with three pure virtual functions:
```cpp
virtual double      getGST()      = 0;
virtual const char* getCategory() = 0;
virtual void        display()     = 0;
```
This enforces a contract — every derived class *must* provide its own implementation.

### 3. 🔷 Inheritance
`Electronics` and `Accessory` both **inherit** from `Product`, reusing the common attributes (`id`, `name`, `price`) while providing their own category and display behaviour.

### 4. 🔷 Polymorphism (Runtime)
The product catalog is stored as `Product* catalog[N]` — an array of **base class pointers**. When `display()` or `getGST()` is called, **virtual dispatch** ensures the correct derived class method runs at runtime.
```cpp
catalog[i]->display();   // Calls Electronics::display or Accessory::display
```

### 5. 🔷 Encapsulation
Data and behaviour are bundled together inside classes. Members like `id`, `name`, `price`, and `qty` are declared `public` — an intentional simplification that removes the need for getter functions while still keeping all logic encapsulated within the class structure.

### 6. 🔷 Virtual Destructor
`Product` declares a `virtual ~Product()` to ensure that when a derived object is deleted through a base class pointer, the correct destructor chain is called — preventing memory leaks.

### 7. 🔷 Dynamic Memory Allocation
Products are created on the heap using `new` and cleaned up with `delete` at program exit, demonstrating manual memory management.
```cpp
new Electronics(1, "Laptop", 55000.00);
// ...
delete catalog[i];
```

### 8. 🔷 File Handling
Completed orders are **appended** to `orders.txt` using `ofstream` in `ios::app` mode. Past orders are read back using `ifstream` and displayed line by line.

### 9. 🔷 Static Class Constant
`ShoppingCart` uses `static const int MAX = 20` to define the cart capacity — a compile-time constant scoped to the class.

---

## ⚙️ How It Works

### Step 1 — Browse Catalog
Select option **1** from the menu. The full product catalog is displayed as a formatted table showing ID, name, category, and base price.

### Step 2 — Add Products
Enter the Product ID to add it to your cart. If the same product is added again, the quantity is incremented rather than creating a duplicate entry.

### Step 3 — View Cart
Select option **3** to see your current cart with per-item breakdown:
- **Base Price × Qty**
- **GST (18%) × Qty**
- **Subtotal per item**
- **Grand total row** at the bottom

### Step 4 — Remove Products
Select option **2** to reduce the quantity of any item by 1. Once quantity hits zero, the item is automatically removed and the array is compacted.

### Step 5 — Checkout
Select option **4** to:
1. Review the full cart one final time
2. Enter an optional discount percentage (0–100)
3. View the final bill showing base total, GST, discount, and **total payable**
4. The order is automatically saved to `orders.txt`

### Step 6 — View Order History
Select option **5** to print all previously saved orders from `orders.txt` directly to the console.

---

## 🧮 GST Calculation

| Category    | GST Rate | Example (Rs. 10,000 base) |
|-------------|----------|---------------------------|
| Electronics | 18%      | Rs. 1,800 GST → Rs. 11,800 total |
| Accessory   | 18%      | Rs. 1,800 GST → Rs. 11,800 total |

> GST is calculated per unit and multiplied by quantity at cart view and checkout.

---

## 👨‍💻 Developer

**Abdeali Makda**