#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "COOKIES_MATH";

void app_main(void)
{
    ESP_LOGI(TAG, "🍪 เริ่มต้นโปรแกรมแบ่งคุกกี้ 🍪");
    ESP_LOGI(TAG, "================================");

    // 🔢 แบบฝึกหัดที่ 1: เปลี่ยนจำนวนคุกกี้และเพื่อน
    int total_cookies = 24;   // คุกกี้ทั้งหมด
    int number_of_friends = 6; // จำนวนเพื่อน
    int cookies_per_person;
    int remaining_cookies;

    ESP_LOGI(TAG, "📖 โจทย์:");
    ESP_LOGI(TAG, "   มีคุกกี้: %d ชิ้น", total_cookies);
    ESP_LOGI(TAG, "   แบ่งให้เพื่อน: %d คน", number_of_friends);
    ESP_LOGI(TAG, "");

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    if (number_of_friends == 0) {
        ESP_LOGE(TAG, "❌ หารด้วยศูนย์ไม่ได้!");
        ESP_LOGI(TAG, "   คุกกี้จะเหลือทั้งหมด: %d ชิ้น", total_cookies);
        return;
    }

    cookies_per_person = total_cookies / number_of_friends;
    remaining_cookies = total_cookies % number_of_friends;

    // 📝 แบบฝึกหัดที่ 2: ตรวจสอบหารลงตัว
    if (remaining_cookies == 0) {
        ESP_LOGI(TAG, "✅ หารลงตัว! ทุกคนได้เท่ากัน");
    } else {
        ESP_LOGI(TAG, "⚠️ หารไม่ลงตัว! เหลือ %d ชิ้น", remaining_cookies);
    }

    // 🧮 ขั้นตอนการคิด
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "🧮 คำนวณ:");
    ESP_LOGI(TAG, "   %d ÷ %d = %d", total_cookies, number_of_friends, cookies_per_person);
    if (remaining_cookies > 0) {
        ESP_LOGI(TAG, "   เหลือเศษ: %d", remaining_cookies);
    }

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "🎨 ภาพประกอบ:");

    // แสดงคุกกี้ทั้งหมด
    ESP_LOGI(TAG, "   คุกกี้ทั้งหมด: ");
    for (int i = 0; i < total_cookies; i++) {
        printf("🍪");
    }
    printf(" (%d ชิ้น)\n", total_cookies);

    // แสดงการแบ่ง
    for (int i = 1; i <= number_of_friends; i++) {
        printf("   เพื่อน %d: ", i);
        for (int j = 0; j < cookies_per_person; j++) {
            printf("🍪");
        }
        printf(" (%d ชิ้น)\n", cookies_per_person);
    }

    if (remaining_cookies > 0) {
        printf("   เหลือ: ");
        for (int i = 0; i < remaining_cookies; i++) {
            printf("🍪");
        }
        printf(" (%d ชิ้น)\n", remaining_cookies);
    }

    ESP_LOGI(TAG, "");

    // 📝 แบบฝึกหัดที่ 3: หาจำนวนที่หารลงตัว
    int cookies = 30;
    ESP_LOGI(TAG, "🔍 แบบฝึกหัดที่ 3: หาจำนวนที่หารคุกกี้ %d ลงตัว:", cookies);
    for (int people = 1; people <= 10; people++) {
        if (cookies % people == 0) {
            ESP_LOGI(TAG, "   ✅ %d คน → คนละ %d ชิ้น", people, cookies / people);
        }
    }

    ESP_LOGI(TAG, "");

    // 🔄 ความสัมพันธ์กับการคูณ
    ESP_LOGI(TAG, "🔄 ความสัมพันธ์กับการคูณ:");
    ESP_LOGI(TAG, "   การหาร: %d ÷ %d = %d", total_cookies, number_of_friends, cookies_per_person);
    ESP_LOGI(TAG, "   การคูณ: %d × %d = %d", cookies_per_person, number_of_friends, cookies_per_person * number_of_friends);
    if (remaining_cookies > 0) {
        ESP_LOGI(TAG, "   รวมกับเศษ: %d + %d = %d", cookies_per_person * number_of_friends, remaining_cookies, total_cookies);
    }

    ESP_LOGI(TAG, "");

    // 📚 สรุป
    ESP_LOGI(TAG, "📚 สรุปการเรียนรู้:");
    ESP_LOGI(TAG, "   1. การหารเลข: a ÷ b = c");
    ESP_LOGI(TAG, "   2. ใช้ %% เพื่อหาเศษ");
    ESP_LOGI(TAG, "   3. หารด้วยศูนย์ไม่ได้");
    ESP_LOGI(TAG, "   4. ตรวจสอบว่าหารลงตัวหรือไม่");
    ESP_LOGI(TAG, "   5. ความสัมพันธ์กับการคูณ");
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "🎉 จบโปรแกรมแบ่งคุกกี้!");
}
