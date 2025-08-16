#include <stdio.h>
#include <string.h>
#include <math.h>       // สำหรับ powf, sqrtf
#include <time.h>       // สำหรับ timestamp ใน history
#include <unistd.h>     // สำหรับ unlink (ลบไฟล์)
#include <ctype.h>      // สำหรับ isdigit

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h" // สำหรับ File System

// --- Constants ---
#define MAX_PRODUCTS 10
#define MAX_HISTORY_ENTRIES 100
#define HISTORY_FILE "/spiffs/calculator_history.txt"

// --- Global TAG for logging ---
static const char *TAG = "CALCULATOR_APP";

// --- 1. Data Structures and Enums ---
typedef enum {
    MODE_EXIT = 0,
    MODE_BASIC,
    MODE_ADVANCED,
    MODE_SHOP,
    MODE_HISTORY
} calculator_mode_t;

typedef enum {
    OP_NONE = 0,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POWER,
    OP_SQRT,
    OP_FACTORIAL
} basic_operation_t;

typedef enum {
    ERROR_NONE = 0,
    ERROR_INVALID_INPUT,
    ERROR_DIVISION_BY_ZERO,
    ERROR_SQRT_NEGATIVE,
    ERROR_FACTORIAL_NEGATIVE,
    ERROR_FACTORIAL_OVERFLOW,
    ERROR_MAX_PRODUCTS_REACHED,
    ERROR_INSUFFICIENT_PAYMENT,
    ERROR_FILE_OPERATION_FAILED,
    ERROR_UNKNOWN
} error_code_t;

typedef struct {
    char name[30];
    int quantity;
    float price_per_unit;
    float total_price;
} product_t;

typedef struct {
    int id;
    char operation[50];
    float result;
    char timestamp[30];
} history_entry_t;

// --- Global Variables for History ---
static history_entry_t history_log[MAX_HISTORY_ENTRIES];
static int history_count = 0;
static int next_history_id = 1;

// --- 2. Error Handling System ---
void handle_error(error_code_t code, const char *message) {
    ESP_LOGE(TAG, "❌ ข้อผิดพลาด (รหัส: %d): %s", code, message);
}

// --- 3. Core Calculation Functions (Basic Math) ---
float basic_add(float a, float b) {
    return a + b;
}

float basic_subtract(float a, float b) {
    return a - b;
}

float basic_multiply(float a, float b) {
    return a * b;
}

float basic_divide(float a, float b) {
    if (b == 0) {
        handle_error(ERROR_DIVISION_BY_ZERO, "ไม่สามารถหารด้วยศูนย์ได้");
        return 0.0;
    }
    return a / b;
}

float basic_power(float base, float exp) {
    return powf(base, exp);
}

float basic_square_root(float num) {
    if (num < 0) {
        handle_error(ERROR_SQRT_NEGATIVE, "ไม่สามารถหารากที่สองของจำนวนลบได้");
        return 0.0;
    }
    return sqrtf(num);
}

