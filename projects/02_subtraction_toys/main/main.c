#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "TOYS_MATH";

void app_main(void)
{
    ESP_LOGI(TAG, "🧸 เริ่มต้นโปรแกรมนับของเล่นของน้อง 🧸");
    ESP_LOGI(TAG, "=========================================");

    // แบบฝึกหัดที่ 1: เปลี่ยนจำนวนของเล่น
    int toys_at_home = 15;      // เปลี่ยนจาก 8 เป็น 15 ชิ้น
    int toys_give_away = 7;     // เปลี่ยนจาก 3 เป็น 7 ชิ้น
    int toys_remaining;

    // แบบฝึกหัดที่ 2: เพิ่มการตรวจสอบว่าของเล่นพอแจกไหม
    if (toys_at_home >= toys_give_away) {
        ESP_LOGI(TAG, "✅ ของเล่นพอแจก");
    } else {
        ESP_LOGI(TAG, "❌ ของเล่นไม่พอ! ขาดไป %d ชิ้น", 
                 toys_give_away - toys_at_home);
    }

    // ตรวจสอบก่อนคำนวณ
    if (toys_give_away > toys_at_home) {
        ESP_LOGW(TAG, "⚠️  คำเตือน: ของเล่นที่จะแจก (%d) มากกว่าที่มีอยู่ (%d)!", 
                 toys_give_away, toys_at_home);
        ESP_LOGI(TAG, "   น้องไม่สามารถแจกของเล่นได้มากกว่าที่มี");
        ESP_LOGI(TAG, "   จะแจกได้เฉพาะ %d ชิ้น (ทั้งหมดที่มี)", toys_at_home);
        toys_give_away = toys_at_home;
    }

    // คำนวณผลลัพธ์
    toys_remaining = toys_at_home - toys_give_away;

    ESP_LOGI(TAG, "📖 โจทย์:");
    ESP_LOGI(TAG, "   น้องมีของเล่น: %d ชิ้น", toys_at_home);
    ESP_LOGI(TAG, "   แจกให้เพื่อน: %d ชิ้น", toys_give_away);
    ESP_LOGI(TAG, "   ❓ น้องเหลือของเล่นกี่ชิ้น?");
    ESP_LOGI(TAG, "");

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // แสดงขั้นตอนการคิด
    ESP_LOGI(TAG, "🧮 ขั้นตอนการคิด:");
    ESP_LOGI(TAG, "   ของเล่นที่มี - ของเล่นที่แจก");
    ESP_LOGI(TAG, "   = %d - %d", toys_at_home, toys_give_away);
    ESP_LOGI(TAG, "   = %d ชิ้น", toys_remaining);
    ESP_LOGI(TAG, "");

    // แสดงคำตอบ
    ESP_LOGI(TAG, "✅ คำตอบ:");
    ESP_LOGI(TAG, "   น้องเหลือของเล่น %d ชิ้น", toys_remaining);
    ESP_LOGI(TAG, "");

    // แบบฝึกหัดที่ 3: เพิ่มของเล่นประเภทอื่น
    int dolls = 5;        // ตุ๊กตา
    int robots = 2;       // หุ่นยนต์
    int total_toys = toys_remaining + dolls + robots;

    ESP_LOGI(TAG, "🪆 ตุ๊กตา: %d ตัว", dolls);
    ESP_LOGI(TAG, "🤖 หุ่นยนต์: %d ตัว", robots);
    ESP_LOGI(TAG, "🎯 ของเล่นทั้งหมด: %d ชิ้น", total_toys);
    ESP_LOGI(TAG, "");

    // แบบฝึกหัดที่ 4: คำถามให้คิด
    int friends = 10;
    int give_each = 2;
    int need_total = friends * give_each;
    int shortfall = (toys_at_home >= need_total) ? 0 : need_total - toys_at_home;

    ESP_LOGI(TAG, "🧠 คำถามให้คิด:");
    ESP_LOGI(TAG, "   ถ้าน้องอยากแจกให้เพื่อน %d คน คนละ %d ชิ้น", friends, give_each);
    ESP_LOGI(TAG, "   ต้องมีของเล่นทั้งหมด %d ชิ้น", need_total);
    if (shortfall == 0) {
        ESP_LOGI(TAG, "   ✅ ของเล่นเพียงพอสำหรับแจก");
    } else {
        ESP_LOGI(TAG, "   ❌ ของเล่นไม่พอ ขาดอีก %d ชิ้น", shortfall);
    }
    ESP_LOGI(TAG, "");

    // เปรียบเทียบกับการบวก
    ESP_LOGI(TAG, "🔄 เปรียบเทียบกับการบวก:");
    ESP_LOGI(TAG, "   การบวก: เพิ่มจำนวน (เช่น ไข่ 4 + 2 = 6)");
    ESP_LOGI(TAG, "   การลบ: ลดจำนวน (เช่น ของเล่น 15 - 7 = 8)");
    ESP_LOGI(TAG, "   ข้อแตกต่าง: การลบต้องระวังไม่ให้ติดลบ!");
    ESP_LOGI(TAG, "");

    // กรณีที่อาจเกิดปัญหา
    int danger_have = 3;
    int danger_give = 5;
    ESP_LOGI(TAG, "⚠️  กรณีที่ต้องระวัง:");
    ESP_LOGI(TAG, "   ถ้าน้องมีของเล่น %d ชิ้น แต่จะแจก %d ชิ้น", danger_have, danger_give);
    ESP_LOGI(TAG, "   จะได้ %d - %d = %d (ผลลัพธ์เป็นลบ!)", 
             danger_have, danger_give, danger_have - danger_give);
    ESP_LOGI(TAG, "   ในชีวิตจริง: น้องไม่สามารถแจกของเล่นมากกว่าที่มีได้");
    ESP_LOGI(TAG, "");

    // สรุปการเรียนรู้
    ESP_LOGI(TAG, "📚 สิ่งที่เรียนรู้:");
    ESP_LOGI(TAG, "   1. การลบเลข (Subtraction): a - b = c");
    ESP_LOGI(TAG, "   2. การตรวจสอบเงื่อนไข (if statement)");
    ESP_LOGI(TAG, "   3. การจัดการกรณีพิเศษ (edge cases)");
    ESP_LOGI(TAG, "   4. ความแตกต่างระหว่างการบวกและการลบ");
    ESP_LOGI(TAG, "   5. การป้องกันผลลัพธ์ที่ไม่สมเหตุสมผล");
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "🎉 จบโปรแกรมนับของเล่นของน้อง!");
    ESP_LOGI(TAG, "📖 อ่านต่อในโปรเจคถัดไป: 03_multiplication_candies");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
}