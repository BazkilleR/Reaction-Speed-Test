# Physical Computing Project 2025 – IT KMITL

## 🚀 Reaction Speed Test (เกมวัดความไวในการตอบสนอง)

> ระบบวัด **Reaction Time** ด้วย Arduino UNO R4 + จอ 4 หลัก TM1637 และหน้าเว็บ (Bootstrap + Chart.js) ที่อ่านข้อมูลผ่าน **Web Serial API**
> โฮสต์บน **GitHub Pages** ใช้ได้ฟรี ไม่ต้องมีเซิร์ฟเวอร์

---

## 👥 สมาชิกกลุ่ม

- นายรัชพล ขาวเขียว — 67070151
- นายเมธาวัสส์ เจียรเศรษฐการ — 67070143
- นายวัทธิกร จุลปาน — 67070163
- นายสุทธิศักดิ์ ราชฉวาง — 67070187
- นายอภิศิษฎ์ นุ่นแก้ว — 67070198

---

## 🔗 เดโมบนเว็บ

- **หน้าแรก (รายละเอียดโปรเจ็กต์):**
  https://bazkiller.github.io/Reaction-Speed-Test/
- **หน้าทดสอบรีเฟลกซ์ (เชื่อมต่อบอร์ดผ่าน Web Serial):**
  https://bazkiller.github.io/Reaction-Speed-Test/reflex.html

> ใช้ **Chrome/Edge บนเดสก์ท็อป** และเปิดผ่าน **HTTPS** (github.io) เท่านั้น
> ปิด Serial Monitor/Plotter ใน Arduino IDE ก่อนกด **Connect** บนหน้าเว็บ

---

## 🎯 วัตถุประสงค์

1. พัฒนาอุปกรณ์สำหรับวัด **ความเร็วการตอบสนอง (Reaction Time)**
2. แสดงผลทันทีบน **TM1637** และบนหน้าเว็บพร้อมสถิติ/กราฟ
3. ดาวน์โหลดผลลัพธ์เป็น **CSV** โดยไม่ต้องมีเซิร์ฟเวอร์

---

## 🧩 ฮาร์ดแวร์ & การต่อสาย

- **บอร์ด:** Arduino **UNO R4** (Minima/WiFi ได้)
- **จอ:** 4-digit **TM1637**
- **ปุ่ม:** D2 ↔ GND (ใช้ `INPUT_PULLUP`)
- **LED:** D4 = แดง, D5 = เขียว (ผ่าน R 220–330Ω ลง GND)

| อุปกรณ์   | พินโมดูล | พินบอร์ด           |
| --------- | -------- | ------------------ |
| TM1637    | VCC      | 5V                 |
|           | GND      | GND                |
|           | CLK      | D3                 |
|           | DIO      | D6                 |
| ปุ่ม      | —        | D2 ↔ GND           |
| LED แดง   | —        | D4 → R → LED → GND |
| LED เขียว | —        | D5 → R → LED → GND |

---

## 🛠️ ซอฟต์แวร์ที่ใช้

- **Arduino IDE** (หรือ PlatformIO)
- ไลบรารี Arduino: **TM1637Display** (Avishay Orpaz)
- **หน้าเว็บ:** Bootstrap 5 + Chart.js + Web Serial
- โฮสต์: **GitHub Pages**

---

## 🔄 ลำดับการทำงาน

1. กดเริ่ม → ไฟ **แดง** ติด ระบบสุ่มรอ 1–10 วินาที
2. ถึงเวลา → ไฟ **เขียว** ติด (READY)
3. ผู้เล่น **กดปุ่ม** → จับเวลา (นับเฉพาะตอนกด)
4. แสดงผลบน TM1637 เป็นรูปแบบ **s.mmm** และส่งลง Serial เช่น [RT] Reaction = 427 ms
5. หน้าเว็บอ่านบรรทัดนี้ → เติมตาราง, คำนวณ **Count / Avg / Best / Worst**, วาดกราฟเส้น + ฮิสโตแกรม และ **ดาวน์โหลด CSV** ได้

---

## ▶️ วิธีรันทดสอบอย่างย่อ

**ฝั่งบอร์ด**

1. ติดตั้งไลบรารี `TM1637Display`
2. อัปโหลดสเก็ตช์ใน `reaction-speed-test-arduino/` (พินตามตารางบน)

**ฝั่งเว็บ**

- เปิดหน้า: https://bazkiller.github.io/Reaction-Speed-Test/reflex.html
- กด **Connect** → เลือกพอร์ต Arduino → เริ่มทดสอบ

---

## 📊 ฟีเจอร์บนหน้าเว็บ

- Connect/Disconnect ผ่าน Web Serial
- ตารางผลพร้อมเวลาและหมายเหตุ
- KPI: **Count / Average / Best / Worst**
- กราฟเส้น (timeline) + ฮิสโตแกรม (ปรับ bin ได้)
- **Download CSV** และ **Clear**
