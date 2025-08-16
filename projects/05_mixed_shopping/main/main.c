#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SHOPPING_MATH";

// โครงสร้างข้อมูลสินค้า
typedef struct {
    char name[20];           // ชื่อสินค้า
    int quantity;            // จำนวน
    float price_per_unit;    // ราคาต่อหน่วย
    float total_price;       // ราคารวม
} product_t;

// ฟังก์ชันคำนวณราคาสินค้า
void calculate_product_total(product_t *product) {
    product->total_price = product->quantity * product->price_per_unit;
}

// ฟังก์ชันแสดงรายการสินค้า
void display_product(const product_t *product) {
    ESP_LOGI(TAG, "   %s: %d × %.0f = %.0f บาท", 
             product->name, product->quantity, product->price_per_unit, product->total_price);
}

// ฟังก์ชันคำนวณราคารวมทั้งหมด
float calculate_total_bill(product_t products[], int count) {
    float total = 0.0;
    for (int i = 0; i < count; i++) {
        calculate_product_total(&products[i]);
        total += products[i].total_price;
    }
    return total;
}

// ฟังก์ชันใช้ส่วนลด (เป็นจำนวนเงิน)
float apply_fixed_discount(float total, float discount) {
    return total - discount;
}

// ฟังก์ชันใช้ส่วนลด (เป็นเปอร์เซ็นต์)
float apply_percent_discount(float total, float discount_percent) {
    return total * (1 - (discount_percent / 100.0));
}

// ฟังก์ชันคำนวณ VAT
float calculate_vat(float amount, float vat_rate_percent) {
    return amount * (vat_rate_percent / 100.0);
}

// ฟังก์ชันแบ่งจ่าย
float split_payment(float amount, int people) {
    if (people <= 0) {
        ESP_LOGE(TAG, "Error: จำนวนคนต้องมากกว่า 0");
        return 0.0;
    }
    return amount / people;
}

void app_main(void)
{
    ESP_LOGI(TAG, "🛒 เริ่มต้นโปรแกรมซื้อของที่ตลาด (ฉบับปรับปรุง) 🛒");
    ESP_LOGI(TAG, "=================================================");
    
    // ข้อมูลการซื้อของ
    product_t products[] = {
        {"แอปเปิ้ล", 6, 15.0, 0.0},
        {"กล้วย", 12, 8.0, 0.0},
        {"ส้ม", 8, 12.0, 0.0},
        {"ช็อกโกแลต", 5, 50.0, 0.0} // เพิ่มสินค้าที่ชอบ: ช็อกโกแลต
    };
    int product_count = sizeof(products) / sizeof(products[0]);
    
    float discount_percent = 10.0; // ส่วนลด 10%
    float vat_rate = 7.0;         // ภาษี VAT 7%
    int people = 3;               // จำนวนคนที่จะแบ่งจ่าย
    
    // แสดงโจทย์
    ESP_LOGI(TAG, "📖 โจทย์:");
    ESP_LOGI(TAG, "   แม่ไปซื้อของที่ตลาด:");
    for (int i = 0; i < product_count; i++) {
        ESP_LOGI(TAG, "   - %s: %d หน่วย หน่วยละ %.0f บาท", 
                 products[i].name, products[i].quantity, products[i].price_per_unit);
    }
    ESP_LOGI(TAG, "   - มีส่วนลด: %.0f%%", discount_percent);
    ESP_LOGI(TAG, "   - มีภาษี VAT: %.0f%%", vat_rate);
    ESP_LOGI(TAG, "   - แบ่งจ่าย: %d คน", people);
    ESP_LOGI(TAG, "");
    
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    
    // คำนวณราคารวมแต่ละสินค้า
    ESP_LOGI(TAG, "🧮 ขั้นตอนการคิด:");
    ESP_LOGI(TAG, "   1. คำนวณราคาแต่ละสินค้า (การคูณ):");
    
    float subtotal = calculate_total_bill(products, product_count);
    
    for (int i = 0; i < product_count; i++) {
        ESP_LOGI(TAG, "      %s: %d × %.0f = %.0f บาท", 
                 products[i].name, products[i].quantity, 
                 products[i].price_per_unit, products[i].total_price);
    }
    ESP_LOGI(TAG, "");
    
    // รวมราคาทั้งหมด
    ESP_LOGI(TAG, "   2. รวมราคาทั้งหมด (การบวก):");
    ESP_LOGI(TAG, "      %.0f บาท (รวมสินค้าทั้งหมด)", subtotal);
    ESP_LOGI(TAG, "");
    
    // หักส่วนลด
    float discounted_total = apply_percent_discount(subtotal, discount_percent);
    float actual_discount_amount = subtotal - discounted_total;
    ESP_LOGI(TAG, "   3. หักส่วนลด (%.0f%%):", discount_percent);
    ESP_LOGI(TAG, "      %.0f - %.2f = %.2f บาท", subtotal, actual_discount_amount, discounted_total);
    ESP_LOGI(TAG, "");

    // เพิ่ม VAT
    float vat_amount = calculate_vat(discounted_total, vat_rate);
    float total_with_vat = discounted_total + vat_amount;
    ESP_LOGI(TAG, "   4. เพิ่มภาษี VAT (%.0f%%):", vat_rate);
    ESP_LOGI(TAG, "      VAT ที่ต้องจ่าย: %.2f บาท", vat_amount);
    ESP_LOGI(TAG, "      ยอดสุทธิรวม VAT: %.2f + %.2f = %.2f บาท", discounted_total, vat_amount, total_with_vat);
    ESP_LOGI(TAG, "");
    
    // แบ่งจ่าย
    float per_person = split_payment(total_with_vat, people);
    ESP_LOGI(TAG, "   5. แบ่งจ่าย (การหาร):");
    ESP_LOGI(TAG, "      %.2f ÷ %d = %.2f บาท/คน", total_with_vat, people, per_person);
    ESP_LOGI(TAG, "");
    
    // แสดงใบเสร็จ
    ESP_LOGI(TAG, "🧾 ใบเสร็จรับเงิน:");
    ESP_LOGI(TAG, "   ==========================================");
    ESP_LOGI(TAG, "   🏪 ตลาดสดใหม่ 🏪");
    ESP_LOGI(TAG, "   ==========================================");
    
    for (int i = 0; i < product_count; i++) {
        display_product(&products[i]);
    }
    
    ESP_LOGI(TAG, "   ----------------------------------------");
    ESP_LOGI(TAG, "   ยอดรวม (ก่อนส่วนลด):      %.2f บาท", subtotal);
    ESP_LOGI(TAG, "   ส่วนลด (%.0f%%):          -%.2f บาท", discount_percent, actual_discount_amount);
    ESP_LOGI(TAG, "   ยอดหลังหักส่วนลด:         %.2f บาท", discounted_total);
    ESP_LOGI(TAG, "   VAT (%.0f%%):             +%.2f บาท", vat_rate, vat_amount);
    ESP_LOGI(TAG, "   ========================================");
    ESP_LOGI(TAG, "   ยอดสุทธิ:                 %.2f บาท", total_with_vat);
    ESP_LOGI(TAG, "   แบ่งจ่าย %d คน:           %.2f บาท/คน", people, per_person);
    ESP_LOGI(TAG, "   ========================================");
    ESP_LOGI(TAG, "   ขอบคุณที่ใช้บริการ 😊");
    ESP_LOGI(TAG, "");
    
    ESP_LOGI(TAG, "🎉 จบโปรแกรมซื้อของที่ตลาด!");
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}