long long basic_factorial(int n) {
    if (n < 0) {
        handle_error(ERROR_FACTORIAL_NEGATIVE, "ไม่สามารถหาแฟกทอเรียลของจำนวนลบได้");
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    long long result = 1;
    for (int i = 1; i <= n; i++) {
        // Simple overflow check (for large numbers, this is still limited by long long)
        if (i > (LLONG_MAX / result)) { // LLONG_MAX from <limits.h> if you include it
             handle_error(ERROR_FACTORIAL_OVERFLOW, "ผลลัพธ์แฟกทอเรียลเกินขีดจำกัด");
             return -1; // Indicate error
        }
        result *= i;
    }
    return result;
}

// --- 4. Shop POS System Functions ---
void shop_calculate_product_total(product_t *product) {
    product->total_price = product->quantity * product->price_per_unit;
}

void shop_add_product(product_t products[], int *count, const char *name, int quantity, float price_per_unit) {
    if (*count >= MAX_PRODUCTS) {
        handle_error(ERROR_MAX_PRODUCTS_REACHED, "ไม่สามารถเพิ่มสินค้าได้อีก: เกินจำนวนสูงสุด");
        return;
    }
    if (strlen(name) >= sizeof(products[*count].name)) {
        handle_error(ERROR_INVALID_INPUT, "ชื่อสินค้าเกินขนาดที่กำหนด");
        return;
    }
    if (quantity <= 0 || price_per_unit <= 0) {
        handle_error(ERROR_INVALID_INPUT, "จำนวนหรือราคาต่อหน่วยต้องมากกว่าศูนย์");
        return;
    }

    strcpy(products[*count].name, name);
    products[*count].quantity = quantity;
    products[*count].price_per_unit = price_per_unit;
    shop_calculate_product_total(&products[*count]); // คำนวณราคารวมทันที
    (*count)++;
    ESP_LOGI(TAG, "เพิ่มสินค้า: %s (%d x %.2f)", name, quantity, price_per_unit);
}

float shop_calculate_subtotal(product_t products[], int count) {
    float subtotal = 0.0;
    for (int i = 0; i < count; i++) {
        subtotal += products[i].total_price;
    }
    return subtotal;
}

float shop_apply_percent_discount(float total, float discount_percent) {
    if (discount_percent < 0 || discount_percent > 100) {
        handle_error(ERROR_INVALID_INPUT, "ส่วนลดเปอร์เซ็นต์ต้องอยู่ระหว่าง 0-100");
        return total;
    }
    return total * (1 - (discount_percent / 100.0));
}

float shop_calculate_vat(float amount, float vat_rate_percent) {
    if (vat_rate_percent < 0) {
        handle_error(ERROR_INVALID_INPUT, "อัตรา VAT ต้องไม่เป็นค่าลบ");
        return 0.0;
    }
    return amount * (vat_rate_percent / 100.0);
}

float shop_calculate_change(float total_bill, float amount_paid) {
    if (amount_paid < total_bill) {
        handle_error(ERROR_INSUFFICIENT_PAYMENT, "จำนวนเงินที่จ่ายไม่เพียงพอ");
        return 0.0;
    }
    return amount_paid - total_bill;
}

void shop_print_receipt(product_t products[], int count, float subtotal, float discount_percent, float vat_amount, float final_total, float amount_paid, float change) {
    ESP_LOGI(TAG, "==========================================");
    ESP_LOGI(TAG, "         🏪 ร้านสะดวกซื้อ 'คิดเก่ง' 🏪         ");
    ESP_LOGI(TAG, "==========================================");
    ESP_LOGI(TAG, "รายการสินค้า:");
    for (int i = 0; i < count; i++) {
        ESP_LOGI(TAG, "  %s: %d x %.2f = %.2f บาท", products[i].name, products[i].quantity, products[i].price_per_unit, products[i].total_price);
    }
    ESP_LOGI(TAG, "------------------------------------------");
    ESP_LOGI(TAG, "ยอดรวมก่อนส่วนลด:       %.2f บาท", subtotal);
    if (discount_percent > 0) {
        ESP_LOGI(TAG, "ส่วนลด (%.0f%%):          -%.2f บาท", discount_percent, subtotal * (discount_percent / 100.0));
    }
    ESP_LOGI(TAG, "รวมหลังหักส่วนลด:       %.2f บาท", subtotal * (1 - (discount_percent / 100.0)));
    ESP_LOGI(TAG, "ภาษี VAT (7%%):          +%.2f บาท", vat_amount);
    ESP_LOGI(TAG, "==========================================");
    ESP_LOGI(TAG, "ยอดสุทธิ:                %.2f บาท", final_total);
    ESP_LOGI(TAG, "เงินที่ได้รับ:            %.2f บาท", amount_paid);
    ESP_LOGI(TAG, "เงินทอน:                 %.2f บาท", change);
    ESP_LOGI(TAG, "==========================================");
    ESP_LOGI(TAG, "         ขอบคุณที่ใช้บริการ 😊         ");
    ESP_LOGI(TAG, "         โปรดกลับมาใช้บริการใหม่!       ");
    ESP_LOGI(TAG, "==========================================");
}

// --- 5. History Management ---
void save_history(const char *operation, float result) {
    if (history_count >= MAX_HISTORY_ENTRIES) {
        ESP_LOGW(TAG, "ประวัติเต็มแล้ว ไม่สามารถบันทึกได้");
        return;
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char time_str[30];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);

    history_log[history_count].id = next_history_id++;
    strncpy(history_log[history_count].operation, operation, sizeof(history_log[history_count].operation) - 1);
    history_log[history_count].operation[sizeof(history_log[history_count].operation) - 1] = '\0';
    history_log[history_count].result = result;
    strncpy(history_log[history_count].timestamp, time_str, sizeof(history_log[history_count].timestamp) - 1);
    history_log[history_count].timestamp[sizeof(history_log[history_count].timestamp) - 1] = '\0';

    history_count++;
    ESP_LOGI(TAG, "บันทึกประวัติแล้ว (#%d): %s = %.2f", history_log[history_count-1].id, operation, result);

    FILE *f = fopen(HISTORY_FILE, "a");
    if (f == NULL) {
        handle_error(ERROR_FILE_OPERATION_FAILED, "ไม่สามารถเปิดไฟล์ประวัติเพื่อเขียนได้");
        return;
    }
    fprintf(f, "%d;%s;%.2f;%s\n", history_log[history_count-1].id, operation, result, time_str);
    fclose(f);
}

void load_history() {
    FILE *f = fopen(HISTORY_FILE, "r");
    if (f == NULL) {
        ESP_LOGW(TAG, "ไม่พบไฟล์ประวัติ เริ่มต้นประวัติใหม่.");
        history_count = 0;
        next_history_id = 1;
        return;
    }

    history_count = 0;
    next_history_id = 1;
    char line[200];
    while (fgets(line, sizeof(line), f) != NULL && history_count < MAX_HISTORY_ENTRIES) {
        int id_read;
        char operation_read[50];
        float result_read;
        char timestamp_read[30];

        if (sscanf(line, "%d;%49[^;];%f;%29[^\n]", &id_read, operation_read, &result_read, timestamp_read) == 4) {
            history_log[history_count].id = id_read;
            strcpy(history_log[history_count].operation, operation_read);
            history_log[history_count].result = result_read;
            strcpy(history_log[history_count].timestamp, timestamp_read);
            
            history_count++;
            if (id_read >= next_history_id) {
                next_history_id = id_read + 1;
            }
        } else {
            ESP_LOGW(TAG, "รูปแบบข้อมูลประวัติไม่ถูกต้อง: %s", line);
        }
    }
    fclose(f);
    ESP_LOGI(TAG, "โหลดประวัติ %d รายการ.", history_count);
}

void display_history() {
    if (history_count == 0) {
        ESP_LOGI(TAG, "ยังไม่มีประวัติการคำนวณ.");
        return;
    }
    ESP_LOGI(TAG, "--- ประวัติการคำนวณ ---");
    for (int i = 0; i < history_count; i++) {
        ESP_LOGI(TAG, "#%d [%s]: %s = %.2f", 
                 history_log[i].id, history_log[i].timestamp, 
                 history_log[i].operation, history_log[i].result);
    }
    ESP_LOGI(TAG, "-----------------------");
}

void clear_history() {
    history_count = 0;
    next_history_id = 1;
    if (unlink(HISTORY_FILE) == 0) {
        ESP_LOGI(TAG, "ไฟล์ประวัติถูกลบแล้ว.");
    } else {
        handle_error(ERROR_FILE_OPERATION_FAILED, "ไม่สามารถลบไฟล์ประวัติได้ (อาจไม่พบไฟล์)");
    }
}

void get_history_stats() {
    ESP_LOGI(TAG, "--- สถิติการใช้งาน ---");
    ESP_LOGI(TAG, "🔢 การคำนวณทั้งหมด: %d ครั้ง", history_count);
    ESP_LOGI(TAG, "⭐ ความแม่นยำ: 100%% (ในตัวอย่างนี้)");
    ESP_LOGI(TAG, "🚀 ประสิทธิภาพ: เยี่ยม (ในตัวอย่างนี้)");
    ESP_LOGI(TAG, "-----------------------");
}

// --- 6. Menu and UI System ---
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

float get_float_input(const char *prompt) {
    float value;
    char buffer[50];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%f", &value) == 1) {
                return value;
            } else {
                ESP_LOGW(TAG, "อินพุตไม่ถูกต้อง กรุณาป้อนตัวเลขทศนิยม.");
            }
        }
        clear_input_buffer();
    }
}

