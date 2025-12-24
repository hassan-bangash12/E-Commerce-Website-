#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*===========================================================
    PRODUCT STRUCT
===========================================================*/
struct Product {
    int id;
    string name;
    double price;
    string category;
};

/*===========================================================
    LINKED LIST (Cart)
===========================================================*/
struct CartNode {
    Product data;
    int qty;
    CartNode* next;
    CartNode(Product p, int q) { 
        data = p; 
        qty = q; 
        next = NULL; 
    }
};

class CartLinkedList {
private:
    CartNode* head;
public:
    CartLinkedList() 
    { 
        head = NULL;
     }

    void insertAtBeginning(Product p, int q)
     {
        CartNode* newNode = new CartNode(p, q);
        newNode->next = head;
        head = newNode;
    }

    CartNode* search(int id)
    {
        CartNode* temp = head;
        while (temp != NULL) {
            if (temp->data.id == id) return temp;
            temp = temp->next;
        }
        return NULL;
    }

    void addItem(Product p, int q) 
    {
        CartNode* found = search(p.id);
        if (found) found->qty += q;
        else insertAtBeginning(p, q);
    }

    bool deleteById(int id) {
        if (!head)
        return false;
        if (head->data.id == id)
    { 
        CartNode* temp = head;
        head = head->next; 
        delete temp;
        return true; 
    }
        CartNode* curr = head;
        while (curr->next && curr->next->data.id != id) 
        curr = curr->next;
        if (!curr->next) return false;
        CartNode* temp = curr->next;
        curr->next = temp->next;
        delete temp;
        return true;
    }

    void setQuantity(int id, int qty) {
        CartNode* node = search(id);
        if (!node) return;
        if (qty <= 0) deleteById(id);
        else node->qty = qty;
    }

    void display() {
        CartNode* temp = head;
        if (!temp) { cout << "Cart is empty.\n"; return; }
        cout << "\n--- CART ITEMS ---\n";
        while (temp) {
            cout << temp->data.name << " x" << temp->qty
                 << " (Rs. " << temp->data.price << " each)\n";
            temp = temp->next;
        }
    }

    vector<pair<Product,int>> getItems() {
        vector<pair<Product,int>> items;
        CartNode* temp = head;
        while (temp) { items.push_back({temp->data,temp->qty}); temp = temp->next; }
        return items;
    }

    void clear() {
        CartNode* temp;
        while (head) 
        {
     temp = head; head = head->next;
      delete temp; 
    }
    }
};

/*===========================================================
    STACK (Undo)
===========================================================*/
struct StackNode {
    string action;
    Product p;
    int qty;
    StackNode* next;
    StackNode(string a, Product pr, int q) { action = a; p = pr; qty = q; next = NULL; }
};

class UndoStack {
private:
    StackNode* top;
public:
    UndoStack() { top = NULL; }
    void push(string act, Product p, int qty) { StackNode* n = new StackNode(act,p,qty); n->next = top; top = n; }
    bool isEmpty() { return top == NULL; }
    StackNode* pop() { if (isEmpty()) return NULL; StackNode* temp = top; top = top->next; return temp; }
};

/*===========================================================
    QUEUE (Orders)
===========================================================*/
struct Order {
    int orderID;
    vector<pair<Product,int>> items;
    double total;
};

struct QueueNode {
    Order order;
    QueueNode* next;
    QueueNode(Order o) { order=o; next=NULL; }
};

class OrderQueue {
private:
    QueueNode* front;
    QueueNode* rear;
public:
    OrderQueue() { front = rear = NULL; }
    void enqueue(Order o) {
        QueueNode* nn = new QueueNode(o);
        if (!rear) { front = rear = nn; return; }
        rear->next = nn; rear = nn;
    }
    bool dequeue() {
        if (!front) return false;
        QueueNode* temp = front;
        front = front->next;
        if (!front) rear = NULL;
        delete temp;
        return true;
    }
    bool displayFront() {
        if (!front) return false;
        cout << "\nProcessing Order #" << front->order.orderID << endl;
        for (auto& it : front->order.items) cout << it.first.name << " x" << it.second << endl;
        cout << "Total = Rs. " << front->order.total << endl;
        return true;
    }
};

/*===========================================================
    TREE (BST Categories)
===========================================================*/
struct TreeNode {
    string category;
    double discount;
    TreeNode* left;
    TreeNode* right;
    TreeNode(string c,double d) { category=c; discount=d; left=right=NULL; }
};

class CategoryBST {
public:
    TreeNode* root;
    CategoryBST() { root=NULL; }
    TreeNode* insert(TreeNode* root,string cat,double disc) {
        if (!root) return new TreeNode(cat,disc);
        if (cat<root->category) root->left=insert(root->left,cat,disc);
        else root->right=insert(root->right,cat,disc);
        return root;
    }
    double search(TreeNode* root,string cat) {
        if (!root) return 0;
        if (root->category==cat) return root->discount;
        if (cat<root->category) return search(root->left,cat);
        else return search(root->right,cat);
    }
};

/*===========================================================
    ECOMMERCE SYSTEM
===========================================================*/
class ECommerce {
public:
    vector<Product> products;
    CartLinkedList cart;
    UndoStack undoStack;
    OrderQueue orderQueue;
    CategoryBST categories;
    vector<Order> orderHistory;
    int nextOrderID = 1;

