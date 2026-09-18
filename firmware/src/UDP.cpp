#include "UDP.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "config.h"

static WiFiUDP udp; // tạo đối tượng quản lý thu/phát UDP
static IPAddress unityIP; // biến lưu địa chỉ IPv4 của Unity
static bool unityKnown = false; // cờ đánh dấu đã biết địa chỉ Unity để gửi chưa

void networkInit(){
    WiFi.softAP(AP_SSID, AP_PASS); // bật chế độ Access Point
    // chọn port --> mở phòng Port 4210 ĐỂ NGHE
    // mọi gói tin UDP gửi đến cổng 4210 --> chuyển vào bộ nhớ buffer của udp
    // unity bắn gói PING đến 4210 --> ESP32 bắt nó và học được IP của Unity
    udp.begin(UDP_PORT); 

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP()); 
    // in địa chỉ IP của ESP32 ra (default là 192.168.4.1)
    // DHCP trong ESP32 cũng tự động activate để ready câp IP cho thiết bị kết nối vào
}

/*LẮNG NGHE VÀ HỌC IP*/
void networkPoll(){
    int packetSize = udp.parsePacket(); // kiểm tra bộ nhớ đệm UPD
    if (packetSize){ // kích thước gói tin > 0
        unityIP = udp.remoteIP(); // trích xuất địa chỉ IP bên gửi
        unityKnown = true; // báo cáo đã có IP hợp lệ
        udp.flush(); // xoá sạch dữ liệu
    }
}

/*ĐÓNG GÓI VÀ GỬI DỮ LIỆU*/
void networkSend(const char* payload){
    if(!unityKnown) return; // chưa biết Unity IP --> nghỉ luôn

    udp.beginPacket(unityIP, UDP_PORT);

    // nạp nội dung chuỗi payload vào gói tin
    // payload đại diện cho phần tử đầu tiên của array --> chính là địa chỉ bắt đầu của chuỗi = &payload[0]
    udp.print(payload); // nạp nội dung chuỗi payload vào gói tin
    udp.endPacket(); // đóng gói và đẩy dữ liệu đi
}