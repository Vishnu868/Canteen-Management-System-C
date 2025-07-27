#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define USER_DATABASE_FILENAME "usersnc.txt"
#define MENU_FILENAME "menu.txt"

struct User {
    char username[20];
    char password[20];
};

struct MenuItem {
    char name[20];
    float price;
    int quantity;
};

struct Order {
    char item[20];
    int quantity;
    float price;
};

// Function prototypes
void readUsersFromFile(struct User users[], int *userCount);
bool authenticateUser(const struct User users[], int userCount, const char *username, const char *password);
void readMenuFromFile(struct MenuItem menu[], int menusize);
void writeMenuToFile(struct MenuItem menu[], int menusize);
void displayMenuAndStock(struct MenuItem menu[], int menusize);
void updateMenuItem(struct MenuItem menu[], int menusize);
void updateStock(struct MenuItem menu[], int menusize);
void updatePrice(struct MenuItem menu[], int menusize);
float placeOrder(int choice, int quantity, struct Order orders[], int *orderCount, struct MenuItem menu[], int menusize);
void displayOrders(struct Order orders[], int orderCount);

void readUsersFromFile(struct User users[], int *userCount) {
    FILE *file = fopen(USER_DATABASE_FILENAME, "r");
    if (file != NULL) {
        while (fscanf(file, "%s %s", users[*userCount].username, users[*userCount].password) == 2) {
            (*userCount)++;
            if (*userCount >= 100) break; // Prevent buffer overflow
        }
        fclose(file);
    } else {
        printf("Warning: Could not open user database file.\n");
    }
}

bool authenticateUser(const struct User users[], int userCount, const char *username, const char *password) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return true;
        }
    }
    return false;
}

void readMenuFromFile(struct MenuItem menu[], int menusize) {
    FILE *file = fopen(MENU_FILENAME, "r");
    if (file != NULL) {
        for (int i = 0; i < menusize; i++) {
            if (fscanf(file, "%s %f %d", menu[i].name, &menu[i].price, &menu[i].quantity) != 3) {
                break; // Stop if we can't read all three values
            }
        }
        fclose(file);
    } else {
        printf("Warning: Could not open menu file.\n");
    }
}

void writeMenuToFile(struct MenuItem menu[], int menusize) {
    FILE *file = fopen(MENU_FILENAME, "w");
    if (file != NULL) {
        for (int i = 0; i < menusize; i++) {
            fprintf(file, "%s %.2f %d\n", menu[i].name, menu[i].price, menu[i].quantity);
        }
        fclose(file);
    } else {
        printf("Error: Could not write to menu file.\n");
    }
}

void displayMenuAndStock(struct MenuItem menu[], int menusize) {
    printf("\n=== MENU ===\n");
    for (int i = 0; i < menusize; i++) {
        printf("%d. %s - $%.2f - Stock: %d\n", i + 1, menu[i].name, menu[i].price, menu[i].quantity);
    }
    printf("============\n\n");
}

void updateMenuItem(struct MenuItem menu[], int menusize) {
    int choice;
    printf("Enter the menu item number you want to update (1-%d): ", menusize);
    scanf("%d", &choice);

    if (choice >= 1 && choice <= menusize) {
        printf("Enter new name for %s: ", menu[choice - 1].name);
        scanf("%s", menu[choice - 1].name);
        printf("Enter new price for %s: ", menu[choice - 1].name);
        scanf("%f", &menu[choice - 1].price);
        printf("Menu item updated successfully!\n");
    } else {
        printf("Invalid choice!\n");
    }
}

void updateStock(struct MenuItem menu[], int menusize) {
    int choice;
    printf("Enter the menu item number to update stock (1-%d): ", menusize);
    scanf("%d", &choice);

    if (choice >= 1 && choice <= menusize) {
        printf("Current stock for %s: %d\n", menu[choice - 1].name, menu[choice - 1].quantity);
        printf("Enter new stock quantity for %s: ", menu[choice - 1].name);
        scanf("%d", &menu[choice - 1].quantity);
        printf("Stock updated successfully!\n");
    } else {
        printf("Invalid choice!\n");
    }
}

void updatePrice(struct MenuItem menu[], int menusize) {
    int choice;
    printf("Enter the menu item number to update price (1-%d): ", menusize);
    scanf("%d", &choice);

    if (choice >= 1 && choice <= menusize) {
        printf("Current price for %s: $%.2f\n", menu[choice - 1].name, menu[choice - 1].price);
        printf("Enter new price for %s: ", menu[choice - 1].name);
        scanf("%f", &menu[choice - 1].price);
        printf("Price updated successfully!\n");
    } else {
        printf("Invalid choice!\n");
    }
}

