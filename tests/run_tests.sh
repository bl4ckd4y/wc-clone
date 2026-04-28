#!/usr/bin/env bash
# Простые тесты: сравниваем вывод нашего wc с системным wc.

set -u

WC=./wc
SAMPLE=tests/sample.txt
PASS=0
FAIL=0

assert_eq() {
    local name="$1"
    local expected="$2"
    local actual="$3"

    if [ "$expected" = "$actual" ]; then
        echo "  PASS: $name"
        PASS=$((PASS + 1))
    else
        echo "  FAIL: $name"
        echo "    expected: '$expected'"
        echo "    actual:   '$actual'"
        FAIL=$((FAIL + 1))
    fi
}

# Извлекаем числа из вывода (без имени файла)
nums() {
    awk '{$NF=""; print $0}' | xargs
}

echo "=== wc-clone tests ==="

# Тест 1: -l (lines)
exp=$(wc -l < "$SAMPLE" | xargs)
act=$($WC -l "$SAMPLE" | awk '{print $1}')
assert_eq "wc -l sample.txt" "$exp" "$act"

# Тест 2: -c (bytes)
exp=$(wc -c < "$SAMPLE" | xargs)
act=$($WC -c "$SAMPLE" | awk '{print $1}')
assert_eq "wc -c sample.txt" "$exp" "$act"

# Тест 3: -w (words)
exp=$(wc -w < "$SAMPLE" | xargs)
act=$($WC -w "$SAMPLE" | awk '{print $1}')
assert_eq "wc -w sample.txt" "$exp" "$act"

# Тест 4: без флагов (lines words bytes)
exp=$(wc "$SAMPLE" | awk '{print $1, $2, $3}')
act=$($WC "$SAMPLE" | awk '{print $1, $2, $3}')
assert_eq "wc sample.txt (no flags)" "$exp" "$act"

# Тест 5: два файла, должна появиться строка total
exp=$(wc "$SAMPLE" "$SAMPLE" | tail -n1 | awk '{print $1, $2, $3, $4}')
act=$($WC "$SAMPLE" "$SAMPLE" | tail -n1 | awk '{print $1, $2, $3, $4}')
assert_eq "wc sample.txt sample.txt (total line)" "$exp" "$act"

# Тест 6: -lw (две цифры в выводе, без байт)
exp=$(wc -lw "$SAMPLE" | awk '{print $1, $2}')
act=$($WC -lw "$SAMPLE" | awk '{print $1, $2}')
assert_eq "wc -lw sample.txt" "$exp" "$act"

# Тест 7: неизвестный флаг -> exit 1 (программа не падает)
$WC -x "$SAMPLE" > /dev/null 2>&1
actual_rc=$?
assert_eq "wc -x (invalid flag exit code)" "1" "$actual_rc"

# Тест 8: несуществующий файл -> exit 1
$WC /nonexistent/file > /dev/null 2>&1
actual_rc=$?
assert_eq "wc nonexistent (exit code)" "1" "$actual_rc"

# Тест 9: пустой файл (0 строк, 0 слов, 0 байт)
empty=$(mktemp)
exp=$(wc "$empty" | awk '{print $1, $2, $3}')
act=$($WC "$empty" | awk '{print $1, $2, $3}')
assert_eq "wc empty_file" "$exp" "$act"
rm -f "$empty"

echo ""
echo "Passed: $PASS"
echo "Failed: $FAIL"

if [ $FAIL -gt 0 ]; then
    exit 1
fi