int get_int_input(const char *prompt) {
    int value;
    char buffer[50];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            } else {
                ESP_LOGW(TAG, "อินพุตไม่ถูกต้อง กรุณาป้อนตัวเลขจำนวนเต็ม.");
            }
        }
        clear_input_buffer();
    }
}

void display_main_menu() {
    ESP_LOGI(TAG, "\n╔══════════════════════════════════════╗");
    ESP_LOGI(TAG, "║        🧮 เครื่องคิดเลขครบครัน        ║");
    ESP_LOGI(TAG, "╠══════════════════════════════════════╣");
    ESP_LOGI(TAG, "║ [1] โหมดพื้นฐาน - Basic Calculator  ║");
    ESP_LOGI(TAG, "║ [2] โหมดขั้นสูง - Advanced Math     ║");
    ESP_LOGI(TAG, "║ [3] โหมดร้านค้า - Shop POS System   ║");
    ESP_LOGI(TAG, "║ [4] โหมดประวัติ - History & Stats   ║");
    ESP_LOGI(TAG, "║ [0] ออกจากโปรแกรม                   ║");
    ESP_LOGI(TAG, "╚══════════════════════════════════════╝");
    ESP_LOGI(TAG, "โปรดเลือกโหมด (0-4): ");
}

int get_user_choice() {
    int choice = get_int_input(">> ");
    if (choice < 0 || choice > 4) {
        handle_error(ERROR_INVALID_INPUT, "ตัวเลือกไม่ถูกต้อง กรุณาป้อน 0-4");
        return -1;
    }
    return choice;
}

