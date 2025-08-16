#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "CANDIES_MATH";

void app_main(void)
{
    ESP_LOGI(TAG, "🍬 เริ่มต้นโปรแกรมนับลูกอมในถุง 🍬");
    ESP_LOGI(TAG, "====================================");

    // แบบฝึกหัดที่ 1: เปลี่ยนจำนวนถุงและลูกอมต่อถุง
    int candy_bags = 7;         // เปลี่ยนเป็น 7 ถุง
    int candies_per_bag = 8;    // เปลี่ยนเป็นถุงละ 8 เม็ด
    int total_candies = candy_bags * candies_per_bag;

    // แบบฝึกหัดที่ 2: เพิ่มลูกอมหลายรส
    int strawberry_bags = 3;
    int orange_bags = 2;
    int grape_bags = 4;

    int total_flavor_bags = strawberry_bags + orange_bags + grape_bags;
    int total_flavored_candies = total_flavor_bags * candies_per_bag;

    ESP_LOGI(TAG, "📖 โจทย์:");
    ESP_LOGI(TAG, "   มีถุงลูกอม: %d ถุง", candy_bags);
    ESP_LOGI(TAG, "   ถุงละ: %d เม็ด", candies_per_bag);
    ESP_LOGI(TAG, "   ❓ มีลูกอมทั้งหมดกี่เม็ด?");
    ESP_LOGI(TAG, "");
    
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // แสดงขั้นตอนการคิด
    ESP_LOGI(TAG, "🧮 ขั้นตอนการคิด:");
    ESP_LOGI(TAG, "   จำนวนถุง × ลูกอมต่อถุง");
    ESP_LOGI(TAG, "   = %d × %d", candy_bags, candies_per_bag);
    ESP_LOGI(TAG, "   = %d เม็ด", total_candies);
    ESP_LOGI(TAG, "");

    // แสดงคำตอบ
    ESP_LOGI(TAG, "✅ คำตอบ:");
    ESP_LOGI(TAG, "   มีลูกอมทั้งหมด %d เม็ด", total_candies);
    ESP_LOGI(TAG, "");

    // ภาพประกอบ
    for (int i = 1; i <= candy_bags; i++) {
        ESP_LOGI(TAG, "   ถุงที่ %d: 🍬🍬🍬🍬🍬🍬🍬🍬 (%d เม็ด)", i, candies_per_bag);
    }
    ESP_LOGI(TAG, "   รวม:     %d เม็ด", total_candies);
    ESP_LOGI(TAG, "");

    // เปรียบเทียบกับการบวกซ้ำๆ
    ESP_LOGI(TAG, "🔄 เปรียบเทียบกับการบวกซ้ำๆ:");
    int sum_check = 0;
    for (int i = 1; i <= candy_bags; i++) {
        sum_check += candies_per_bag;
        if (i == 1)
            ESP_LOGI(TAG, "                  %d", candies_per_bag);
        else if (i < candy_bags)
            ESP_LOGI(TAG, "                + %d", candies_per_bag);
        else
            ESP_LOGI(TAG, "                + %d = %d", candies_per_bag, sum_check);
    }
    ESP_LOGI(TAG, "   ✅ ผลลัพธ์เหมือนกัน! การคูณคือการบวกซ้ำๆ");
    ESP_LOGI(TAG, "");

    // แบบฝึกหัดที่ 2: รายละเอียดลูกอมรสต่างๆ
    ESP_LOGI(TAG, "🍭 ลูกอมหลายรส:");
    ESP_LOGI(TAG, "🍓 สตรอเบอร์รี่: %d ถุง = %d เม็ด", strawberry_bags, strawberry_bags * candies_per_bag);
    ESP_LOGI(TAG, "🍊 รสส้ม:         %d ถุง = %d เม็ด", orange_bags, orange_bags * candies_per_bag);
    ESP_LOGI(TAG, "🍇 รสองุ่น:       %d ถุง = %d เม็ด", grape_bags, grape_bags * candies_per_bag);
    ESP_LOGI(TAG, "🎯 รวมลูกอมหลายรส: %d เม็ด", total_flavored_candies);
    ESP_LOGI(TAG, "");

    // แบบฝึกหัดที่ 3: ตารางสูตรคูณ
    ESP_LOGI(TAG, "📊 ตารางสูตรคูณของ %d:", candies_per_bag);
    for (int i = 1; i <= 10; i++) {
        ESP_LOGI(TAG, "   %d × %d = %d", i, candies_per_bag, i * candies_per_bag);
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "");

    // แบบฝึกหัดที่ 4: แจกลูกอมให้เพื่อน
    int friends = 12;
    int candies_per_friend = total_candies / friends;
    int remaining_candies = total_candies % friends;

    ESP_LOGI(TAG, "👥 แจกลูกอมให้เพื่อน:");
    ESP_LOGI(TAG, "   เพื่อนทั้งหมด: %d คน", friends);
    ESP_LOGI(TAG, "   ลูกอมทั้งหมด: %d เม็ด", total_candies);
    ESP_LOGI(TAG, "   แต่ละคนจะได้: %d เม็ด", candies_per_friend);
    ESP_LOGI(TAG, "   เหลือเศษลูกอม: %d เม็ด", remaining_candies);
    ESP_LOGI(TAG, "");

    // ตัวอย่างเพิ่มเติม
    ESP_LOGI(TAG, "💡 ตัวอย่างเพิ่มเติม:");
    int ex1_bags = 3, ex1_candies = 8;
    ESP_LOGI(TAG, "   ถ้ามี %d ถุง ถุงละ %d เม็ด = %d เม็ด", ex1_bags, ex1_candies, ex1_bags * ex1_candies);

    int ex2_bags = 7, ex2_candies = 4;
    ESP_LOGI(TAG, "   ถ้ามี %d ถุง ถุงละ %d เม็ด = %d เม็ด", ex2_bags, ex2_candies, ex2_bags * ex2_candies);
    ESP_LOGI(TAG, "");

    // ความรู้เพิ่มเติม
    ESP_LOGI(TAG, "🎓 แนวคิดขั้นสูง:");
    ESP_LOGI(TAG, "   1. การสลับจำนวน: %d × %d = %d × %d = %d", 
             candy_bags, candies_per_bag, candies_per_bag, candy_bags, total_candies);
    ESP_LOGI(TAG, "   2. การคูณด้วย 0: %d × 0 = 0", candy_bags);
    ESP_LOGI(TAG, "   3. การคูณด้วย 1: %d × 1 = %d", candies_per_bag, candies_per_bag);
    ESP_LOGI(TAG, "");

    // สรุป
    ESP_LOGI(TAG, "📚 สิ่งที่เรียนรู้:");
    ESP_LOGI(TAG, "   1. การคูณ (Multiplication)");
    ESP_LOGI(TAG, "   2. การบวกซ้ำ ๆ");
    ESP_LOGI(TAG, "   3. การใช้ for-loop");
    ESP_LOGI(TAG, "   4. การหาร (Division) และการหาเศษ (Modulo)");
    ESP_LOGI(TAG, "   5. การแสดงผลแบบภาพและตาราง");
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "🎉 จบโปรแกรมนับลูกอมในถุง!");
    ESP_LOGI(TAG, "📖 อ่านต่อในโปรเจคถัดไป: 04_division_cookies");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
}
