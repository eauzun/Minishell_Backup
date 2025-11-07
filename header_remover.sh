#!/usr/bin/env bash

# Tüm .c dosyalarını bul
find . -type f -name "*.c" | while read -r file; do
    echo "Düzenleniyor: $file"

    # Dosyanın içeriğini ilk #include'dan itibaren al
    awk '/#include/{found=1} found' "$file" > "${file}.tmp"

    # Eğer #include bulunduysa dosyayı güncelle
    if [ -s "${file}.tmp" ]; then
        mv "${file}.tmp" "$file"
    else
        rm -f "${file}.tmp"
        echo "Uyarı: $file içinde #include bulunamadı, atlandı."
    fi
done

echo "Tüm header blokları kaldırıldı."