void run_basic_mode() {
    ESP_LOGI(TAG, "\n--- โหมดพื้นฐาน ---");
    ESP_LOGI(TAG, "[1] บวก [2] ลบ [3] คูณ [4] หาร");
    ESP_LOGI(TAG, "[5] ยกกำลัง [6] รากที่สอง [7] แฟกทอเรียล [0] กลับเมนูหลัก");
    int choice = get_int_input("เลือกการดำเนินการ (0-7): ");

    float num1, num2, result_float;
    long long result_ll;
    char operation_str[50];

    switch (choice) {
        case OP_ADD:
            num1 = get_float_input("ป้อนตัวเลขแรก: ");
            num2 = get_float_input("ป้อนตัวเลขที่สอง: ");
            result_float = basic_add(num1, num2);
            ESP_LOGI(TAG, "ผลลัพธ์: %.2f", result_float);
            snprintf(operation_str, sizeof(operation_str), "%.2f + %.2f", num1, num2);
            save_history(operation_str, result_float);
            break;
        case OP_SUBTRACT:
            num1 = get_float_input("ป้อนตัวตั้ง: ");
            num2 = get_float_input("ป้อนตัวลบ: ");
            result_float = basic_subtract(num1, num2);
            ESP_LOGI(TAG, "ผลลัพธ์: %.2f", result_float);
            snprintf(operation_str, sizeof(operation_str), "%.2f - %.2f", num1, num2);
            save_history(operation_str, result_float);
            break;
        case OP_MULTIPLY:
            num1 = get_float_input("ป้อนตัวเลขแรก: ");
            num2 = get_float_input("ป้อนตัวเลขที่สอง: ");
            result_float = basic_multiply(num1, num2);
            ESP_LOGI(TAG, "ผลลัพธ์: %.2f", result_float);
            snprintf(operation_str, sizeof(operation_str), "%.2f * %.2f", num1, num2);
            save_history(operation_str, result_float);
            break;
        case OP_DIVIDE:
            num1 = get_float_input("ป้อนตัวตั้ง: ");
            num2 = get_float_input("ป้อนตัวหาร: ");
            result_float = basic_divide(num1, num2);
            if (num2 != 0) {
                ESP_LOGI(TAG, "ผลลัพธ์: %.2f", result_float);
                snprintf(operation_str, sizeof(operation_str), "%.2f / %.2f", num1, num2);
                save_history(operation_str, result_float);
            }
            break;
        case OP_POWER:
            num1 = get_float_input("ป้อนฐาน: ");
            num2 = get_float_input("ป้อนเลขชี้กำลัง: ");
            result_float = basic_power(num1, num2);
            ESP_LOGI(TAG, "ผลลัพธ์: %.2f", result_float);
            snprintf(operation_str, sizeof(operation_str), "%.2f ^ %.2f", num1, num2);
            save_history(operation_str, result_float);
            break;
        case OP_SQRT:
            num1 = get_float_input("ป้อนตัวเลข: ");
            result_float = basic_square_root(num1);
            if (num1 >= 0) {
                ESP_LOGI(TAG, "ผลลัพธ์: %.2f", result_float);
                snprintf(operation_str, sizeof(operation_str), "sqrt(%.2f)", num1);
                save_history(operation_str, result_float);
            }
            break;
        case OP_FACTORIAL:
            num1 = get_int_input("ป้อนตัวเลขจำนวนเต็มบวก: ");
            result_ll = basic_factorial((int)num1);
            if (result_ll != -1) {
                ESP_LOGI(TAG, "ผลลัพธ์: %lld", result_ll);
                snprintf(operation_str, sizeof(operation_str), "Factorial(%d)", (int)num1);
                save_history(operation_str, (float)result_ll);
            }
            break;
        case 0:
            ESP_LOGI(TAG, "กลับสู่เมนูหลัก");
            break;
        default:
            handle_error(ERROR_INVALID_INPUT, "ตัวเลือกไม่ถูกต้อง");
            break;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void run_shop_mode() {
    ESP_LOGI(TAG, "\n--- โหมดร้านค้า (POS System) ---");
    product_t current_products[MAX_PRODUCTS];
    int current_product_count = 0;
    float subtotal = 0.0;
    float discount_percent = 0.0;
    float vat_rate = 7.0;
    float vat_amount = 0.0;
    float final_total = 0.0;
    float amount_paid = 0.0;
    float change = 0.0;

    int shop_choice;
    do {
        ESP_LOGI(TAG, "\nเลือกดำเนินการในโหมดร้านค้า:");
        ESP_LOGI(TAG, "[1] เพิ่มสินค้า [2] คำนวณรวม [3] กำหนดส่วนลด (%%)");
        ESP_LOGI(TAG, "[4] ชำระเงิน/เงินทอน [5] พิมพ์ใบเสร็จ [0] กลับเมนูหลัก");
        shop_choice = get_int_input(">> ");

        switch (shop_choice) {
            case 1: {
                char name[30];
                int quantity;
                float price;
                printf("ป้อนชื่อสินค้า: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                clear_input_buffer();

                quantity = get_int_input("ป้อนจำนวน: ");
                price = get_float_input("ป้อนราคาต่อหน่วย: ");
                shop_add_product(current_products, &current_product_count, name, quantity, price);
                break;
            }
            case 2:
                subtotal = shop_calculate_subtotal(current_products, current_product_count);
                ESP_LOGI(TAG, "ยอดรวมสินค้า: %.2f บาท", subtotal);
                break;
            case 3:
                discount_percent = get_float_input("ป้อนส่วนลดเป็นเปอร์เซ็นต์ (เช่น 10 สำหรับ 10%%): ");
                if (discount_percent < 0 || discount_percent > 100) {
                     handle_error(ERROR_INVALID_INPUT, "ส่วนลดต้องอยู่ระหว่าง 0-100%%");
                     discount_percent = 0.0;
                }
                ESP_LOGI(TAG, "ตั้งค่าส่วนลด: %.0f%%", discount_percent);
                break;
            case 4: {
                subtotal = shop_calculate_subtotal(current_products, current_product_count);
                float discounted = shop_apply_percent_discount(subtotal, discount_percent);
                vat_amount = shop_calculate_vat(discounted, vat_rate);
                final_total = discounted + vat_amount;
                ESP_LOGI(TAG, "ยอดสุทธิที่ต้องชำระ: %.2f บาท", final_total);
                amount_paid = get_float_input("ป้อนจำนวนเงินที่ได้รับ: ");
                change = shop_calculate_change(final_total, amount_paid);
                if (amount_paid >= final_total) {
                    ESP_LOGI(TAG, "เงินทอน: %.2f บาท", change);
                }
                break;
            }
            case 5:
                subtotal = shop_calculate_subtotal(current_products, current_product_count);
                float discounted_for_receipt = shop_apply_percent_discount(subtotal, discount_percent);
                vat_amount = shop_calculate_vat(discounted_for_receipt, vat_rate);
                final_total = discounted_for_receipt + vat_amount;
                shop_print_receipt(current_products, current_product_count, subtotal, discount_percent, vat_amount, final_total, amount_paid, change);
                save_history("Shop Transaction", final_total);
                break;
            case 0:
                ESP_LOGI(TAG, "กลับสู่เมนูหลัก");
                break;
            default:
                handle_error(ERROR_INVALID_INPUT, "ตัวเลือกไม่ถูกต้อง");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    } while (shop_choice != 0);
}

void run_history_mode() {
    ESP_LOGI(TAG, "\n--- โหมดประวัติ ---");
    int history_choice;
    do {
        ESP_LOGI(TAG, "[1] ดูประวัติทั้งหมด [2] ล้างประวัติ [3] ดูสถิติ [0] กลับเมนูหลัก");
        history_choice = get_int_input(">> ");

        switch (history_choice) {
            case 1:
                display_history();
                break;
            case 2:
                clear_history();
                ESP_LOGI(TAG, "ประวัติถูกล้างแล้ว.");
                break;
            case 3:
                get_history_stats();
                break;
            case 0:
                ESP_LOGI(TAG, "กลับสู่เมนูหลัก");
                break;
            default:
                handle_error(ERROR_INVALID_INPUT, "ตัวเลือกไม่ถูกต้อง");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    } while (history_choice != 0);
}

// TODO: ยังไม่ได้สร้างฟังก์ชัน run_advanced_mode()
void run_advanced_mode() {
    ESP_LOGW(TAG, "💡 โหมดขั้นสูงยังไม่พร้อมใช้งาน! โปรดรอการอัปเดต.");
    vTaskDelay(pdMS_TO_TICKS(1000));
}

// --- 7. Main Application Logic ---
static void spiffs_init(void) {
    ESP_LOGI(TAG, "กำลังเริ่มต้น SPIFFS...");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "ไม่สามารถฟอร์แมต SPIFFS หากการเมานต์ล้มเหลว");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "ไม่พบพาร์ติชัน SPIFFS");
        } else {
            ESP_LOGE(TAG, "การลงทะเบียน SPIFFS ล้มเหลว (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ไม่สามารถรับข้อมูล SPIFFS ได้ (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SPIFFS เริ่มต้นสมบูรณ์. ขนาดรวม: %dKB, ใช้ไป: %dKB", total / 1024, used / 1024);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "🚀 เริ่มต้นเครื่องคิดเลขครบครัน! 🚀");
    vTaskDelay(pdMS_TO_TICKS(1000));

    spiffs_init();
    load_history(); // Load history on startup

    calculator_mode_t current_mode = MODE_BASIC;
    int user_choice;

    while (current_mode != MODE_EXIT) {
        display_main_menu();
        user_choice = get_user_choice();

        if (user_choice == -1) {
            continue;
        }

        current_mode = (calculator_mode_t)user_choice;

        switch (current_mode) {
            case MODE_BASIC:
                run_basic_mode();
                break;
            case MODE_ADVANCED:
                run_advanced_mode(); // This will show the "not available" message
                break;
            case MODE_SHOP:
                run_shop_mode();
                break;
            case MODE_HISTORY:
                run_history_mode();
                break;
            case MODE_EXIT:
                ESP_LOGI(TAG, "👋 กำลังออกจากโปรแกรม ขอบคุณที่ใช้บริการ!");
                break;
            default:
                handle_error(ERROR_UNKNOWN, "โหมดการทำงานไม่ถูกต้อง");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS ยกเลิกการเมานต์แล้ว.");
}