    ECommerce() {
        categories.root = categories.insert(categories.root,"Electronics",10);
        categories.root = categories.insert(categories.root,"Clothes",15);

        products.push_back({1,"Phone",30000,"Electronics"});
        products.push_back({2,"Laptop",80000,"Electronics"});
        products.push_back({3,"T-Shirt",1500,"Clothes"});
        products.push_back({4,"Jeans",2500,"Clothes"});
    }

    void addToCart(int id,int qty) {
        for (auto& p : products) {
            if (p.id==id) { cart.addItem(p,qty); undoStack.push("ADD",p,qty); return; }
        }
        cout << "Product not found!\n";
    }

    void undo() {
        StackNode* last = undoStack.pop();
        if (!last) { cout << "Nothing to undo.\n"; return; }
        if (last->action=="ADD") cart.deleteById(last->p.id);
        else if (last->action=="SET") cart.setQuantity(last->p.id,last->qty);
        cout << "Undo successful.\n";
    }

    void clearCart() { cart.clear(); cout << "Cart cleared.\n"; }

    void updateQuantity(int id,int newQty) {
        CartNode* node = cart.search(id);
        if (!node) { cout << "Item not found.\n"; return; }
        int oldQty = node->qty;
        cart.setQuantity(id,newQty);
        undoStack.push("SET",node->data,oldQty);
        cout << "Quantity updated.\n";
    }

    void searchProductByName(string name) {
        cout << "\nSearching for: " << name << endl;
        bool found=false;
        for (auto &p: products) {
            if (p.name.find(name)!=string::npos) {
                cout << "Found: " << p.id << " - " << p.name << " Rs." << p.price << endl;
                found=true;
            }
        }
        if (!found) cout << "No product matched.\n";
    }

    void showOrderHistory() {
        if (orderHistory.empty()) { cout << "No past orders.\n"; return; }
        cout << "\n--- ORDER HISTORY ---\n";
        for (auto &o : orderHistory) {
            cout << "Order #" << o.orderID << " Total = Rs." << o.total << endl;
            for (auto &it : o.items) cout << "   " << it.first.name << " x" << it.second << endl;
            cout << "-----------------------------\n";
        }
    }

    void estimateDelivery() {
        int days = 2 + rand()%5; 
        cout << "Estimated Delivery: " << days << " days.\n";
    }

    // ---------------- SPECIAL OFFERS ----------------
    double applySpecialOffers(vector<pair<Product,int>> &items) {
        double total=0;
        for (auto &it: items) {
            Product p=it.first;
            int qty=it.second;
            double price=p.price;

            // Buy 2 Get 1 Free (Clothes)
            if (p.category=="Clothes") {
                int freeItems = qty/3;
                int payableQty = qty - freeItems;
                price *= payableQty;
            } else {
                price *= qty;
            }

            // Bulk Discount 10%
            if (qty>=3) price *= 0.9;

            total += price;
        }

        // Extra 5% off if total>50,000
        if (total>50000) total *= 0.95;

        // Free delivery above 20,000
        if (total<=20000) { cout << "\nDelivery Charges: Rs.200\n"; total+=200; }
        else cout << "\nFREE Delivery Applied!\n";

        return total;
    }

    // ---------------- CHECKOUT ----------------
    void checkout() {
        vector<pair<Product,int>> items = cart.getItems();
        if (items.empty()) { cout << "Cart empty!\n"; return; }

        double total = applySpecialOffers(items);

        Order order;
        order.orderID = nextOrderID++;
        order.items = items;
        order.total = total;

        orderQueue.enqueue(order);
        orderHistory.push_back(order);
        cart.clear();

        cout << "\nOrder Placed Successfully!\n";
        cout << "Order ID: " << order.orderID << endl;
        cout << "Final Total (after all discounts & offers): Rs. " << total << endl;
    }
};

/*===========================================================
    MAIN MENU
===========================================================*/
int main() {
    ECommerce e;
    int choice;
    while (true) {
        cout << "\n===== E-COMMERCE MENU =====\n";
        cout << "1. View Products\n2. Add to Cart\n3. View Cart\n4. Undo Last Action\n5. Checkout\n";
        cout << "6. Process Next Order\n7. Exit\n8. Search Product\n9. Update Quantity\n";
        cout << "10. Clear Cart\n11. Order History\n12. Delivery Estimate\n";
        cout << "Enter choice: "; cin >> choice;

        if (choice==1) for (auto &p:e.products) cout<<p.id<<". "<<p.name<<" Rs."<<p.price<<"\n";
        else if (choice==2) { int id,qty; cout<<"Enter product ID: "; cin>>id; cout<<"Enter quantity: "; cin>>qty; e.addToCart(id,qty);}
        else if (choice==3) e.cart.display();
        else if (choice==4) e.undo();
        else if (choice==5) e.checkout();
        else if (choice==6) { if(!e.orderQueue.displayFront()) cout<<"No orders in queue.\n"; else e.orderQueue.dequeue();}
        else if (choice==7) break;
        else if (choice==8) { string name; cout<<"Enter product name: "; cin>>name; e.searchProductByName(name);}
        else if (choice==9) { int id,qty; cout<<"Enter product ID: "; cin>>id; cout<<"Enter new quantity: "; cin>>qty; e.updateQuantity(id,qty);}
        else if (choice==10) e.clearCart();
        else if (choice==11) e.showOrderHistory();
        else if (choice==12) e.estimateDelivery();
        else cout<<"Invalid choice!\n";
    }
    return 0;
}
