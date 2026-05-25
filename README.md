# Posture Tracking System

Bu proje, Mikroişlemciler dersi kapsamında geliştirilmiş giyilebilir bir postür (duruş) takip sistemidir. Sistem, kullanıcının duruşunu sürekli olarak izler ve yanlış bir duruş (kambur durma vb.) algıladığında sesli uyarı vererek kullanıcının duruşunu düzeltmesini sağlar.

## Özellikler

- **Çoklu Sensör Verisi:** İki adet MPU6050 (İvmeölçer ve Jiroskop) ve bir adet Flex (Esneme) sensörü kullanılarak vücut hareketleri ve omurga eğriliği hassas bir şekilde ölçülür.
- **Giyilebilir Tasarım:** Sensörler esnek bir lastik bant üzerine dikilerek vücuda rahatça giyilebilir ve günlük kullanıma uygun hale getirilmiştir.
- **Anında Geri Bildirim:** Belirlenen eşik değerlerinin dışında bir duruş algılandığında sistem üzerindeki Buzzer aracılığıyla anında sesli uyarı verilir.

## Kullanılan Donanımlar

- 1x Arduino Uno (Ana kontrolcü)
- 2x MPU6050 (İvmeölçer ve Jiroskop sensörü)
- 1x Flex Sensör (Esneme/Bükülme sensörü)
- 1x Buzzer (Sesli uyarı için)
- Lastik bant (Giyilebilir aparat için)
- Jumper kablolar ve güç kaynağı

## Proje İçeriği

- `posture_tracking_system_code/`: Arduino Uno için yazılmış C++ kaynak kodları.
- `postur_tracking_system.fzz`: Fritzing devre şeması çizimi.
- Sunum dosyaları (`.pptx`) ve proje raporları (`.docx`).

## Çalışma Mantığı

1. Giyilebilir bant kullanıcının sırt bölgesine yerleştirilir.
2. MPU6050 sensörleri kullanıcının sırtının alt ve üst bölgesindeki eğimleri ölçerken, Flex sensör omurganın ne kadar büküldüğünü algılar.
3. Arduino Uno, bu sensörlerden gelen verileri sürekli olarak okur ve analiz eder.
4. Eğer sensör değerleri, kullanıcının kambur veya yanlış bir postürde olduğunu gösteriyorsa (tanımlı eşik değerleri aşıldığında), Arduino Buzzer'ı tetikleyerek kullanıcıyı uyarır.
5. Kullanıcı dik duruş pozisyonuna geri döndüğünde uyarı sesi kesilir.