float placeOrder(int choice, int quantity, struct Order orders[], int *orderCount, struct MenuItem menu[], int menuSize) {
    float total = 0.0;

    if (choice >= 1 && choice <= menuSize) {
        if (quantity <= 0) {
            printf("Invalid quantity!\n");
            return 0.0;
        }
        
        if (quantity > menu[choice - 1].quantity) {
            printf("Sorry, not enough stock for %s. Available: %d\n", 
                   menu[choice - 1].name, menu[choice - 1].quantity);
            return 0.0;
        } else {
            total = quantity * menu[choice - 1].price;
            menu[choice - 1].quantity -= quantity;
            strcpy(orders[*orderCount].item, menu[choice - 1].name);
            orders[*orderCount].quantity = quantity;
            orders[*orderCount].price = total;
            (*orderCount)++;
            printf("Added %d %s to your order. Subtotal: $%.2f\n", 
                   quantity, menu[choice - 1].name, total);
        }
    } else {
        printf("Invalid choice!\n");
    }

    return total;
}

void displayOrders(struct Order orders[], int orderCount) {
    if (orderCount == 0) {
        printf("No orders placed yet.\n");
        return;
    }
    
    printf("\n=== YOUR ORDERS ===\n");
    float grandTotal = 0.0;
    for (int i = 0; i < orderCount; i++) {
        printf("%d. %s - Quantity: %d, Price: $%.2f\n", 
               i + 1, orders[i].item, orders[i].quantity, orders[i].price);
        grandTotal += orders[i].price;
    }
    printf("===================\n");
    printf("Grand Total: $%.2f\n\n", grandTotal);
}

int main() {
    int choice, quantity, orderCount = 0, menuSize = 13, userCount = 0;
    float sessionTotal = 0.0;

    struct User users[100];
    struct MenuItem menu[13];
    struct Order orders[100];

    readUsersFromFile(users, &userCount);
    readMenuFromFile(menu, menuSize);

    char username[20], password[20];

    printf("=== Restaurant Management System ===\n\n");

    // Authentication loop
    while (1) {
        printf("Enter your username: ");
        scanf("%s", username);
        
        // Clear input buffer
        while (getchar() != '\n');

        printf("Enter your password: ");
        scanf("%s", password);
        
        // Clear input buffer
        while (getchar() != '\n');

        if (authenticateUser(users, userCount, username, password)) {
            printf("Authentication successful. Welcome, %s!\n\n", username);
            break;
        } else {
            printf("Authentication failed. Invalid username or password.\n\n");
            printf("Available users from database:\n");
            for (int i = 0; i < userCount; i++) {
                printf("- %s\n", users[i].username);
            }
            printf("\n");
        }
    }

    // Main menu loop
    do {
        printf("=== MAIN MENU ===\n");
        printf("1. Display Menu and Stock\n");
        printf("2. Update Menu Item\n");
        printf("3. Update Stock\n");
        printf("4. Update Price\n");
        printf("5. Place Order\n");
        printf("6. View Current Orders\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayMenuAndStock(menu, menuSize);
                break;
                
            case 2:
                displayMenuAndStock(menu, menuSize);
                updateMenuItem(menu, menuSize);
                writeMenuToFile(menu, menuSize);
                break;
                
            case 3:
                displayMenuAndStock(menu, menuSize);
                updateStock(menu, menuSize);
                writeMenuToFile(menu, menuSize);
                break;
                
            case 4:
                displayMenuAndStock(menu, menuSize);
                updatePrice(menu, menuSize);
                writeMenuToFile(menu, menuSize);
                break;
                
            case 5:
                displayMenuAndStock(menu, menuSize);
                printf("Enter your choice (1-%d) or 0 to go back: ", menuSize);
                scanf("%d", &choice);

                if (choice == 0) {
                    break;
                }

                if (choice >= 1 && choice <= menuSize) {
                    printf("Enter quantity: ");
                    scanf("%d", &quantity);
                    float orderTotal = placeOrder(choice, quantity, orders, &orderCount, menu, menuSize);
                    sessionTotal += orderTotal;
                    writeMenuToFile(menu, menuSize);
                } else {
                    printf("Invalid menu choice!\n");
                }
                break;
                
            case 6:
                displayOrders(orders, orderCount);
                break;
                
            case 0:
                if (orderCount > 0) {
                    displayOrders(orders, orderCount);
                    printf("Thank you for your orders! Session total: $%.2f\n", sessionTotal);
                } else {
                    printf("No orders placed this session.\n");
                }
                printf("Goodbye, %s!\n", username);
                break;
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
        
        if (choice != 0) {
            printf("\nPress Enter to continue...");
            while (getchar() != '\n'); // consume any remaining input
            getchar(); // wait for user input
        }
        
    } while (choice != 0);

    return 0;
}