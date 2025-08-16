#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// กำหนดชื่อสำหรับแสดงใน log
static const char *TAG = "EGGS_AND_APPLES";

void app_main(void)
{
    ESP_LOGI(TAG, "🥚 เริ่มต้นโปรแกรมนับไข่ไก่และไข่เป็ดของแม่ 🥚");
    ESP_LOGI(TAG, "===========================================");

    // === แบบฝึกหัดที่ 1: เปลี่ยนจำนวนไข่ไก่ ===
    int eggs_already_have = 8;    // แม่มีไข่ไก่ 8 ฟอง
    int eggs_new_today = 3;       // ไก่ออกไข่เพิ่ม 3 ฟอง
    int total_eggs;               // รวมไข่ไก่

    // === แบบฝึกหัดที่ 2: เพิ่มไข่เป็ด ===
    int duck_eggs = 3;            // แม่มีไข่เป็ด 3 ฟอง
    int total_all_eggs;           // รวมไข่ไก่ + ไข่เป็ด

    // แสดงข้อมูลไข่ไก่และไข่เป็ด
    ESP_LOGI(TAG, "📖 โจทย์ไข่:");
    ESP_LOGI(TAG, "   แม่มีไข่ไก่อยู่แล้ว: %d ฟอง", eggs_already_have);
    ESP_LOGI(TAG, "   เช้านี้ไก่ออกไข่เพิ่ม: %d ฟอง", eggs_new_today);
    ESP_LOGI(TAG, "   และแม่มีไข่เป็ด: %d ฟอง", duck_eggs);
    ESP_LOGI(TAG, "");

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    total_eggs = eggs_already_have + eggs_new_today;
    total_all_eggs = total_eggs + duck_eggs;

    ESP_LOGI(TAG, "🧮 คำนวณ:");
    ESP_LOGI(TAG, "   ไข่ไก่ทั้งหมด = %d + %d = %d ฟอง", eggs_already_have, eggs_new_today, total_eggs);
    ESP_LOGI(TAG, "   ไข่ทั้งหมด (รวมไข่เป็ด) = %d + %d = %d ฟอง", total_eggs, duck_eggs, total_all_eggs);
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "🎨 ภาพประกอบ:");
    ESP_LOGI(TAG, "   🐔 ไข่ไก่เดิม: 🥚  %d", eggs_already_have);
    ESP_LOGI(TAG, "   🐣 ไข่ไก่ใหม่: 🥚  %d", eggs_new_today);
    ESP_LOGI(TAG, "   🦆 ไข่เป็ด:    🥚  %d", duck_eggs);
    ESP_LOGI(TAG, "   ✅ รวมทั้งหมด: 🥚  %d", total_all_eggs);
    ESP_LOGI(TAG, "");

    // === แบบฝึกหัดที่ 3: โจทย์ใหม่ - แอปเปิ้ลในตะกร้า ===
    ESP_LOGI(TAG, "🍎 แบบฝึกหัดที่ 3: โจทย์ใหม่ - แอปเปิ้ลในตะกร้า");
    int apples_in_basket = 5;
    int apples_bought_today = 4;
    int total_apples = apples_in_basket + apples_bought_today;

    ESP_LOGI(TAG, "📖 โจทย์:");
    ESP_LOGI(TAG, "   ตอนแรกมีแอปเปิ้ลในตะกร้า: %d ลูก", apples_in_basket);
    ESP_LOGI(TAG, "   วันนี้ไปซื้อมาเพิ่ม: %d ลูก", apples_bought_today);
    ESP_LOGI(TAG, "   ❓ ตอนนี้มีแอปเปิ้ลทั้งหมดกี่ลูก?");
    ESP_LOGI(TAG, "");

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "🧮 คำนวณ:");
    ESP_LOGI(TAG, "   แอปเปิ้ลทั้งหมด = %d + %d = %d ลูก", apples_in_basket, apples_bought_today, total_apples);
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "🎨 ภาพแอปเปิ้ล:");
    ESP_LOGI(TAG, "   🍎  %d (เดิม)", apples_in_basket);
    ESP_LOGI(TAG, "   🍏  %d (ใหม่)", apples_bought_today);
    ESP_LOGI(TAG, "   ✅ รวม: 🍎  %d ลูก", total_apples);
    ESP_LOGI(TAG, "");

    // === สรุปการเรียนรู้ ===
    ESP_LOGI(TAG, "📚 สิ่งที่เรียนรู้:");
    ESP_LOGI(TAG, "   1. การบวกเลข (Addition): a + b = c");
    ESP_LOGI(TAG, "   2. ตัวแปรและชนิดข้อมูล `int`");
    ESP_LOGI(TAG, "   3. การเขียนโจทย์ใหม่จากชีวิตประจำวัน");
    ESP_LOGI(TAG, "   4. การจัดลำดับการแสดงผลในโปรแกรม");
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "🎉 จบโปรแกรมแบบฝึกหัดทั้ง 3 ข้อ!");
    ESP_LOGI(TAG, "📖 อ่านต่อในโปรเจคถัดไป: 02_subtraction_toys");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
}